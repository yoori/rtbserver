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

#ifndef _USER_INFO_MANAGER_WD_FREQCAP_PROFILE_HPP_
#define _USER_INFO_MANAGER_WD_FREQCAP_PROFILE_HPP_

#include <string>
#include <set>
#include <vector>

#include <Generics/MemBuf.hpp>
#include <Generics/Time.hpp>

#include <Commons/FreqCap.hpp>

#include "Allocator.hpp"

#include <UserInfoSvcs/UserInfoCommons/WDFreqCapProfileDescription.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    typedef std::vector<Generics::Time> TimeList;

    struct NewsItemImpression
    {
      std::string id;
      unsigned long total;
      TimeList times;
    };

    typedef std::list<NewsItemImpression> NewsItemImpressionList;

    struct IdImpression
    {
      unsigned long id;
      unsigned long total;
      TimeList times;
    };

    typedef std::list<IdImpression> IdImpressionList;

    class WDFreqCapProfile
    {
    public:
      typedef std::set<std::string> NewsIdSet;
      typedef std::set<unsigned long> IdSet;

      WDFreqCapProfile(SmartMemBufPtr plain_profile);

      void get_impressions(
        NewsItemImpressionList* news_ids,
        IdImpressionList* category_ids,
        IdImpressionList* channel_ids) const;

      /* add new impressions to profile and clean its by .._fc values */
      void fill_impressions(
        const Generics::Time& time,
        const AdServer::Commons::FreqCap& event_fc,
        const AdServer::Commons::FreqCap& category_fc,
        const AdServer::Commons::FreqCap& channel_fc,
        const NewsIdSet& news_imps,
        const IdSet& category_imps,
        const IdSet& channel_imps);

      std::ostream& print(std::ostream& out, const char* prefix = "") const;

      static
      std::ostream& print(
        std::ostream& out,
        const void* buf,
        unsigned long buf_size,
        const char* prefix = "");

    private:
      SmartMemBuf_var plain_profile_;
    };
  }
}

#endif /*_USER_INFO_MANAGER_WD_FREQCAP_PROFILE_HPP_*/


