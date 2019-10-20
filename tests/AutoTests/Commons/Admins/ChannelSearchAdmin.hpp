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


#ifndef __AUTOTESTS_COMMONS_ADMINS_CHANNELSEARCHADMIN_HPP
#define __AUTOTESTS_COMMONS_ADMINS_CHANNELSEARCHADMIN_HPP

#include "Admins.hpp"

namespace AutoTest
{
  class ChannelSearchAdmin:  
    public BaseAdminCmd<ChannelSearchAdmin, 1>  
  {  
    public:    
      class Expected      
      {      
        friend class ChannelSearchAdmin;        
        public:        
          Expected() {}          
          Expected& channel_id (const std::string& val);          
                
          enum FieldName          
          {          
            CHANNEL_ID = 0            
          };          
        protected:        
          FieldIndexMap values_;          
      };      
        
    public:    
      typedef BaseAdminCmd<ChannelSearchAdmin, FIELDS_COUNT> Base;    
        
      void make_cmd (const char* address,      
        const char* phrase,      
        ChannelSrv service      
      )      
      {      
        address_ = address;        
        AdminParams params;        
        if (phrase)        
        {        
          params.push_back(AdminParamPair("--phrase=", phrase));          
        }        
        make_admin_cmd(*this, "search", address, params, static_cast<size_t>(service));      
      }      
      ChannelSearchAdmin(      
        const char* address,      
        const char* phrase,      
        ChannelSrv service = ChannelSearch      
      )      
      {      
        make_cmd(address, phrase, service);      
      }      
      ChannelSearchAdmin(      
        const std::string& address,      
        const char* phrase,      
        ChannelSrv service = ChannelSearch      
      )      
      {      
        make_cmd(address.c_str(), phrase, service);      
      }      
      ChannelSearchAdmin(      
        const char* address,      
        const std::string& phrase,      
        ChannelSrv service = ChannelSearch      
      )      
      {      
        make_cmd(address, phrase.c_str(), service);      
      }      
      ChannelSearchAdmin(      
        const std::string& address,      
        const std::string& phrase,      
        ChannelSrv service = ChannelSearch      
      )      
      {      
        make_cmd(address.c_str(), phrase.c_str(), service);      
      }      
      const char* channel_id (unsigned int i = 0);      
      bool check(const Expected& expected, bool exist = true)      
        throw(eh::Exception);      
      bool check(const std::string& expected, bool exist = true)      
        throw(eh::Exception);      
  };  
    
}
namespace AutoTest
{
  inline  
  ChannelSearchAdmin::Expected&  
  ChannelSearchAdmin::Expected::channel_id(const std::string& val)  
  {  
     values_.insert(std::make_pair(CHANNEL_ID, ComparableRegExp(val)));  
     return *this;  
  }  
  inline  
  const char*  
  ChannelSearchAdmin::channel_id (unsigned int i)  
  {  
    if (empty() || values_[0].size() != FIELDS_COUNT) return 0;  
    return values_[i][Expected::CHANNEL_ID].c_str();  
  }  
}
#endif  // __AUTOTESTS_COMMONS_ADMINS_CHANNELSEARCHADMIN_HPP

