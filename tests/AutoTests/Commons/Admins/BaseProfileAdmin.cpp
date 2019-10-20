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


#include "BaseProfileAdmin.hpp"

namespace AutoTest
{
  template<>  
  BaseAdminCmd<BaseProfileAdmin, 20>::names_type const  
  BaseAdminCmd<BaseProfileAdmin, 20>::field_names = {  
    "version",    
    "create_time",    
    "history_time",    
    "ignore_fraud_time",    
    "last_request_time",    
    "session_start_time",    
    "colo_timestamps",    
    "page_ht_candidates",    
    "page_history_matches",    
    "page_history_visits",    
    "page_session_matches",    
    "search_ht_candidates",    
    "search_history_matches",    
    "search_history_visits",    
    "search_session_matches",    
    "url_ht_candidates",    
    "url_history_matches",    
    "url_history_visits",    
    "url_session_matches",    
    "audience_channels"    
  };  
  bool  
  BaseProfileAdmin::check(const Expected& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected.values_):  
      !check_(expected.values_);  
  }  
  bool  
  BaseProfileAdmin::check(const std::string& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected):  
      !check_(expected);  
  }  
}
