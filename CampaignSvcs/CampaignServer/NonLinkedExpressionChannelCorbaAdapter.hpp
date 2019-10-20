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

#ifndef _NON_LINKED_EXPRESSION_CHANNEL_CORBA_ADAPTER_HPP_
#define _NON_LINKED_EXPRESSION_CHANNEL_CORBA_ADAPTER_HPP_

#include <Commons/CorbaAlgs.hpp>

#include <CampaignSvcs/CampaignCommons/CampaignCommons.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>

#include <CampaignSvcs/CampaignCommons/ExpressionChannelCorbaAdapter.hpp>
#include "NonLinkedExpressionChannel.hpp"

namespace AdServer
{
  namespace CampaignSvcs
  {
    void pack_non_linked_expression(
      ExpressionInfo& expression_info,
      const NonLinkedExpressionChannel::Expression& expression)
      throw();

    void unpack_non_linked_expression(
      NonLinkedExpressionChannel::Expression& expression,
      const ExpressionInfo& expression_info);

    void pack_platform_expression_channel(
      ExpressionChannelInfo& channel_info,
      unsigned long channel_id,
      const Generics::Time& timestamp)
      throw();

    void pack_non_linked_expression_channel(
      ExpressionChannelInfo& channel_info,
      const NonLinkedExpressionChannel* expression_channel)
      throw();
  }
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    inline
    void pack_non_linked_expression(
      ExpressionInfo& expression_info,
      const NonLinkedExpressionChannel::Expression& expression)
      throw()
    {
      expression_info.operation = expression.op;
      expression_info.channel_id = expression.channel_id;
      expression_info.sub_channels.length(expression.sub_channels.size());
      CORBA::ULong ei = 0;
      for(NonLinkedExpressionChannel::Expression::ExpressionList::const_iterator eit =
            expression.sub_channels.begin();
          eit != expression.sub_channels.end(); ++eit, ++ei)
      {
        pack_non_linked_expression(expression_info.sub_channels[ei], *eit);
      }
    }

    inline
    void unpack_non_linked_expression(
      NonLinkedExpressionChannel::Expression& expression,
      const ExpressionInfo& expression_info)
    {
      expression.op = static_cast<NonLinkedExpressionChannel::Operation>(
        expression_info.operation);
      expression.channel_id = expression_info.channel_id;
      for(CORBA::ULong ei = 0; ei < expression_info.sub_channels.length(); ++ei)
      {
        NonLinkedExpressionChannel::Expression sub_expr;
        unpack_non_linked_expression(sub_expr,
          expression_info.sub_channels[ei]);
        expression.sub_channels.push_back(sub_expr);
      }
    }

    inline
    void pack_platform_expression_channel(
      ExpressionChannelInfo& channel_info,
      unsigned long channel_id,
      const Generics::Time& timestamp)
      throw()
    {
      pack_platform_channel_params(channel_info, channel_id, timestamp);
      channel_info.expression.operation = 'S';
    }

    inline
    void pack_non_linked_expression_channel(
      ExpressionChannelInfo& channel_info,
      const NonLinkedExpressionChannel* expression_channel)
      throw()
    {
      pack_channel_params(channel_info, expression_channel->params());

      const NonLinkedExpressionChannel::Expression* expr =
        expression_channel->expression();
      if(expr)
      {
        pack_non_linked_expression(channel_info.expression, *expr);
      }
      else
      {
        channel_info.expression.operation = 'S';
      }
    }

    inline
    NonLinkedExpressionChannel_var
    unpack_non_linked_channel(
      const ExpressionChannelInfo& channel_info)
      throw(eh::Exception)
    {
      ChannelParams channel_params;
      unpack_channel_params(channel_params, channel_info);

      if(channel_info.expression.operation != 'S')
      {
        NonLinkedExpressionChannel::Expression expr;
        unpack_non_linked_expression(expr, channel_info.expression);
        return new NonLinkedExpressionChannel(channel_params, expr);
      }
      else
      {
        return new NonLinkedExpressionChannel(channel_params);
      }
    }
  }
}

#endif /*_NON_LINKED_EXPRESSION_CHANNEL_CORBA_ADAPTER_HPP_*/
