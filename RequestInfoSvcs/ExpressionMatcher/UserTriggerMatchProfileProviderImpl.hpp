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

#ifndef EXPRESSIONMATCHER_USERTRIGGERMATCHPROFILEPROVIDERIMPL_HPP
#define EXPRESSIONMATCHER_USERTRIGGERMATCHPROFILEPROVIDERIMPL_HPP

#include <list>
#include <vector>

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/DefaultImpl.hpp>
#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/MemBuf.hpp>

#include <Commons/UserInfoManip.hpp>
#include <Commons/CorbaObject.hpp>
#include <Commons/HostDistribution.hpp>

#include "UserTriggerMatchContainer.hpp"
#include <RequestInfoSvcs/ExpressionMatcher/ExpressionMatcher.hpp>

namespace AdServer
{
namespace RequestInfoSvcs
{
  class UserTriggerMatchProfileProviderImpl:
    public UserTriggerMatchProfileProvider,
    public ReferenceCounting::AtomicImpl
  {
  public:
    UserTriggerMatchProfileProviderImpl(
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      const CORBACommons::CorbaObjectRefList& expression_matcher_refs,
      Commons::HostDistributionFile* host_distr,
      const char* self_host_name,
      UserTriggerMatchContainer* self_provider,
      unsigned long common_chunks_number)
      throw();

    virtual
    Generics::ConstSmartMemBuf_var
    get_user_profile(const AdServer::Commons::UserId& user_id)
      throw(Exception);

  protected:
    virtual ~UserTriggerMatchProfileProviderImpl() throw()
    {}

  private:
    typedef AdServer::Commons::CorbaObject<
      AdServer::RequestInfoSvcs::ExpressionMatcher>
      ExpressionMatcherRef;

    typedef std::vector<ExpressionMatcherRef> ExpressionMatcherRefArray;

  private:
    std::map<unsigned long, ExpressionMatcherRef> chunks_ref_map_;
    std::set<unsigned long> own_chunks_;
    const UserTriggerMatchContainer_var self_provider_;
    unsigned long common_chunks_number_;
  };
}
}

#endif /*EXPRESSIONMATCHER_USERTRIGGERMATCHPROFILEPROVIDERIMPL_HPP*/
