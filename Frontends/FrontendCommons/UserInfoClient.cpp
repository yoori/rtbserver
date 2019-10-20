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

#include <Commons/CorbaConfig.hpp>
#include <UserInfoSvcs/UserInfoManagerController/UserInfoOperationDistributor.hpp>
#include "UserInfoClient.hpp"

namespace FrontendCommons
{
  UserInfoClient::UserInfoClient(
    const UserInfoManagerControllerGroupSeq& user_info_manager_controller_group,
    const CORBACommons::CorbaClientAdapter* corba_client_adapter,
    Logging::Logger* logger)
    throw()
  {
    AdServer::UserInfoSvcs::UserInfoOperationDistributor::
      ControllerRefList controller_groups;

    for(UserInfoManagerControllerGroupSeq::const_iterator cg_it =
        user_info_manager_controller_group.begin();
        cg_it != user_info_manager_controller_group.end();
        ++cg_it)
    {
      AdServer::UserInfoSvcs::UserInfoOperationDistributor::
        ControllerRef controller_ref_group;

      Config::CorbaConfigReader::read_multi_corba_ref(
        *cg_it,
        controller_ref_group);

      controller_groups.push_back(controller_ref_group);
    }

    AdServer::UserInfoSvcs::UserInfoOperationDistributor_var distributor =
      new AdServer::UserInfoSvcs::UserInfoOperationDistributor(
        logger,
        controller_groups,
        corba_client_adapter);
    user_info_matcher_ = ReferenceCounting::add_ref(distributor);
    add_child_object(distributor);
  }

  AdServer::UserInfoSvcs::UserInfoMatcher*
  UserInfoClient::user_info_session() throw()
  {
    return AdServer::UserInfoSvcs::
      UserInfoManagerSession::_duplicate(user_info_matcher_);
  }
}
