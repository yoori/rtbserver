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

#ifndef COMPOSITE_REQUEST_OPERATION_PROCESSOR_HPP
#define COMPOSITE_REQUEST_OPERATION_PROCESSOR_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include "RequestOperationProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  /**
   * CompositeRequestOperationProcessor
   * delegate request processing to child processors
   */
  class CompositeRequestOperationProcessor:
    public virtual RequestOperationProcessor,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, RequestOperationProcessor::Exception);

    void add_child_processor(RequestOperationProcessor* child_processor)
      throw(Exception);

    virtual void
    process_impression(
      const ImpressionInfo& impression_info)
      throw(Exception);

    virtual void
    process_action(
      const AdServer::Commons::UserId& new_user_id,
      RequestContainerProcessor::ActionType action_type,
      const Generics::Time& time,
      const AdServer::Commons::RequestId& request_id)
      throw(Exception);

    virtual void
    process_impression_post_action(
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const AdServer::RequestInfoSvcs::RequestPostActionInfo& request_post_action_info)
      throw(Exception);
    
    virtual void
    change_request_user_id(
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const Generics::ConstSmartMemBuf* request_profile)
      throw(Exception);

  protected:
    virtual ~CompositeRequestOperationProcessor() throw() {}

  private:
    typedef std::list<RequestOperationProcessor_var> RequestOperationProcessorList;
    RequestOperationProcessorList child_processors_;
  };

  typedef
    ReferenceCounting::SmartPtr<CompositeRequestOperationProcessor>
    CompositeRequestOperationProcessor_var;
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  // CompositeRequestOperationProcessor
  inline
  void
  CompositeRequestOperationProcessor::add_child_processor(
    RequestOperationProcessor* child_processor) throw(Exception)
  {
    RequestOperationProcessor_var add_processor(
      ReferenceCounting::add_ref(child_processor));
    child_processors_.push_back(add_processor);
  }

  void
  CompositeRequestOperationProcessor::process_impression(
    const ImpressionInfo& impression_info)
    throw(Exception)
  {
    for(RequestOperationProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->process_impression(impression_info);
    }
  }

  void
  CompositeRequestOperationProcessor::process_impression_post_action(
    const AdServer::Commons::UserId& new_user_id,
    const AdServer::Commons::RequestId& request_id,
    const AdServer::RequestInfoSvcs::RequestPostActionInfo& request_post_action_info)
    throw(Exception)
  {
    for(RequestOperationProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->process_impression_post_action(
        new_user_id,
        request_id,
        request_post_action_info);
    }
  }

  void
  CompositeRequestOperationProcessor::process_action(
    const AdServer::Commons::UserId& new_user_id,
    RequestContainerProcessor::ActionType action_type,
    const Generics::Time& time,
    const AdServer::Commons::RequestId& request_id)
    throw(Exception)
  {
    for(RequestOperationProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->process_action(
        new_user_id, action_type, time, request_id);
    }
  }

  void
  CompositeRequestOperationProcessor::change_request_user_id(
    const AdServer::Commons::UserId& new_user_id,
    const AdServer::Commons::RequestId& request_id,
    const Generics::ConstSmartMemBuf* request_profile)
    throw(Exception)
  {
    for(RequestOperationProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->change_request_user_id(
        new_user_id, request_id, request_profile);
    }
  }
}
}

#endif /*COMPOSITE_REQUEST_OPERATION_PROCESSOR_HPP*/
