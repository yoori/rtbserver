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


#ifndef __AUTOTESTS_COMMONS_ADMINS_CREATIVECATEGORYADMIN_HPP
#define __AUTOTESTS_COMMONS_ADMINS_CREATIVECATEGORYADMIN_HPP

#include "Admins.hpp"

namespace AutoTest
{
  class CreativeCategoryAdmin:  
    public BaseAdminCmd<CreativeCategoryAdmin, 2>  
  {  
    public:    
      class Expected      
      {      
        friend class CreativeCategoryAdmin;        
        public:        
          Expected() {}          
          Expected& creative_category_id (const std::string& val);          
          Expected& creative_category_id(unsigned long val);          
          Expected& timestamp (const std::string& val);          
                
          enum FieldName          
          {          
            CREATIVE_CATEGORY_ID = 0,            
            TIMESTAMP            
          };          
        protected:        
          FieldIndexMap values_;          
      };      
        
    public:    
      typedef BaseAdminCmd<CreativeCategoryAdmin, FIELDS_COUNT> Base;    
        
      void make_cmd (const char* address,      
        const char* category      
      )      
      {      
        address_ = address;        
        AdminParams params;        
        if (category)        
        {        
          params.push_back(AdminParamPair("creative_category_id=", category));          
        }        
        make_admin_cmd(*this, "creative_categories", address, params, static_cast<size_t>(CampaignManager));      
      }      
      CreativeCategoryAdmin(      
        const char* address,      
        const char* category      
      )      
      {      
        make_cmd(address, category);      
      }      
      CreativeCategoryAdmin(      
        const std::string& address,      
        const char* category      
      )      
      {      
        make_cmd(address.c_str(), category);      
      }      
      CreativeCategoryAdmin(      
        const char* address,      
        const std::string& category      
      )      
      {      
        make_cmd(address, category.c_str());      
      }      
      CreativeCategoryAdmin(      
        const std::string& address,      
        const std::string& category      
      )      
      {      
        make_cmd(address.c_str(), category.c_str());      
      }      
      CreativeCategoryAdmin(      
        const char* address,      
        unsigned long category      
      )      
      {      
        make_cmd(address, strof(category).c_str());      
      }      
      CreativeCategoryAdmin(      
        const std::string& address,      
        unsigned long category      
      )      
      {      
        make_cmd(address.c_str(), strof(category).c_str());      
      }      
      const char* creative_category_id (unsigned int i = 0);      
      const char* timestamp (unsigned int i = 0);      
      bool check(const Expected& expected, bool exist = true)      
        throw(eh::Exception);      
      bool check(const std::string& expected, bool exist = true)      
        throw(eh::Exception);      
  };  
    
}
namespace AutoTest
{
  inline  
  CreativeCategoryAdmin::Expected&  
  CreativeCategoryAdmin::Expected::creative_category_id(const std::string& val)  
  {  
     values_.insert(std::make_pair(CREATIVE_CATEGORY_ID, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  CreativeCategoryAdmin::creative_category_id (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::CREATIVE_CATEGORY_ID].c_str();  
  }  
  inline  
  CreativeCategoryAdmin::Expected&  
  CreativeCategoryAdmin::Expected::creative_category_id(unsigned long val)  
  {  
     values_.insert(std::make_pair(CREATIVE_CATEGORY_ID, ComparableRegExp(strof(val))));  
     return *this;  
  }  
  inline  
  CreativeCategoryAdmin::Expected&  
  CreativeCategoryAdmin::Expected::timestamp(const std::string& val)  
  {  
     values_.insert(std::make_pair(TIMESTAMP, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  CreativeCategoryAdmin::timestamp (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::TIMESTAMP].c_str();  
  }  
}
#endif  // __AUTOTESTS_COMMONS_ADMINS_CREATIVECATEGORYADMIN_HPP

