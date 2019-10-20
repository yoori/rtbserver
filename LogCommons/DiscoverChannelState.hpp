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

#ifndef AD_SERVER_LOG_PROCESSING_DISCOVER_CHANNEL_STATE_HPP
#define AD_SERVER_LOG_PROCESSING_DISCOVER_CHANNEL_STATE_HPP

#include <iosfwd>
#include <Generics/Time.hpp>
#include "LogCommons.hpp"
#include "StatCollector.hpp"


namespace AdServer {
namespace LogProcessing {

class DiscoverChannelStateKey
{
public:

  DiscoverChannelStateKey(
    unsigned long channel_id = 0
  )
  :
    channel_id_(channel_id)
  {
  }

  bool operator==(const DiscoverChannelStateKey &rhs) const
  {
    return channel_id_ == rhs.channel_id_;
  }

  unsigned long channel_id() const
  {
    return channel_id_;
  }

  size_t hash() const
  {
    return channel_id_;
  }

  friend std::istream& operator>>(std::istream &is,
    DiscoverChannelStateKey &key);
  friend std::ostream& operator<<(std::ostream &os,
    const DiscoverChannelStateKey &key) throw(eh::Exception);

private:
  unsigned long channel_id_;
};

class DiscoverChannelStateData
{
public:
  DiscoverChannelStateData()
  :
    time_(),
    total_news_(),
    daily_news_()
  {
  }

  DiscoverChannelStateData(
    const SecondsTimestamp &time,
    unsigned long total_news,
    unsigned long daily_news
  )
  :
    time_(time),
    total_news_(total_news),
    daily_news_(daily_news)
  {
  }

  bool operator==(const DiscoverChannelStateData &rhs) const
  {
    if (&rhs == this)
    {
      return true;
    }
    return time_ == rhs.time_ &&
      total_news_ == rhs.total_news_ &&
      daily_news_ == rhs.daily_news_;
  }

  DiscoverChannelStateData& operator+=(const DiscoverChannelStateData &rhs)
  {
    if (time_ < rhs.time_)
    {
      time_ = rhs.time_;
      total_news_ = rhs.total_news_;
      daily_news_ = rhs.daily_news_;
    }
    return *this;
  }

  const SecondsTimestamp& time() const
  {
    return time_;
  }

  unsigned long total_news() const
  {
    return total_news_;
  }

  unsigned long daily_news() const
  {
    return daily_news_;
  }

  friend std::istream& operator>>(std::istream &is,
    DiscoverChannelStateData &data);
  friend std::ostream& operator<<(std::ostream &os,
    const DiscoverChannelStateData &data);

private:
  SecondsTimestamp time_;
  unsigned long total_news_;
  unsigned long daily_news_;
};

typedef StatCollector<DiscoverChannelStateKey, DiscoverChannelStateData>
  DiscoverChannelStateCollector;

typedef LogDefaultTraits<DiscoverChannelStateCollector, false>
  DiscoverChannelStateTraits;


} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_DISCOVER_CHANNEL_STATE_HPP */

