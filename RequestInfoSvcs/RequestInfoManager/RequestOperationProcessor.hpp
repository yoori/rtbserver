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

#ifndef REQUESTOPERATIONPROCESSOR_HPP
#define REQUESTOPERATIONPROCESSOR_HPP

#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/MemBuf.hpp>

#include <Commons/UserInfoManip.hpp>

#include "RequestActionProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  struct RequestOperationProcessor:
    public virtual ReferenceCounting::Interface
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    virtual void
    process_impression(
      const ImpressionInfo& impression_info)
      throw(Exception) = 0;

    virtual void
    process_action(
      const AdServer::Commons::UserId& new_user_id,
      RequestContainerProcessor::ActionType action_type,
      const Generics::Time& time,
      const AdServer::Commons::RequestId& request_id)
      throw(Exception) = 0;

    virtual void
    process_impression_post_action(
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const RequestPostActionInfo& request_post_action_info)
      throw(Exception) = 0;

    virtual void
    change_request_user_id(
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const Generics::ConstSmartMemBuf* request_profile)
      throw(Exception) = 0;

  protected:
    virtual ~RequestOperationProcessor() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<RequestOperationProcessor>
    RequestOperationProcessor_var;
}
}

#endif /*REQUESTOPERATIONPROCESSOR_HPP*/
