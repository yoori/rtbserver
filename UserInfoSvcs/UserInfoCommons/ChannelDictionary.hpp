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

#ifndef _USER_INFO_SVCS_CHANNEL_DICTIONARY_HPP_
#define _USER_INFO_SVCS_CHANNEL_DICTIONARY_HPP_

#include <set>
#include <map>

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <Generics/Time.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Generics/HashTableAdapters.hpp>

#include "Allocator.hpp"

#include <Generics/TAlloc.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    typedef Generics::TAlloc::AllocOnly<void*, 256, true> HashAllocOnly;
    typedef Generics::TAlloc::ThreadPool<void*, 256> ThreadPoolAlloc;

    typedef std::map<
      unsigned long,
      unsigned long,
      std::less<unsigned long>,
      ThreadPoolAlloc> ChannelMatchMap; // channel_id => weight

    typedef std::list<unsigned long, ThreadPoolAlloc> ChannelIdList;
    typedef std::list<unsigned long, ThreadPoolAlloc> CategoryIdList;
    typedef std::map<unsigned long, CategoryIdList, std::less<unsigned long>, HashAllocOnly>
      CategoryMap;

    const char AUDIENCE_CHANNEL = 'A';
    const char URL_CHANNEL = 'U';
    const char PAGE_CHANNEL = 'P';
    const char SEARCH_CHANNEL = 'S';
    const char URL_KEYWORD_CHANNEL = 'R';

    struct PrefChannelInterval
    {
      PrefChannelInterval()
        : max_time_from(0),
          max_time_to(0),
          max_visits(0),
          min_window_size(-1)
      {}

      Generics::Time max_time_from;
      Generics::Time max_time_to;
      unsigned long max_visits;
      unsigned long min_window_size;
    };


    struct ChannelInterval
    {
      ChannelInterval() {}

      ChannelInterval(
        const Generics::Time& time_from_in,
        const Generics::Time& time_to_in,
        const unsigned long min_visits_in,
        const unsigned long weight_in)
        : time_from(time_from_in),
          time_to(time_to_in),
          min_visits(min_visits_in),
          weight(weight_in)
      {}

      Generics::Time time_from;
      Generics::Time time_to;
      unsigned long min_visits;
      unsigned long weight;
    };

    class ChannelIntervalList: protected std::list<ChannelInterval, HashAllocOnly>
    {
    public:
      typedef std::list<ChannelInterval, HashAllocOnly> BaseChannelIntervalList;
      typedef BaseChannelIntervalList::const_iterator const_iterator;
      typedef BaseChannelIntervalList::const_reverse_iterator const_reverse_iterator;

      const_iterator begin() const
      {
        return BaseChannelIntervalList::begin();
      }

      const_iterator end() const
      {
        return BaseChannelIntervalList::end();
      }

      const_reverse_iterator rbegin() const
      {
        return BaseChannelIntervalList::rbegin();
      }

      const_reverse_iterator rend() const
      {
        return BaseChannelIntervalList::rend();
      }

      bool empty() const
      {
        return BaseChannelIntervalList::empty();
      }

      unsigned long size() const
      {
        return BaseChannelIntervalList::size();
      }

      void insert(const ChannelInterval& val)
      {
        iterator it = BaseChannelIntervalList::begin();

        while (it != BaseChannelIntervalList::end() &&
               it->time_from < val.time_from)
        {
          ++it;
        }

        if (it != BaseChannelIntervalList::end())
        {
          BaseChannelIntervalList::insert(it, val);
        }
        else
        {
          BaseChannelIntervalList::push_back(val);
        }
/*
        if (val.time_to.tv_sec - val.time_from.tv_sec <
            pref_channel_interval_.time_to.tv_sec -
            pref_channel_interval_.time_from.tv_sec)
        {
          pref_channel_interval_.time_to = val.time_to;
          pref_channel_interval_.time_from = val.time_from;
        }
*/
        if (static_cast<unsigned long>(val.time_to.tv_sec - val.time_from.tv_sec) <
            pref_channel_interval_.min_window_size)
        {
          pref_channel_interval_.min_window_size =
            val.time_to.tv_sec - val.time_from.tv_sec;
        }

        if (val.min_visits > pref_channel_interval_.max_visits)
        {
          pref_channel_interval_.max_visits = val.min_visits;
        }

        if (val.time_to > pref_channel_interval_.max_time_to)
        {
          pref_channel_interval_.max_time_to = val.time_to;
        }
      }

      unsigned long max_visits() const
      {
        return pref_channel_interval_.max_visits;
      }

      unsigned long min_window_size() const
      {
        return pref_channel_interval_.min_window_size;
      }

      unsigned long max_time_to() const
      {
        return pref_channel_interval_.max_time_to.tv_sec;
      }

    private:
      PrefChannelInterval pref_channel_interval_;
    };

    struct ChannelIntervalsPack: public ReferenceCounting::AtomicImpl
    {
      ChannelIntervalsPack(): contextual(false), zero_channel(false), weight(0)
      {}

      bool contextual;
      bool zero_channel;
      unsigned long weight;

      ChannelIntervalList short_intervals;
      ChannelIntervalList today_long_intervals;
      ChannelIntervalList long_intervals;

    protected:
      virtual ~ChannelIntervalsPack() throw () {}
    };

    struct ChannelFeatures
    {
      ChannelFeatures(
        bool discover_val,
        unsigned long threshold_val)
        : discover(discover_val),
          threshold(threshold_val)
      {}
      
      bool discover;
      unsigned long threshold;
    };
    
    typedef ReferenceCounting::SmartPtr<ChannelIntervalsPack>
      ChannelIntervalsPack_var;

    typedef Generics::NumericHashAdapter<unsigned long> ChannelIdHash;
    typedef Generics::GnuHashTable<ChannelIdHash, ChannelIntervalsPack_var, HashAllocOnly>
      ChannelsHashMap;

    typedef Generics::GnuHashTable<ChannelIdHash, ChannelFeatures, HashAllocOnly>
      ChannelFeaturesMap;

    struct ChannelDictionary: public ReferenceCounting::AtomicImpl
    {
      ChannelsHashMap audience_channels;
      ChannelsHashMap page_channels;
      ChannelsHashMap search_channels;
      ChannelsHashMap url_channels;
      ChannelsHashMap url_keyword_channels;
      
      CategoryMap channel_categories;
      ChannelFeaturesMap channel_features;

    protected:
      virtual
      ~ChannelDictionary() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<ChannelDictionary> ChannelDictionary_var;
  }
}

#endif /*_USER_INFO_SVCS_CHANNEL_DICTIONARY_HPP_*/
