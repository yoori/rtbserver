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

#ifndef _USER_INFO_SVCS_USER_INFO_EXCHANGER_PROXY_IMPL_HPP_
#define _USER_INFO_SVCS_USER_INFO_EXCHANGER_PROXY_IMPL_HPP_

#include <list>
#include <map>
#include <set>
#include <string>

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>

#include <xsd/AdServerCommons/AdServerCommons.hpp>
#include <xsd/UserInfoSvcs/UserInfoExchangerProxyConfig.hpp>

#include <CORBACommons/CorbaAdapters.hpp>

#include <UserInfoSvcs/UserInfoExchanger/UserInfoExchanger_s.hpp>


namespace AdServer
{
  namespace UserInfoSvcs
  {
    /**
     * Implementation of UserInfoExchangerProxy.
     */
    class UserInfoExchangerProxyImpl :
      public virtual POA_AdServer::UserInfoSvcs::UserInfoExchanger,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      DECLARE_EXCEPTION(NotReady, Exception);

      typedef xsd::AdServer::Configuration::UserInfoExchangerProxyConfigType
        UserInfoExchangerProxyConfig;

      UserInfoExchangerProxyImpl(
        Generics::ActiveObjectCallback* callback,
        Logging::Logger* logger,
        const UserInfoExchangerProxyConfig& user_info_exchanger_proxy_config)
        throw(Exception);

      virtual ~UserInfoExchangerProxyImpl() throw();

      /* UserInfoExchangerProxy interface */
      virtual void 
      register_users_request(
        const char* customer_id,
        const AdServer::UserInfoSvcs::ColoUsersRequestSeq& users)
        throw(AdServer::UserInfoSvcs::UserInfoExchanger::NotReady,
              AdServer::UserInfoSvcs::UserInfoExchanger::ImplementationException);

      virtual void
      receive_users(
        const char* customer_id, 
        AdServer::UserInfoSvcs::UserProfileSeq_out user_profiles,
        const AdServer::UserInfoSvcs::ReceiveCriteria& receive_criteria)
        throw(AdServer::UserInfoSvcs::UserInfoExchanger::NotReady,
              AdServer::UserInfoSvcs::UserInfoExchanger::ImplementationException);  

      virtual void 
      get_users_requests(
        const char* customer_id,
        AdServer::UserInfoSvcs::UserIdSeq_out users)
        throw(AdServer::UserInfoSvcs::UserInfoExchanger::NotReady,
              AdServer::UserInfoSvcs::UserInfoExchanger::ImplementationException);

      virtual void
      send_users(
        const char* customer_id,
        const AdServer::UserInfoSvcs::UserProfileSeq& user_profiles)
        throw(AdServer::UserInfoSvcs::UserInfoExchanger::NotReady,
              AdServer::UserInfoSvcs::UserInfoExchanger::ImplementationException);

    public:
      struct TraceLevel
      {
        enum
        {
          LOW = Logging::Logger::TRACE,
          MIDDLE,
          HIGH
        };
      };

    protected:
      Generics::ActiveObjectCallback_var callback_;
      Logging::Logger_var logger_;

      CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
      AdServer::UserInfoSvcs::UserInfoExchanger_var uie_ref_;
      
    };

    typedef
      ReferenceCounting::SmartPtr<UserInfoExchangerProxyImpl>
      UserInfoExchangerProxyImpl_var;

  } /* UserInfoSvcs */
} /* AdServer */


#endif /*_USER_INFO_SVCS_USER_INFO_EXCHANGER_PROXY_IMPL_HPP_*/
