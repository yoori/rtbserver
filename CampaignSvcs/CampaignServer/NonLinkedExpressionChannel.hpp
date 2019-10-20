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

#ifndef _NONLINKEDEXPRESSIONCHANNEL_HPP_
#define _NONLINKEDEXPRESSIONCHANNEL_HPP_

#include <vector>
#include <memory>

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannel.hpp>

namespace AdServer
{
  namespace CampaignSvcs
  {
    /* Hold expression channel (or simple channel) presentation
     * non linked to other channels */
    class NonLinkedExpressionChannel:
      public ReferenceCounting::AtomicImpl
    {
    public:
      friend void print(std::ostream& out,
        const NonLinkedExpressionChannel* channel)
        throw();

      friend bool channel_equal(
        NonLinkedExpressionChannel* left,
        NonLinkedExpressionChannel* right,
        bool status_check)
        throw();

      enum Operation
      {
        NOP = '-',
        AND = '&',
        OR = '|',
        AND_NOT = '^'
      };

      struct Expression
      {
        typedef std::vector<Expression> ExpressionList;
        Operation op;
        unsigned long channel_id;
        ExpressionList sub_channels;

        Expression(): op(NOP), channel_id(0) {}
        Expression(unsigned long channel_id_val)
          : op(NOP), channel_id(channel_id_val)
        {}

        bool operator==(const Expression& right) const throw();

        void swap(Expression& right) throw();

        void all_channels(ChannelIdSet& channels) const throw();
      };

    public:
      NonLinkedExpressionChannel(const ChannelParams& channel_params)
        : channel_params_(channel_params)
      {}

      NonLinkedExpressionChannel(
        const ChannelParams& channel_params,
        const Expression& expression_val)
        throw()
        : channel_params_(channel_params),
          expr_(new Expression(expression_val))
      {};

      NonLinkedExpressionChannel(const Expression& expression_val)
        throw()
        : expr_(new Expression(expression_val))
      {};

      NonLinkedExpressionChannel(const NonLinkedExpressionChannel& right)
        throw()
        : ReferenceCounting::Interface(right),
          ReferenceCounting::AtomicImpl(),
          channel_params_(right.channel_params_),
          expr_(right.expr_.get() ? new Expression(*right.expr_) : 0)
      {}

      bool equal(const NonLinkedExpressionChannel* right,
        bool status_check) const throw();

      const Expression* expression() const throw()
      {
        return expr_.get();
      }

      const ChannelParams& params() const throw()
      {
        return channel_params_;
      }

      ChannelParams& params() throw()
      {
        return channel_params_;
      }

      void params(const ChannelParams& channel_params)
      {
        channel_params_ = channel_params;
      }

    protected:
      virtual
      ~NonLinkedExpressionChannel() throw ()
      {
      }

    private:
      ChannelParams channel_params_;
      std::unique_ptr<Expression> expr_;
    };

    typedef ReferenceCounting::SmartPtr<NonLinkedExpressionChannel>
      NonLinkedExpressionChannel_var;
  }
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    inline
    void print_expression(std::ostream& out,
      const NonLinkedExpressionChannel::Expression& expr)
      throw()
    {
      if(expr.op == NonLinkedExpressionChannel::NOP)
      {
        out << expr.channel_id;
      }
      else
      {
        out << "(";
        for(NonLinkedExpressionChannel::Expression::ExpressionList::const_iterator eit =
              expr.sub_channels.begin();
            eit != expr.sub_channels.end(); ++eit)
        {
          if(eit != expr.sub_channels.begin())
          {
            out << " " << static_cast<char>(expr.op) << " ";
          }

          print_expression(out, *eit);
        }
        out << ")";
      }
    }

    inline
    void print(std::ostream& out,
      const NonLinkedExpressionChannel* channel)
      throw()
    {
      if(!channel->expr_.get())
      {
        out << "[" << channel->channel_params_.channel_id << "]";
      }
      else
      {
        print_expression(out, *(channel->expr_));
      }
    }

    inline
    bool
    channel_equal(NonLinkedExpressionChannel* left,
      NonLinkedExpressionChannel* right,
      bool status_check = true)
      throw()
    {
      return ((left->expr_.get() && right->expr_.get() &&
        *(left->expr_) == *(right->expr_)) ||
        (!left->expr_.get() && !right->expr_.get())) &&
        left->channel_params_.equal(right->channel_params_, status_check);
    }

    inline
    bool NonLinkedExpressionChannel::Expression::operator==(
      const Expression& right) const
      throw()
    {
      return op == right.op &&
        (op != NOP || channel_id == right.channel_id) &&
        sub_channels.size() == right.sub_channels.size() &&
        std::equal(sub_channels.begin(),
          sub_channels.end(), right.sub_channels.begin());
    }

    inline
    void NonLinkedExpressionChannel::Expression::swap(
      Expression& right)
      throw()
    {
      std::swap(op, right.op);
      std::swap(channel_id, right.channel_id);
      sub_channels.swap(right.sub_channels);
    }

    inline
    void
    NonLinkedExpressionChannel::Expression::all_channels(
      ChannelIdSet& channels) const throw()
    {
      if(op == NOP)
      {
        if(channel_id)
        {
          channels.insert(channel_id);
        }
      }
      else
      {
        for(ExpressionList::const_iterator eit = sub_channels.begin();
            eit != sub_channels.end(); ++eit)
        {
          eit->all_channels(channels);
        }
      }
    }
  }
}

#endif /*_NONLINKEDEXPRESSIONCHANNEL_HPP_*/
