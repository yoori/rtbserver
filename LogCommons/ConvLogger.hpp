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

#ifndef AD_SERVER_LOG_COMMONS_CONVLOGGER_HPP
#define AD_SERVER_LOG_COMMONS_CONVLOGGER_HPP

#include <iosfwd>
#include <string>
#include <ostream>

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Commons/Containers.hpp>
#include <Commons/UserInfoManip.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <LogCommons/LogCommons.hpp>
#include <LogCommons/StatCollector.hpp>

#include <LogCommons/GenericLogIoImpl.hpp>
#include <LogCommons/CsvUtils.hpp>

namespace AdServer
{
namespace LogProcessing
{
  class ConvData
  {
    SecondsTimestamp time_;
    UserIdIoWrapper user_id_;
    unsigned long action_id_;

  public:
    const SecondsTimestamp&
    time() const throw ()
    {
      return time_;
    }

    const UserId&
    user_id() const throw ()
    {
      return user_id_;
    }

    unsigned long
    action_id() const throw ()
    {
      return action_id_;
    }

    ConvData() throw ()
    {}

    ConvData(
      const SecondsTimestamp& time_val,
      const UserId& user_id_val,
      unsigned long action_id_val)
      : time_(time_val),
        user_id_(user_id_val),
        action_id_(action_id_val)
    {}

    bool
    operator==(const ConvData& data) const
    {
      if (this == &data)
      {
        return true;
      }

      return time_ == data.time_ &&
        user_id_ == data.user_id_ &&
        action_id_ == data.action_id_;
    }

    unsigned long distrib_hash() const
    {
      return user_id_distribution_hash(user_id_);
    }

    friend FixedBufStream<TabCategory>&
      operator>>(FixedBufStream<TabCategory>& is, ConvData& data)
      throw(eh::Exception);

    friend std::ostream&
    operator<<(std::ostream& os, const ConvData& data)
      throw(eh::Exception);
  };

  typedef SeqCollector<ConvData, true> ConvCollector;

  struct ConvTraits:
    LogDefaultTraits<ConvCollector, false, false>
  {
    template <class FUNCTOR_>
    static void for_each_old(FUNCTOR_& /*f*/) throw(eh::Exception)
    {}

    typedef GenericLogIoHelperImpl<ConvTraits> IoHelperType;
  };
} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_COMMONS_CONVLOGGER_HPP */

