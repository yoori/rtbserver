/* 
 * This file is part of the RTBServer distribution (https://github.com/yoori/rtbserver).
 * RTBServer is DSP server that allow to bid (see RTB auction) targeted ad
 * via RTB protocols (OpenRTB, Google AdExchange, Yandex RTB)
 *
 * Copyright (c) 2014 Yuri Kuznecov <yuri.kuznecov@gmail.com>.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOG_SAVE_PROCESSOR_HPP
#define LOG_SAVE_PROCESSOR_HPP

#include <LogCommons/LogSaverBaseImpl.hpp>
#include "ErrorCode.hpp"
#include "ProcStatImpl.hpp"
#include "DbConnectionFactory.hpp"

namespace AdServer {
namespace LogProcessing {

namespace {

const char DEFAULT_ERROR_DIR[] = "Error";
const char DEFAULT_PG_ERROR_DIR[] = "Error/PG";
const char DEFAULT_CSV_ERROR_DIR[] = "Error/CSV";
const char BACKUP_MODE_DIR_SUFFIX[] = ".bak";

const char FR_INTERMED_DIR[] = "Intermediate";
const std::size_t FR_MAX_FILES_TO_STORE = 50000;
}

template <typename LogExtTraits>
class LogSaveProcessor : public ReferenceCounting::AtomicImpl
{
public:
  typedef ReferenceCounting::SmartPtr<LogSaveProcessor<LogExtTraits> >
    SmartPtr;
  typedef typename SpilloverImpl<typename
    LogExtTraits::CollectorBundleType::CollectorT>::SmartPtr
    Spillover_var;

  typedef typename LogSaverBaseImpl<
    typename LogExtTraits::CollectorBundleType>::SmartPtr LogSaverBase_var;
  typedef ProcStatsValues<LogExtTraits> ProcStatT;

  LogSaveProcessor(
    const LogSaverBase_var& saver,
    const ProcStatImpl_var& proc_stat_impl,
    const std::string& in_dir,
    bool backup_mode,
    Logging::Logger* logger)
    throw ()
    : saver_(saver),
      proc_stat_(proc_stat_impl),
      in_dir_(in_dir),
      logger_(ReferenceCounting::add_ref(logger))
  {
    init_backup_file_store_(backup_mode);
  }

  /**
   * Must call with not empty @param data.
   */
  void
  dump(Generics::Timer& dump_timer, const Spillover_var& data);

private:
  virtual
  ~LogSaveProcessor() throw ()
  {}

  typedef std::unique_ptr<FileStore> FileStorePtrT;

  void
  init_backup_file_store_(bool backup_mode);

  void
  unlink_files_(const CollectorBundleFileList& file_list);

  void
  handle_save_exception_(
    const Spillover_var& data,
    const char* exception_name,
    const char* error_folder_name,
    const char* ex_descr,
    const char* ex_code = 0)
    throw ();

  LogSaverBase_var saver_;
  ProcStatT proc_stat_;
  const std::string in_dir_;
  Logging::Logger_var logger_;
  FileStorePtrT backup_file_store_;
};

template <typename LogExtTraits>
void
LogSaveProcessor<LogExtTraits>::dump(
  Generics::Timer& dump_timer, const Spillover_var& data)
{
  try
  {
    saver_->save(data);
    unlink_files_(data->files());
    std::size_t collector_size = data->collector_size;
    // Clear switch off errors counting in handle_save_exception_
    data->clear();
    proc_stat_.stat_num_entries(dump_timer, collector_size);
  }
  catch (const DbConnectionFactory::NotActive&)
  {
    if (logger_->log_level() >= Logging::Logger::TRACE)
    {
      logger_->sstream(Logging::Logger::TRACE)
        << FNT << "Db dump cancelled, DB isn't active";
    }
    throw;
  }
  catch (const typename LogSaverBaseImpl<
    typename LogExtTraits::CollectorBundleType>::OperationDeferred& ex)
  {
    handle_save_exception_(
      data,
      "LogSaver::OperationDeferred",
      0,// 0 means that files should be moved into Deferred, instead Error folder
      ex.what(),
      ex.code());
  }
  catch (const typename SaveEx::PgException& ex)
  {
    handle_save_exception_(
      data,
      "PgSaverType::PgException",
      DEFAULT_PG_ERROR_DIR,
      ex.what(),
      ex.code());
  }
  catch (const typename SaveEx::CsvException &ex)
  {
    handle_save_exception_(
      data,
      "CsvSaverType::CsvException",
      DEFAULT_CSV_ERROR_DIR,
      ex.what(),
      ex.code());
  }
  catch (const eh::DescriptiveException& ex)
  {
    handle_save_exception_(
      data,
      "Exception",
      DEFAULT_ERROR_DIR,
      ex.what(),
      ex.code());
  }
  catch (const std::exception& ex)
  {
    handle_save_exception_(
      data,
      "std::exception",
      DEFAULT_ERROR_DIR,
      ex.what(),
      LOG_GEN_IMPL_ERR_CODE_7);
  }
}

template <typename LogExtTraits>
void
LogSaveProcessor<LogExtTraits>::handle_save_exception_(
  const Spillover_var& data,
  const char* exception_name,
  const char* error_folder_name,
  const char* ex_descr,
  const char* ex_code)
  throw ()
{
  logger_->sstream(Logging::Logger::ERROR, 0, ex_code)
    << FNT << "caught " << exception_name << ": " << ex_descr;
  if (logger_->log_level() >= Logging::Logger::TRACE)
  {
    logger_->sstream(Logging::Logger::TRACE)
      << FNT << "Trying to save files ["
      << data->files() << "] in " << error_folder_name << "/";
  }
  try
  {
    // Must move files to Error or Deferred folder
    LogExtTraits::reject_files(
      data->files(), in_dir_, error_folder_name, data->save_time);
    if (!data->files().empty())
    {
      data->clear();
      proc_stat_->template add<Generics::Values::UnsignedInt>(
        ProcStatT::output_error_count, 1);
    }
  }
  catch (const eh::Exception &ex)
  {
    logger_->sstream(Logging::Logger::ERROR, 0, LOG_GEN_IMPL_ERR_CODE_6)
      << FNT << "processing " << exception_name << ": " << ex.what();
  }
}

template <typename LogExtTraits>
void
LogSaveProcessor<LogExtTraits>::init_backup_file_store_(bool backup_mode)
{
  if (backup_mode)
  {
    std::string backup_dir = in_dir_;
    backup_dir += BACKUP_MODE_DIR_SUFFIX;
    backup_file_store_.reset(new FileStore(backup_dir, ".", false));
  }
}

template <typename LogExtTraits>
void
LogSaveProcessor<LogExtTraits>::unlink_files_(
  const CollectorBundleFileList& file_list)
{
  if (backup_file_store_.get())
  {
    backup_file_store_->store(file_list);
  }
  else
  {
    for (CollectorBundleFileList::const_iterator it = file_list.begin();
      it != file_list.end(); ++it)
    {
      unlink((*it)->full_path().c_str());
    }
  }
}

} // namespace LogProcessing
} // namespace AdServer

#endif // LOG_SAVE_PROCESSOR_HPP
