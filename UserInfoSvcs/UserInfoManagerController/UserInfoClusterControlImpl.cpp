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

#include <UserInfoSvcs/UserInfoManagerController/UserInfoClusterControlImpl.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    UserInfoClusterControlImpl::UserInfoClusterControlImpl()
      throw()
    {}
    
    UserInfoClusterControlImpl::UserInfoClusterControlImpl(
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      const UIMRefVector& uims,
      const std::vector<std::string>& hosts)
      throw()
      : hosts_(hosts)
    {
      assert(uims.size() == hosts.size());

      try
      {
        CORBACommons::CorbaClientAdapter_var cca(
          ReferenceCounting::add_ref(corba_client_adapter));

        for (unsigned int i = 0; i < uims.size(); ++i)
        {
          CORBA::Object_var obj = cca->resolve_object(uims[i]);

          if (CORBA::is_nil(obj.in()))
          {
            uims_.clear();
            break;
          }

          AdServer::UserInfoSvcs::UserInfoManager_var
            user_info_manager =
            AdServer::UserInfoSvcs::UserInfoManager::_narrow(obj.in());
          
          if (!CORBA::is_nil(user_info_manager.in()))
          {
            uims_.push_back(user_info_manager);
          }
          else
          {
            uims_.clear();
            break;
          }
        }
      }
      catch(...)
      {
        uims_.clear();
      }
    }
    
    UserInfoClusterControlImpl::~UserInfoClusterControlImpl()
      throw()
    {}

    CORBACommons::IProcessControl::ALIVE_STATUS
    UserInfoClusterControlImpl::is_alive() throw()
    {
      bool uim_status = true;
      
      try
      {
        if (uims_.size() == 0)
        {
          return CORBACommons::IProcessControl::AS_NOT_ALIVE;
        }
        
        for (unsigned int i = 0; i < uims_.size(); ++i)
        {
          uim_status = uim_status && uims_[i]->uim_ready();
        }
      }
      catch(const CORBA::SystemException& e)
      {
        return CORBACommons::IProcessControl::AS_NOT_ALIVE;
      }

      if (uim_status == true)
      {
        return CORBACommons::IProcessControl::AS_READY;
      }

      return CORBACommons::IProcessControl::AS_ALIVE;
    }

    char* UserInfoClusterControlImpl::comment() throw(CORBACommons::OutOfMemory)
    {
      try
      {
        Stream::Error str;
        for (unsigned int i = 0; i < uims_.size(); ++i)
        {
          if (!uims_[i]->uim_ready())
          {
            str << hosts_[i] << ": " << uims_[i]->get_progress() << ";";
          }
        }

        CORBA::String_var result;
        result << str.str();
        return result._retn();
      }
      catch(const eh::Exception& e)
      {
        throw CORBACommons::OutOfMemory();
      }
    }
  }
}

