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

#ifndef AD_SERVER_LOG_PROCESSING_CHANNEL_OVERLAP_USER_STAT_HPP
#define AD_SERVER_LOG_PROCESSING_CHANNEL_OVERLAP_USER_STAT_HPP


#include <iosfwd>
#include <Generics/Time.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include "LogCommons.hpp"
#include "StatCollector.hpp"


namespace AdServer {
namespace LogProcessing {

class ChannelOverlapUserStatInnerKey
{
public:
  ChannelOverlapUserStatInnerKey()
  :
    channel1_id_(),
    channel2_id_(),
    hash_()
  {
  }

  ChannelOverlapUserStatInnerKey(
    unsigned long channel1_id,
    unsigned long channel2_id
  )
  :
    channel1_id_(channel1_id),
    channel2_id_(channel2_id),
    hash_()
  {
    calc_hash_();
  }

  bool operator==(const ChannelOverlapUserStatInnerKey& rhs) const
  {
    if (&rhs == this)
    {
      return true;
    }
    return channel1_id_ == rhs.channel1_id_ &&
      channel2_id_ == rhs.channel2_id_;
  }

  unsigned long channel1_id() const
  {
    return channel1_id_;
  }

  unsigned long channel2_id() const
  {
    return channel2_id_;
  }

  size_t hash() const
  {
    return hash_;
  }

  friend
  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is,
    ChannelOverlapUserStatInnerKey& key)
    throw(eh::Exception);

  friend
  std::ostream&
  operator<<(std::ostream& os, const ChannelOverlapUserStatInnerKey& key)
    throw(eh::Exception);

private:
  void calc_hash_()
  {
    Generics::Murmur64Hash hasher(hash_);
    hash_add(hasher, channel1_id_);
    hash_add(hasher, channel2_id_);
  }

  unsigned long channel1_id_;
  unsigned long channel2_id_;
  size_t hash_;
};

class ChannelOverlapUserStatInnerData
{
public:
  ChannelOverlapUserStatInnerData()
  :
    unique_users_()
  {
  }

  explicit
  ChannelOverlapUserStatInnerData(
    unsigned long unique_users
  )
  :
    unique_users_(unique_users)
  {
  }

  bool operator==(const ChannelOverlapUserStatInnerData& rhs) const
  {
    if (&rhs == this)
    {
      return true;
    }
    return unique_users_ == rhs.unique_users_;
  }

  ChannelOverlapUserStatInnerData&
  operator+=(const ChannelOverlapUserStatInnerData& rhs)
  {
    unique_users_ += rhs.unique_users_;
    return *this;
  }

  unsigned long unique_users() const
  {
    return unique_users_;
  }

  friend
  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is,
    ChannelOverlapUserStatInnerData& data)
    throw(eh::Exception);

  friend
  std::ostream&
  operator<<(std::ostream& os, const ChannelOverlapUserStatInnerData& data)
    throw(eh::Exception);

private:
  unsigned long unique_users_;
};


typedef StatCollector<
          ChannelOverlapUserStatInnerKey,
          ChannelOverlapUserStatInnerData,
          false,
          true
        > ChannelOverlapUserStatInnerCollector;

typedef DayTimestamp ChannelOverlapUserStatKey;
typedef ChannelOverlapUserStatInnerCollector ChannelOverlapUserStatData;

typedef StatCollector<ChannelOverlapUserStatKey, ChannelOverlapUserStatData>
  ChannelOverlapUserStatCollector;

typedef LogDefaultTraits<ChannelOverlapUserStatCollector>
  ChannelOverlapUserStatTraits;


} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_CHANNEL_OVERLAP_USER_STAT_HPP */

