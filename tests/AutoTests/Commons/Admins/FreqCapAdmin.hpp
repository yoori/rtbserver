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


#ifndef __AUTOTESTS_COMMONS_ADMINS_FREQCAPADMIN_HPP
#define __AUTOTESTS_COMMONS_ADMINS_FREQCAPADMIN_HPP

#include "Admins.hpp"

namespace AutoTest
{
  class FreqCapAdmin:  
    public BaseAdminCmd<FreqCapAdmin, 3>  
  {  
    public:    
      class Expected      
      {      
        friend class FreqCapAdmin;        
        public:        
          Expected() {}          
          Expected& id (const std::string& val);          
          Expected& id(unsigned long val);          
          Expected& window_time (const std::string& val);          
          Expected& window_limit (const std::string& val);          
                
          enum FieldName          
          {          
            ID = 0,            
            WINDOW_TIME,            
            WINDOW_LIMIT            
          };          
        protected:        
          FieldIndexMap values_;          
      };      
        
    public:    
      typedef BaseAdminCmd<FreqCapAdmin, FIELDS_COUNT> Base;    
        
      void make_cmd (const char* address,      
        const char* freqcap      
      )      
      {      
        address_ = address;        
        AdminParams params;        
        if (freqcap)        
        {        
          params.push_back(AdminParamPair("id=", freqcap));          
        }        
        make_admin_cmd(*this, "freq_cap", address, params, static_cast<size_t>(CampaignManager));      
      }      
      FreqCapAdmin(      
        const char* address,      
        const char* freqcap      
      )      
      {      
        make_cmd(address, freqcap);      
      }      
      FreqCapAdmin(      
        const std::string& address,      
        const char* freqcap      
      )      
      {      
        make_cmd(address.c_str(), freqcap);      
      }      
      FreqCapAdmin(      
        const char* address,      
        const std::string& freqcap      
      )      
      {      
        make_cmd(address, freqcap.c_str());      
      }      
      FreqCapAdmin(      
        const std::string& address,      
        const std::string& freqcap      
      )      
      {      
        make_cmd(address.c_str(), freqcap.c_str());      
      }      
      FreqCapAdmin(      
        const char* address,      
        unsigned long freqcap      
      )      
      {      
        make_cmd(address, strof(freqcap).c_str());      
      }      
      FreqCapAdmin(      
        const std::string& address,      
        unsigned long freqcap      
      )      
      {      
        make_cmd(address.c_str(), strof(freqcap).c_str());      
      }      
      const char* id (unsigned int i = 0);      
      const char* window_time (unsigned int i = 0);      
      const char* window_limit (unsigned int i = 0);      
      bool check(const Expected& expected, bool exist = true)      
        throw(eh::Exception);      
      bool check(const std::string& expected, bool exist = true)      
        throw(eh::Exception);      
  };  
    
}
namespace AutoTest
{
  inline  
  FreqCapAdmin::Expected&  
  FreqCapAdmin::Expected::id(const std::string& val)  
  {  
     values_.insert(std::make_pair(ID, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  FreqCapAdmin::id (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::ID].c_str();  
  }  
  inline  
  FreqCapAdmin::Expected&  
  FreqCapAdmin::Expected::id(unsigned long val)  
  {  
     values_.insert(std::make_pair(ID, ComparableRegExp(strof(val))));  
     return *this;  
  }  
  inline  
  FreqCapAdmin::Expected&  
  FreqCapAdmin::Expected::window_time(const std::string& val)  
  {  
     values_.insert(std::make_pair(WINDOW_TIME, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  FreqCapAdmin::window_time (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::WINDOW_TIME].c_str();  
  }  
  inline  
  FreqCapAdmin::Expected&  
  FreqCapAdmin::Expected::window_limit(const std::string& val)  
  {  
     values_.insert(std::make_pair(WINDOW_LIMIT, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  FreqCapAdmin::window_limit (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::WINDOW_LIMIT].c_str();  
  }  
}
#endif  // __AUTOTESTS_COMMONS_ADMINS_FREQCAPADMIN_HPP

