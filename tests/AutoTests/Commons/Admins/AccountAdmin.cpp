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


#include "AccountAdmin.hpp"

namespace AutoTest
{
  template<>  
  BaseAdminCmd<AccountAdmin, 20>::names_type const  
  BaseAdminCmd<AccountAdmin, 20>::field_names = {  
    "account_id",    
    "agency_account_id",    
    "internal_account_id",    
    "status",    
    "eval_status",    
    "flags",    
    "at_flags",    
    "text_adserving",    
    "currency_id",    
    "country",    
    "commision",    
    "budget",    
    "paid_amount",    
    "time_offset",    
    "walled_garden_accounts",    
    "auction_rate",    
    "use_pub_pixels",    
    "pub_pixel_optin",    
    "pub_pixel_optout",    
    "timestamp"    
  };  
  bool  
  AccountAdmin::check(const Expected& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected.values_):  
      !check_(expected.values_);  
  }  
  bool  
  AccountAdmin::check(const std::string& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected):  
      !check_(expected);  
  }  
}
