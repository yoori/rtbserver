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


#include "ColocationAdmin.hpp"

namespace AutoTest
{
  template<>  
  BaseAdminCmd<ColocationAdmin, 4>::names_type const  
  BaseAdminCmd<ColocationAdmin, 4>::field_names = {  
    "colo_id",    
    "colo_rate_id",    
    "account_id",    
    "revenue_share"    
  };  
  bool  
  ColocationAdmin::check(const Expected& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected.values_):  
      !check_(expected.values_);  
  }  
  bool  
  ColocationAdmin::check(const std::string& expected, bool exist)  
    throw(eh::Exception)  
  {  
    return exist? check_(expected):  
      !check_(expected);  
  }  
}
