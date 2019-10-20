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


#include "RequestKeywordProfileAdmin.hpp"

namespace AutoTest
{
  template<>  
  BaseAdminCmd<RequestKeywordProfileAdmin, 20>::names_type const  
  BaseAdminCmd<RequestKeywordProfileAdmin, 20>::field_names = {  
    "time",    
    "colo_id",    
    "tag_id",    
    "ccg_id",    
    "cc_id",    
    "channel_id",    
    "position",    
    "num_shown",    
    "page_last_match",    
    "page_hourly_matches",    
    "page_daily_matches",    
    "page_weekly_matches",    
    "search_last_match",    
    "search_hourly_matches",    
    "search_daily_matches",    
    "search_weekly_matches",    
    "ccg_imps",    
    "cc_imps",    
    "channel_imps",    
    "click_done"    
  };  
  bool  
  RequestKeywordProfileAdmin::check(const Expected& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected.values_):  
      !check_(expected.values_);  
  }  
  bool  
  RequestKeywordProfileAdmin::check(const std::string& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected):  
      !check_(expected);  
  }  
}
