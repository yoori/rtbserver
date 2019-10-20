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

#include <LogCommons/LogCommons.hpp>
#include "RequestOperationDistributor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  RequestOperationDistributor::RequestOperationDistributor(
    unsigned long distrib_count,
    unsigned long index,
    unsigned long services_count,
    RequestOperationProcessor* request_operation_processor,
    RequestOperationProcessor* other_request_operation_processor)
    throw()
    : distrib_count_(distrib_count),
      index_(index),
      services_count_(services_count),
      request_operation_processor_(
        ReferenceCounting::add_ref(request_operation_processor)),
      other_request_operation_processor_(
        ReferenceCounting::add_ref(other_request_operation_processor))
  {}

  void
  RequestOperationDistributor::process_impression(
    const ImpressionInfo& impression_info)
    throw(RequestOperationProcessor::Exception)
  {
    get_request_operation_processor_(
      impression_info.user_id)->process_impression(
        impression_info);
  }

  void
  RequestOperationDistributor::process_action(
    const AdServer::Commons::UserId& new_user_id,
    RequestContainerProcessor::ActionType action_type,
    const Generics::Time& time,
    const AdServer::Commons::RequestId& request_id)
    throw(RequestOperationProcessor::Exception)
  {
    get_request_operation_processor_(new_user_id)->process_action(
      new_user_id,
      action_type,
      time,
      request_id);
  }

  void
  RequestOperationDistributor::process_impression_post_action(
    const AdServer::Commons::UserId& new_user_id,
    const AdServer::Commons::RequestId& request_id,
    const RequestPostActionInfo& request_post_action_info)
    throw(RequestOperationProcessor::Exception)
  {
    get_request_operation_processor_(new_user_id)->process_impression_post_action(
      new_user_id,
      request_id,
      request_post_action_info);
  }

  void
  RequestOperationDistributor::change_request_user_id(
    const AdServer::Commons::UserId& new_user_id,
    const AdServer::Commons::RequestId& request_id,
    const Generics::ConstSmartMemBuf* request_profile)
    throw(Exception)
  {
    get_request_operation_processor_(new_user_id)->change_request_user_id(
      new_user_id,
      request_id,
      request_profile);
  }

  RequestOperationProcessor_var
  RequestOperationDistributor::get_request_operation_processor_(
    const AdServer::Commons::UserId& new_user_id)
    throw()
  {
    const unsigned long target_index =
      (AdServer::LogProcessing::user_id_distribution_hash(new_user_id) % distrib_count_) %
        // ~ file index
      services_count_;

    if(target_index == index_)
    {
      return request_operation_processor_;
    }

    return other_request_operation_processor_;
  }

  void
  RequestOperationDistributor::request_operation_processor(
    RequestOperationProcessor* request_operation_processor)
    throw()
  {
    request_operation_processor_ = ReferenceCounting::add_ref(
      request_operation_processor);
  }
}
}
