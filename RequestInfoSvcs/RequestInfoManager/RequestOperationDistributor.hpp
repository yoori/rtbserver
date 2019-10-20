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

#ifndef REQUESTOPERATIONDISTRIBUTOR_HPP
#define REQUESTOPERATIONDISTRIBUTOR_HPP

#include "RequestOperationProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  class RequestOperationDistributor:
    public ReferenceCounting::AtomicImpl,
    public RequestOperationProcessor
  {
  public:
    RequestOperationDistributor(
      unsigned long distrib_count,
      unsigned long index,
      unsigned long services_count,
      RequestOperationProcessor* request_operation_processor,
      RequestOperationProcessor* other_request_operation_processor)
      throw();

    virtual void
    process_impression(
      const ImpressionInfo& impression_info)
      throw(RequestOperationProcessor::Exception);

    virtual void
    process_action(
      const AdServer::Commons::UserId& new_user_id,
      RequestContainerProcessor::ActionType action_type,
      const Generics::Time& time,
      const AdServer::Commons::RequestId& request_id)
      throw(RequestOperationProcessor::Exception);

    virtual void
    process_impression_post_action(
      const AdServer::Commons::UserId& user_id,
      const AdServer::Commons::RequestId& request_id,
      const RequestPostActionInfo& request_post_action_info)
      throw(RequestOperationProcessor::Exception);

    virtual void
    change_request_user_id(
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const Generics::ConstSmartMemBuf* request_profile)
      throw(Exception);

    void
    request_operation_processor(
      RequestOperationProcessor* request_operation_processor)
      throw();

  protected:
    RequestOperationProcessor_var
    get_request_operation_processor_(
      const AdServer::Commons::UserId& user_id)
      throw();

  private:
    const unsigned long distrib_count_;
    const unsigned long index_;
    const unsigned long services_count_;
    RequestOperationProcessor_var request_operation_processor_;
    RequestOperationProcessor_var other_request_operation_processor_;
  };

  typedef ReferenceCounting::SmartPtr<RequestOperationDistributor>
    RequestOperationDistributor_var;
}
}

#endif /*REQUESTOPERATIONDISTRIBUTOR_HPP*/
