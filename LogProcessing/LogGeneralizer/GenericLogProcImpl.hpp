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

#ifndef AD_SERVER_LOG_PROCESSING_GENERIC_LOG_PROC_IMPL_HPP
#define AD_SERVER_LOG_PROCESSING_GENERIC_LOG_PROC_IMPL_HPP


#include <iosfwd>
#include <string>
#include <fstream>

#include <Generics/Function.hpp>
#include <Logger/Logger.hpp>

#include <LogCommons/LogCommons.hpp>
#include "LogVersionManager.hpp"
#include "ProcStatImpl.hpp"

namespace AdServer {
namespace LogProcessing {

template <
  class LOG_TYPE_EXT_TRAITS_,
  class LOG_VERSION_MANAGER_ = LogVersionManager<LOG_TYPE_EXT_TRAITS_>
>
class GenericLogProcessorImpl: public LogProcessorImplBase
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  typedef LOG_TYPE_EXT_TRAITS_ Traits;
  typedef LOG_VERSION_MANAGER_ LogVersionManagerT;

  GenericLogProcessorImpl(
    const LogProcThreadInfo_var& context,
    const std::string &in_dir,
    const std::string &out_dir,
    const CollectorBundleParams &bundle_params,
    const ProcStatImpl_var& proc_stat_impl,
    const FileReceiverInterrupter_var& fr_interrupter
  )
  :
    LogProcessorImplBase(in_dir, context->logger),
    proc_stat_(proc_stat_impl),
    log_version_manager_(new LogVersionManagerT(context, out_dir,
      bundle_params))
  {
    create_file_receiver_(fr_interrupter);
  }

  GenericLogProcessorImpl(
    const std::string &in_dir,
    LOG_VERSION_MANAGER_ *log_version_manager,
    Logging::Logger *logger,
    const ProcStatImpl_var& proc_stat_impl,
    const FileReceiverInterrupter_var& fr_interrupter
  )
  :
    LogProcessorImplBase(in_dir, logger),
    proc_stat_(proc_stat_impl),
    log_version_manager_(log_version_manager)
  {
    create_file_receiver_(fr_interrupter);
  }

  GenericLogProcessorImpl(
    const LogProcThreadInfo_var& context,
    const std::string &in_dir,
    const std::string &out_dir,
    const CollectorBundleParams &bundle_params,
    HitsFilter_var &hits_filter,
    const ProcStatImpl_var& proc_stat_impl,
    const FileReceiverInterrupter_var& fr_interrupter
  )
  :
    LogProcessorImplBase(in_dir, context->logger),
    proc_stat_(proc_stat_impl),
    log_version_manager_(new LogVersionManagerT(
      context, out_dir, bundle_params, hits_filter))
  {
    create_file_receiver_(fr_interrupter);
  }

  virtual
  void
  check_and_load();

  virtual
  const FileReceiver_var&
  get_file_receiver()
  {
    return file_receiver_;
  }

protected:
  virtual
  ~GenericLogProcessorImpl() throw() {}

private:
  typedef std::unique_ptr<LogVersionManagerT> LogVersionManagerPtrT;
  typedef ProcStatsValues<LOG_TYPE_EXT_TRAITS_> ProcStatT;

  void
  create_file_receiver_(const FileReceiverInterrupter_var& fr_interrupter)
  {
    fr_interrupter_ = fr_interrupter;

    std::string fr_intermed_dir = in_dir_;

    (fr_intermed_dir += '/') += FR_INTERMED_DIR;
    file_receiver_ = new FileReceiver(fr_intermed_dir.c_str(),
      FR_MAX_FILES_TO_STORE, fr_interrupter, 0);
  }

  ProcStatT proc_stat_;
  LogVersionManagerPtrT log_version_manager_;
  FileReceiverInterrupter_var fr_interrupter_;
  FileReceiver_var file_receiver_;
};

template <class L_TRAITS_, class LVM_>
void
GenericLogProcessorImpl<L_TRAITS_, LVM_>::check_and_load()
{
  static unsigned num_input_ops = 0;

  static Generics::Time total_elapsed_time;
  static Generics::Time total_file_proc_elapsed_time;

  unsigned long processed_files_count = 0;

  Generics::Time start_time(Generics::Time::get_time_of_day());

  Generics::Timer file_proc_timer;
  Generics::Time file_processing_max_time;

  if (file_receiver_->empty())
  {
    Traits::search_files(in_dir_, file_receiver_);
  }

  FileReceiver::FileGuard_var file;
  std::string tmp;
  while (fr_interrupter_->active() && (file = file_receiver_->get_eldest(tmp)))
  {
    if (logger_->log_level() >= Logging::Logger::TRACE)
    {
      logger_->sstream(Logging::Logger::TRACE)
        << FNT << ": processing file '" << file->full_path() << '\'';
    }

    file_proc_timer.start();

    std::ifstream ifs(file->full_path().c_str());
    if (!ifs)
    {
      char buf[128];
      Stream::Error es;
      es << FNT << "Failed to open file '" << file->full_path() << "'."
         << eh::ErrnoHelper::error_message(buf,
              strerror_r(errno, buf, sizeof(buf))) << " Skipping ...";
      logger_->error(es.str(), 0, LOG_GEN_IMPL_ERR_CODE_8);
      continue;
    }
    try
    {
      log_version_manager_->load(ifs, file);
      ++processed_files_count;
    }
    catch (const eh::Exception &ex)
    {
      proc_stat_->template add<Generics::Values::UnsignedInt>(
        proc_stat_.input_error_count, 1);

      logger_->sstream(
        Logging::Logger::ERROR,
        0/*ASPECT*/,
        LOG_GEN_IMPL_ERR_CODE_5) << FNT << ": Exception: " << ex.what();

      if (logger_->log_level() >= Logging::Logger::TRACE)
      {
        logger_->sstream(Logging::Logger::TRACE)
          << FNT << ": Trying to save file '"
          << file->full_path() << "' in " << DEFAULT_ERROR_DIR << "/";
      }
      try
      {
        FileStore(in_dir_, DEFAULT_ERROR_DIR).store(file->full_path());
      }
      catch (const eh::Exception &ex)
      {
        logger_->sstream(
          Logging::Logger::ERROR,
          0/*ASPECT*/,
          LOG_GEN_IMPL_ERR_CODE_6) << FNT << ": Exception: " << ex.what();
      }
    }

    file_proc_timer.stop_add(total_file_proc_elapsed_time);
    const Generics::Time &file_time = file_proc_timer.elapsed_time();
    if (file_processing_max_time < file_time)
    {
      file_processing_max_time = file_time;
      proc_stat_->set_if_gt_time(proc_stat_.input_time_per_file_max,
        file_time);
    }
  }

  if (processed_files_count)
  {
    proc_stat_->template stat_processed_files<ProcStatT>(
      total_elapsed_time, total_file_proc_elapsed_time,
      start_time, num_input_ops, processed_files_count);
  }
}

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_GENERIC_LOG_PROC_IMPL_HPP */
