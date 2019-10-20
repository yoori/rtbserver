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


#ifndef __AUTOTESTS_COMMONS_ADMINS_REACHPROFILEADMIN_HPP
#define __AUTOTESTS_COMMONS_ADMINS_REACHPROFILEADMIN_HPP

#include "Admins.hpp"

namespace AutoTest
{
  class ReachProfileAdmin:  
    public BaseAdminCmd<ReachProfileAdmin, 6>  
  {  
    public:    
      class Expected      
      {      
        friend class ReachProfileAdmin;        
        public:        
          Expected() {}          
          Expected& last_request_day (const std::string& val);          
          Expected& total_appear_campaigns (const std::string& val);          
          Expected& total_appear_ccgs (const std::string& val);          
          Expected& total_appear_creatives (const std::string& val);          
          Expected& total_appear_advs (const std::string& val);          
          Expected& total_appear_display_advs (const std::string& val);          
                
          enum FieldName          
          {          
            LAST_REQUEST_DAY = 0,            
            TOTAL_APPEAR_CAMPAIGNS,            
            TOTAL_APPEAR_CCGS,            
            TOTAL_APPEAR_CREATIVES,            
            TOTAL_APPEAR_ADVS,            
            TOTAL_APPEAR_DISPLAY_ADVS            
          };          
        protected:        
          FieldIndexMap values_;          
      };      
        
    public:    
      typedef BaseAdminCmd<ReachProfileAdmin, FIELDS_COUNT> Base;    
        
      void make_cmd (const char* address,      
        const char* uuid,      
        RequestInfoSrv service      
      )      
      {      
        address_ = address;        
        AdminParams params;        
        {        
          params.push_back(AdminParamPair("", uuid ? uuid : ""));          
        }        
        make_admin_cmd(*this, "print-reach", address, params, static_cast<size_t>(service));      
        add_cmd_i("--align");      
      }      
      ReachProfileAdmin(      
        const char* address,      
        const char* uuid,      
        RequestInfoSrv service = RequestInfoManager      
      )      
      {      
        make_cmd(address, uuid, service);      
      }      
      ReachProfileAdmin(      
        const std::string& address,      
        const char* uuid,      
        RequestInfoSrv service = RequestInfoManager      
      )      
      {      
        make_cmd(address.c_str(), uuid, service);      
      }      
      ReachProfileAdmin(      
        const char* address,      
        const std::string& uuid,      
        RequestInfoSrv service = RequestInfoManager      
      )      
      {      
        make_cmd(address, uuid.c_str(), service);      
      }      
      ReachProfileAdmin(      
        const std::string& address,      
        const std::string& uuid,      
        RequestInfoSrv service = RequestInfoManager      
      )      
      {      
        make_cmd(address.c_str(), uuid.c_str(), service);      
      }      
      const char* last_request_day (unsigned int i = 0);      
      const char* total_appear_campaigns (unsigned int i = 0);      
      const char* total_appear_ccgs (unsigned int i = 0);      
      const char* total_appear_creatives (unsigned int i = 0);      
      const char* total_appear_advs (unsigned int i = 0);      
      const char* total_appear_display_advs (unsigned int i = 0);      
      bool check(const Expected& expected, bool exist = true)      
        throw(eh::Exception);      
      bool check(const std::string& expected, bool exist = true)      
        throw(eh::Exception);      
  };  
    
}
namespace AutoTest
{
  inline  
  ReachProfileAdmin::Expected&  
  ReachProfileAdmin::Expected::last_request_day(const std::string& val)  
  {  
     values_.insert(std::make_pair(LAST_REQUEST_DAY, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ReachProfileAdmin::last_request_day (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::LAST_REQUEST_DAY].c_str();  
  }  
  inline  
  ReachProfileAdmin::Expected&  
  ReachProfileAdmin::Expected::total_appear_campaigns(const std::string& val)  
  {  
     values_.insert(std::make_pair(TOTAL_APPEAR_CAMPAIGNS, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ReachProfileAdmin::total_appear_campaigns (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::TOTAL_APPEAR_CAMPAIGNS].c_str();  
  }  
  inline  
  ReachProfileAdmin::Expected&  
  ReachProfileAdmin::Expected::total_appear_ccgs(const std::string& val)  
  {  
     values_.insert(std::make_pair(TOTAL_APPEAR_CCGS, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ReachProfileAdmin::total_appear_ccgs (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::TOTAL_APPEAR_CCGS].c_str();  
  }  
  inline  
  ReachProfileAdmin::Expected&  
  ReachProfileAdmin::Expected::total_appear_creatives(const std::string& val)  
  {  
     values_.insert(std::make_pair(TOTAL_APPEAR_CREATIVES, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ReachProfileAdmin::total_appear_creatives (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::TOTAL_APPEAR_CREATIVES].c_str();  
  }  
  inline  
  ReachProfileAdmin::Expected&  
  ReachProfileAdmin::Expected::total_appear_advs(const std::string& val)  
  {  
     values_.insert(std::make_pair(TOTAL_APPEAR_ADVS, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ReachProfileAdmin::total_appear_advs (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::TOTAL_APPEAR_ADVS].c_str();  
  }  
  inline  
  ReachProfileAdmin::Expected&  
  ReachProfileAdmin::Expected::total_appear_display_advs(const std::string& val)  
  {  
     values_.insert(std::make_pair(TOTAL_APPEAR_DISPLAY_ADVS, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ReachProfileAdmin::total_appear_display_advs (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::TOTAL_APPEAR_DISPLAY_ADVS].c_str();  
  }  
}
#endif  // __AUTOTESTS_COMMONS_ADMINS_REACHPROFILEADMIN_HPP

