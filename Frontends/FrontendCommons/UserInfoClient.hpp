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

#ifndef _USER_INFO_CLIENT_
#define _USER_INFO_CLIENT_

#include <eh/Exception.hpp>
#include <Sync/SyncPolicy.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <xsd/Frontends/FeConfig.hpp>
#include <UserInfoSvcs/UserInfoManagerController/UserInfoOperationDistributor.hpp>

namespace FrontendCommons
{
  class UserInfoClient:
      public virtual ReferenceCounting::AtomicImpl,
      public Generics::CompositeActiveObject
  {
  public:
    typedef xsd::AdServer::Configuration::
      CommonFeConfigurationType::UserInfoManagerControllerGroup_sequence
      UserInfoManagerControllerGroupSeq;

  public:
    UserInfoClient(
      const UserInfoManagerControllerGroupSeq& user_info_manager_controller_group,
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      Logging::Logger* logger)
      throw();

    virtual ~UserInfoClient() throw() {};

    AdServer::UserInfoSvcs::UserInfoMatcher*
    user_info_session() throw();

  private:
    AdServer::UserInfoSvcs::UserInfoMatcher_var user_info_matcher_;
  };

  typedef ReferenceCounting::SmartPtr<UserInfoClient> UserInfoClient_var;
}

#endif /* _USER_INFO_CLIENT_ */
