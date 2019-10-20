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

#ifndef AD_SERVER_LOG_PROCESSING_DISCOVER_FEED_STATE_HPP
#define AD_SERVER_LOG_PROCESSING_DISCOVER_FEED_STATE_HPP

#include <iosfwd>
#include <sstream>
#include <Generics/Time.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include "LogCommons.hpp"
#include "StatCollector.hpp"


namespace AdServer {
namespace LogProcessing {

class DiscoverFeedStateKey
{
public:
  DiscoverFeedStateKey(
    unsigned long feed_id = 0
  )
  :
    feed_id_(feed_id)
  {
  }

  bool operator==(const DiscoverFeedStateKey &rhs) const
  {
    return feed_id_ == rhs.feed_id_;
  }

  unsigned long feed_id() const
  {
    return feed_id_;
  }

  size_t hash() const
  {
    return feed_id_;
  }

  friend std::istream& operator>>(std::istream &is,
    DiscoverFeedStateKey &key);
  friend std::ostream& operator<<(std::ostream &os,
    const DiscoverFeedStateKey &key) throw(eh::Exception);

private:
  unsigned long feed_id_;
};

class DiscoverFeedStateData
{
public:
  DiscoverFeedStateData()
  :
    time_(),
    total_news_()
  {
  }

  DiscoverFeedStateData(
    const SecondsTimestamp &time,
    unsigned long total_news
  )
  :
    time_(time),
    total_news_(total_news)
  {
  }

  bool operator==(const DiscoverFeedStateData &rhs) const
  {
    if (&rhs == this)
    {
      return true;
    }
    return time_ == rhs.time_ && total_news_ == rhs.total_news_;
  }

  DiscoverFeedStateData& operator+=(const DiscoverFeedStateData &rhs)
  {
    if (time_ < rhs.time_)
    {
      time_ = rhs.time_;
      total_news_ = rhs.total_news_;
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

  friend std::istream& operator>>(std::istream &is,
    DiscoverFeedStateData &data);
  friend std::ostream& operator<<(std::ostream &os,
    const DiscoverFeedStateData &data);

private:
  SecondsTimestamp time_;
  unsigned long total_news_;
};


typedef StatCollector<DiscoverFeedStateKey, DiscoverFeedStateData>
  DiscoverFeedStateCollector;

typedef LogDefaultTraits<DiscoverFeedStateCollector, false>
  DiscoverFeedStateTraits;



} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_DISCOVER_FEED_STATE_HPP */

