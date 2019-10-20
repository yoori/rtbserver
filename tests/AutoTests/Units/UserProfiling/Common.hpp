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


#include <tests/AutoTests/Commons/Common.hpp>

#ifndef _USERPROFILING_COMMONS_HPP
#define _USERPROFILING_COMMONS_HPP


namespace AutoTest
{
  namespace UserProfiling
  {

    // Utils

    /**
     * @brief prepare advertising request.
     * @param test unit.
     * @param advertising request.
     * @param request parameters.
     * @param using debug time sign.
     */
    template<typename RequestType>
    void make_request(
      BaseUnit* test,
      NSLookupRequest& request,
      const RequestType& params,
      const AutoTest::Time& base_time = Generics::Time::ZERO)
    {
      if (params.referer_kw)
      {
        std::string refererkw_names(params.referer_kw);
        std::string referer_kw;
        String::StringManip::SplitComma tokenizer(refererkw_names);
        String::SubString token;
        while (tokenizer.get_token(token))
        {
          String::StringManip::trim(token);
          if (!referer_kw.empty()) referer_kw+=",";
          referer_kw += test->fetch_string(token.str());
        }
        
        request.referer_kw = referer_kw;
      }
      if (base_time != Generics::Time::ZERO)
      {
        request.debug_time = base_time + params.time_ofset;
      }
    }
    
  }
}


#endif  // _USERPROFILING_COMMONS_HPP
