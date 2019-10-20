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


#include "CampaignAdmin.hpp"

namespace AutoTest
{
  template<>  
  BaseAdminCmd<CampaignAdmin, 23>::names_type const  
  BaseAdminCmd<CampaignAdmin, 23>::field_names = {  
    "id",    
    "account_id",    
    "account_currency_id",    
    "campaign_group_name",    
    "channels",    
    "ecpm",    
    "delivery_threshold",    
    "status",    
    "eval_status",    
    "timestamp",    
    "exclude_pub_accounts",    
    "exclude_tags",    
    "date_start",    
    "date_end",    
    "budget",    
    "daily_budget",    
    "cmp_date_start",    
    "cmp_date_end",    
    "max_pub_share",    
    "channel",    
    "flags",    
    "country",    
    "sites"    
  };  
  bool  
  CampaignAdmin::check(const Expected& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected.values_):  
      !check_(expected.values_);  
  }  
  bool  
  CampaignAdmin::check(const std::string& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected):  
      !check_(expected);  
  }  
  const char*  
  CampaignAdmin::modificator_names_[1] = {  
    "--expand"    
  };  
}
