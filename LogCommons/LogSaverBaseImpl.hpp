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

#ifndef LOGSAVERBASEIMPL_HPP_INCLUDED
#define LOGSAVERBASEIMPL_HPP_INCLUDED

#include <LogCommons/LogCommons.hpp>

namespace AdServer {
namespace LogProcessing {

  template <typename CollectorType>
  class SpilloverImpl : public ReferenceCounting::AtomicImpl
  {
  public:
    typedef ReferenceCounting::SmartPtr<SpilloverImpl<CollectorType> >
      SmartPtr;

    SpilloverImpl() throw ()
      : collector_size(0)
    {}

    void
    clear() throw ()
    {
      collector.clear();
      files_.clear();
      collector_size = 0;
      save_time = Generics::Time::ZERO;
      min_file_timestamp_.clear();
    }

    void
    push_back(const CollectorBundleFileGuard_var& file_handle)
      throw (eh::Exception)
    {
      files_.push_back(file_handle);
      const std::string& new_file_name = file_handle->file_name();
      if (!min_file_timestamp_.empty())
      {
        if (new_file_name < min_file_timestamp_)
        {
          min_file_timestamp_ = new_file_name;
        }
      }
      else
      {
        min_file_timestamp_ = new_file_name;
      }
    }

    const CollectorBundleFileList&
    files() const throw ()
    {
      return files_;
    }

    Generics::Time
    get_min_file_timestamp() const throw (InvalidLogFileNameFormat)
    {
      if (min_file_timestamp_.empty())
      {
        return Generics::Time::ZERO;
      }
      LogFileNameInfo info;
      parse_log_file_name(min_file_timestamp_, info);
      return info.timestamp;
    }

    CollectorType collector;
    std::size_t collector_size;
    Generics::Time save_time;
  private:
    virtual
    ~SpilloverImpl() throw ()
    {}

    CollectorBundleFileList files_;
    std::string min_file_timestamp_;
  };

  template <typename CollectorBundleType>
  class LogSaverBaseImpl : public virtual LogSaver
  {
  public:
    typedef ReferenceCounting::SmartPtr<LogSaverBaseImpl<CollectorBundleType> >
      SmartPtr;
    typedef typename SpilloverImpl<
      typename CollectorBundleType::CollectorT>::SmartPtr Spillover_var;
    typedef CollectorFilter<typename CollectorBundleType::CollectorT>
      CollectorFilterType;

    LogSaverBaseImpl(
      CollectorFilterType* collector_filter)
      : collector_filter_(ReferenceCounting::add_ref(collector_filter))
    {}

    virtual
    void
    save(const Spillover_var& data) = 0;

  protected:
    typename CollectorFilterType::SmartPtr collector_filter_;
    typedef typename CollectorBundleType::CollectorT CollectorT;

    virtual
    ~LogSaverBaseImpl() throw ()
    {}

  };

} // namespace LogProcessing
} // namespace AdServer

#endif // LOGSAVERBASEIMPL_HPP_INCLUDED
