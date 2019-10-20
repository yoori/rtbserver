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


#ifndef REQUESTINFOSVCS_EXPRESSIONMATCHER_CONSIDERREVENUEINTERFACE_HPP_
#define REQUESTINFOSVCS_EXPRESSIONMATCHER_CONSIDERREVENUEINTERFACE_HPP_

#include "InventoryActionProcessor.hpp"

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class ConsiderRevenueInterface
    {
    public:
      virtual
      ~ConsiderRevenueInterface() throw ()
      {}

      virtual void
      consider_action(
        const AdServer::Commons::UserId& user_id,
        const Generics::Time& time,
        const RevenueDecimal& revenue)
        throw () = 0;

      virtual void
      consider_click(
        const AdServer::Commons::UserId& user_id,
        const AdServer::Commons::RequestId& request_id,
        const Generics::Time& time,
        const RevenueDecimal& revenue)
        throw () = 0;

      virtual void
      consider_impression(
        const AdServer::Commons::UserId& user_id,
        const AdServer::Commons::RequestId& request_id,
        const Generics::Time& time,
        const RevenueDecimal& revenue,
        const ChannelIdSet& channels)
        throw () = 0;

      virtual void
      consider_request(
        const AdServer::Commons::UserId& user_id,
        const Generics::Time& time)
        throw () = 0;
    };
  }
}

#endif /* REQUESTINFOSVCS_EXPRESSIONMATCHER_CONSIDERREVENUEINTERFACE_HPP_ */
