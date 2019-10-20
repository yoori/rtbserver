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

#ifndef USERINFOSVCS_BINDREQUESTPROCESSOR_HPP
#define USERINFOSVCS_BINDREQUESTPROCESSOR_HPP

#include <ReferenceCounting/Interface.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

namespace AdServer
{
namespace UserInfoSvcs
{
  struct BindRequestProcessor:
    public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(ChunkNotFound, Exception);

    struct BindRequest
    {
      std::vector<std::string> bind_user_ids;
    };

  public:
    // return previous state
    virtual void
    add_bind_request(
      const String::SubString& id,
      const BindRequest& bind_request,
      const Generics::Time& now)
      throw(ChunkNotFound, Exception) = 0;

    // create_time : time of user creation, will be used for min age check
    virtual BindRequest
    get_bind_request(
      const String::SubString& external_id,
      const Generics::Time& now)
      throw(ChunkNotFound, Exception) = 0;

    virtual void
    clear_expired(const Generics::Time& expire_time)
      throw(Exception) = 0;

    virtual void
    dump() throw(Exception) = 0;
  };

  typedef ReferenceCounting::SmartPtr<BindRequestProcessor>
    BindRequestProcessor_var;
}
}

namespace AdServer
{
namespace UserInfoSvcs
{
}
}

#endif /*USERINFOSVCS_BINDREQUESTPROCESSOR_HPP*/
