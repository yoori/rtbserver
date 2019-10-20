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

#ifndef REQUESTOPERATIONSAVER_HPP
#define REQUESTOPERATIONSAVER_HPP

#include <Commons/UserInfoManip.hpp>
#include <ProfilingCommons/MessageSaver.hpp>
#include "RequestOperationProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  class RequestOperationSaver:
    public virtual RequestOperationProcessor,
    public virtual Generics::ActiveObject,
    public virtual ReferenceCounting::AtomicImpl,
    protected ProfilingCommons::MessageSaver
  {
  public:
    using MessageSaver::FileNameList;

  public:
    RequestOperationSaver(
      Logging::Logger* logger,
      const char* output_dir,
      const char* file_prefix,
      unsigned long chunks_count,
      const Generics::Time& flush_period)
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
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const RequestPostActionInfo& request_post_action_info)
      throw(RequestOperationProcessor::Exception);

    virtual void
    change_request_user_id(
      const AdServer::Commons::UserId& new_user_id,
      const AdServer::Commons::RequestId& request_id,
      const Generics::ConstSmartMemBuf* request_profile)
      throw(RequestOperationProcessor::Exception);

    using MessageSaver::flush;

  protected:
    virtual
    ~RequestOperationSaver()
      throw()
    {}

    void
    write_operation_(
      const AdServer::Commons::UserId& user_id,
      unsigned long op,
      const Generics::MemBuf& mem_buf)
      throw(eh::Exception);

    FileHolderGuard_var
    get_file_holder_(const AdServer::Commons::UserId& user_id)
      throw (eh::Exception);
  };

  typedef ReferenceCounting::SmartPtr<RequestOperationSaver>
    RequestOperationSaver_var;
}
}

#endif /*REQUESTOPERATIONSAVER_HPP*/
