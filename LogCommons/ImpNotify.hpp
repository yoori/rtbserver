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

#ifndef AD_SERVER_LOG_COMMONS_IMP_NOTIFY_HPP
#define AD_SERVER_LOG_COMMONS_IMP_NOTIFY_HPP


#include <iosfwd>
#include <ostream>

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <LogCommons/LogCommons.hpp>
#include <LogCommons/StatCollector.hpp>

#include <LogCommons/GenericLogIoImpl.hpp>

namespace AdServer {
namespace LogProcessing {

struct ImpNotifyData
{
  SecondsTimestamp time;
  UserId user_id;
  NumberList channels;
  FixedNumber revenue;

  ImpNotifyData(): revenue(FixedNumber::ZERO) {}

  ImpNotifyData(
    const SecondsTimestamp& time_val,
    const UserId& user_id_val,
    const NumberList& channels_val,
    const FixedNumber& revenue_val
  )
  :
    time(time_val),
    user_id(user_id_val),
    channels(channels_val),
    revenue(revenue_val)
  {
  }

  bool operator==(const ImpNotifyData& data) const
  {
    if (this == &data)
    {
      return true;
    }
    return time == data.time &&
      user_id == data.user_id &&
      channels == data.channels &&
      revenue == data.revenue;
  }

  unsigned long distrib_hash() const
  {
    return user_id_distribution_hash(user_id);
  }

  void invariant() const throw(ConstraintViolation)
  {
    if (user_id.is_null())
    {
      Stream::Error es;
      es << "ImpNotifyData::invariant(): user_id is NULL";
      throw ConstraintViolation(es);
    }
  }

  friend FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ImpNotifyData& data)
    throw(eh::Exception);

  friend std::ostream&
  operator<<(std::ostream& os, const ImpNotifyData& data)
    throw(eh::Exception);
};

typedef SeqCollector<ImpNotifyData, true> ImpNotifyCollector;

struct ImpNotifyTraits: LogDefaultTraits<ImpNotifyCollector, false, false>
{
  typedef GenericLogIoHelperImpl<ImpNotifyTraits> IoHelperType;
};

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_COMMONS_IMP_NOTIFY_HPP */

