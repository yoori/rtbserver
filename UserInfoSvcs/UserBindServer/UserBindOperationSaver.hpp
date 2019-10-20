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

#ifndef USERBINDOPERATIONSAVER_HPP
#define USERBINDOPERATIONSAVER_HPP

#include <string>
#include <ProfilingCommons/MessageSaver.hpp>

#include "UserBindProcessor.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  class UserBindOperationSaver:
    public UserBindProcessor,
    public virtual Generics::ActiveObject,
    public virtual ReferenceCounting::AtomicImpl,
    public ProfilingCommons::MessageSaver
  {
  public:
    enum Operation
    {
      OP_ADD_USER_ID = 1,
      OP_GET_USER_ID
    };

  public:
    UserBindOperationSaver(
      Logging::Logger* logger,
      const char* output_files_path,
      const char* output_file_prefix,
      unsigned long chunks_number,
      const Generics::Time& flush_period,
      UserBindProcessor* next_processor);

    virtual UserInfo
    add_user_id(
      const String::SubString& external_id,
      const Commons::UserId& user_id,
      const Generics::Time& now,
      bool resave_if_exists,
      bool ignore_bad_event)
      throw(ChunkNotFound, UserBindProcessor::Exception);

    virtual UserInfo
    get_user_id(
      const String::SubString& external_id,
      const Commons::UserId& current_user_id,
      const Generics::Time& now,
      bool silent,
      const Generics::Time& create_time,
      bool for_set_cookie)
      throw(ChunkNotFound, UserBindProcessor::Exception);

    virtual void
    clear_expired(
      const Generics::Time& unbound_expire_time,
      const Generics::Time& bound_expire_time)
      throw(UserBindProcessor::Exception);

    virtual void
    dump() throw(UserBindProcessor::Exception);

  protected:
    virtual
    ~UserBindOperationSaver() throw () = default;

    UserBindProcessor_var next_processor_;
  };

  typedef ReferenceCounting::SmartPtr<UserBindOperationSaver>
    UserBindOperationSaver_var;
}
}

#endif /*USERBINDOPERATIONSAVER_HPP*/
