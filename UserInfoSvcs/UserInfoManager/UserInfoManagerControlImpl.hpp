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

#ifndef _USER_INFO_SVCS_USER_INFO_MANAGER_CONTROL_IMPL_HPP_
#define _USER_INFO_SVCS_USER_INFO_MANAGER_CONTROL_IMPL_HPP_

#include <string>

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Sync/SyncPolicy.hpp>

#include <UserInfoSvcs/UserInfoManager/UserInfoManagerControl_s.hpp>
#include <UserInfoSvcs/UserInfoManager/UserInfoManagerImpl.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    /**
     * Implementation of UserInfoManagerControl.
     */
    class UserInfoManagerControlImpl :
      public virtual CORBACommons::ReferenceCounting::ServantImpl<
        POA_AdServer::UserInfoSvcs::UserInfoManagerControl>
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      
      UserInfoManagerControlImpl(
        UserInfoManagerImpl* user_info_manager_impl)
        throw(Exception);

      virtual ~UserInfoManagerControlImpl() throw();

      /** UserInfoManagerControl interface */
      virtual AdServer::UserInfoSvcs::UserInfoManagerStatus status() throw();

      virtual void 
      get_source_info(
        AdServer::UserInfoSvcs::ChunksConfig_out user_info_source)
        throw(AdServer::UserInfoSvcs::UserInfoManagerControl::ImplementationException);

      virtual void admit() throw();

    protected:
      typedef Sync::PosixRWLock Mutex_;
      typedef Sync::PosixRGuard ReadGuard_;
      typedef Sync::PosixWGuard WriteGuard_;
     
      mutable Mutex_ lock_;
      
      Logging::Logger_var logger_;
      UserInfoManagerImpl_var user_info_manager_impl_;
      bool admitted_;
    };

    typedef
      ReferenceCounting::SmartPtr<UserInfoManagerControlImpl>
      UserInfoManagerControlImpl_var;

  } /* UserInfoSvcs */
} /* AdServer */

#endif /*_USER_INFO_SVCS_USER_INFO_MANAGER_CONTROL_IMPL_HPP_*/
