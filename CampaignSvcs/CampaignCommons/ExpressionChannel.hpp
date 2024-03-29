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

#ifndef _EXPRESSIONCHANNEL_HPP_
#define _EXPRESSIONCHANNEL_HPP_

#include <string>
#include <set>
#include <list>
#include <vector>

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Time.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Generics/HashTableAdapters.hpp>
#include "CampaignTypes.hpp"

namespace AdServer
{
  namespace CampaignSvcs
  {
    /**
     * ExpressionChannelBase
     * ExpressionChannelHolder
     * ExpressionChannel
     *
     * Filling channel tree from expressions set:
     *   for all expressions:
     *     1. parse expression with using ExpressionParser
     *     2. push result ExpressionChannel into collection
     *       it will be linked to ChannelHolder's from collection
     * Filling of CORBA channel tree representation:
     *   for each channel from collection call CorbaChannelConverter
     * Filling channel tree from CORBA representation:
     *   for each sequence element call CorbaChannelConverter
     */
    typedef unsigned long ChannelId;
    typedef std::unordered_set<ChannelId> ChannelIdHashSet;
    typedef std::set<ChannelId> ChannelIdSet;
    typedef std::map<ChannelId, unsigned long> ChannelWeightMap;
    
    struct ChannelHolder;

    struct ChannelParams
    {
      class DiscoverParams: public ReferenceCounting::AtomicImpl
      {
      public:
        DiscoverParams(
          const char* query_val,
          const char* annotation_val) throw();

        bool operator==(const DiscoverParams& right) const throw();

        std::string query;
        std::string annotation;
        Generics::Time create_time;

      protected:
        virtual
        ~DiscoverParams() throw () = default;
      };

      typedef ReferenceCounting::SmartPtr<DiscoverParams> DiscoverParams_var;
      
      class CMPParams: public ReferenceCounting::AtomicImpl
      {
      public:
        CMPParams(
          unsigned long channel_rate_id_val,
          const RevenueDecimal& imp_revenue_val,
          const RevenueDecimal& click_revenue_val)
          throw();

        bool operator==(const CMPParams& right) const throw();

        unsigned long channel_rate_id;
        RevenueDecimal imp_revenue;
        RevenueDecimal click_revenue;

      protected:
        virtual
        ~CMPParams() throw () = default;
      };

      typedef ReferenceCounting::SmartPtr<CMPParams> CMPParams_var;

      class CommonParams: public ReferenceCounting::AtomicImpl
      {
      public:
        CommonParams() throw ()
          : account_id(0), flags(0), is_public(false), freq_cap_id(0)
        {}

        bool operator==(const CommonParams& right) const throw();

        unsigned long account_id;
        std::string language;
        unsigned long flags;
        bool is_public;
        unsigned long freq_cap_id;

      protected:
        virtual
        ~CommonParams() throw () = default;
      };

      typedef ReferenceCounting::SmartPtr<CommonParams> CommonParams_var;

      class DescriptiveParams: public ReferenceCounting::AtomicImpl
      {
      public:
        DescriptiveParams() throw()
          : parent_channel_id(0)
        {}
        
        bool operator==(const DescriptiveParams& right) const throw()
        {
          return name == right.name &&
            parent_channel_id == right.parent_channel_id;
        }

        std::string name;
        ChannelId parent_channel_id;

      protected:
        virtual
        ~DescriptiveParams() throw () = default;
      };

      typedef ReferenceCounting::SmartPtr<DescriptiveParams> DescriptiveParams_var;

      ChannelParams(ChannelId channel_id_val = 0)
        : channel_id(channel_id_val),
          type('E'),
          status('A'),
          action_id(0)
      {}

      bool equal(const ChannelParams& right, bool status_check = true)
        const throw();

      ChannelId channel_id;
      char type;
      std::string country;
      char status;
      unsigned long action_id;

      CommonParams_var common_params;
      DescriptiveParams_var descriptive_params;
      DiscoverParams_var discover_params;
      CMPParams_var cmp_params;

      Generics::Time timestamp;
    };

    class ExpressionChannel;
    class SimpleChannel;
    
    typedef ReferenceCounting::SmartPtr<const SimpleChannel>
      ConstSimpleChannel_var;
    typedef ReferenceCounting::SmartPtr<const ExpressionChannel>
      ConstExpressionChannel_var;
    typedef ReferenceCounting::SmartPtr<SimpleChannel>
      SimpleChannel_var;
    typedef ReferenceCounting::SmartPtr<ExpressionChannel>
      ExpressionChannel_var;
    
    struct ChannelUseCount
    {
      ChannelUseCount(): count(0) {}
      unsigned long count;
      ChannelIdSet channel_ids;
      std::set<unsigned long> ccg_ids;
    };

    typedef std::list<ChannelId> ChannelIdList;
    typedef Generics::NumericHashAdapter<ChannelId> ChannelIdHashAdapter;
    typedef Generics::GnuHashTable<ChannelIdHashAdapter, ChannelUseCount>
      ChannelUseCountMap;

    class ExpressionChannelBase;
    typedef ReferenceCounting::SmartPtr<ExpressionChannelBase>
      ExpressionChannelBase_var;
    typedef std::list<ExpressionChannelBase_var> ExpressionChannelList;

    /* base Channel interface */
    class ExpressionChannelBase: public virtual ReferenceCounting::Interface
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      virtual bool has_params() const throw() = 0;
      
      virtual const ChannelParams& params() const throw() = 0;

      virtual ChannelParams& params() throw() = 0;

      virtual unsigned long triggered(
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap* weighted_triggered_channels,
        const char* status_set = "A",
        ChannelUseCountMap* uc_tbl = 0,
        ChannelIdSet* matched_bp = 0) const
        throw(Exception) = 0;

      virtual bool use(
        ChannelUseCountMap& uc_tbl,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set = "A",
        ChannelIdSet* matched_bp = 0) const
        throw(Exception) = 0;

      virtual void
      triggered_named_channels(
        ChannelIdSet& responded_channels,
        const ChannelIdHashSet& triggered_channels) const
        throw(Exception) = 0;

      virtual bool
      triggered_expression(
        std::ostream& responded_expr,
        const ChannelIdHashSet& triggered_channels)
        const
        throw(Exception, eh::Exception) = 0;

      virtual void
      get_cmp_channels(
        ExpressionChannelList& cmp_channels,
        const ChannelIdHashSet& simple_channels)
        throw(Exception) = 0;
      
      virtual void get_all_cmp_channels(
        ExpressionChannelList& cmp_channels)
        throw(Exception) = 0;

      virtual void get_all_channels(
        ChannelIdSet& channels)
        throw() = 0;

      virtual ConstExpressionChannel_var
      expression_channel() const throw()
      {
        return ConstExpressionChannel_var();
      }
      
      virtual ConstSimpleChannel_var
      simple_channel() const throw()
      {
        return ConstSimpleChannel_var();
      }

      virtual ReferenceCounting::SmartPtr<ExpressionChannelBase>
      optimize()
      {
        return ReferenceCounting::add_ref(this);
      };

      virtual std::ostream&
      print(std::ostream& os) const throw (eh::Exception)
      {
        return os;
      }

    protected:
      virtual
      ~ExpressionChannelBase() throw () = default;
    };

    /* Channel implementation that delegate all calls to
     * child channel */
    class ExpressionChannelHolder:
      public ExpressionChannelBase,
      public ReferenceCounting::AtomicImpl
    {
    public:
      ExpressionChannelHolder() throw();

      ExpressionChannelHolder(ExpressionChannelBase* channel) throw();

      virtual bool has_params() const throw()
      {
        return channel.in() ? channel->has_params() : false;
      }
      
      virtual const ChannelParams& params() const throw()
      {
        assert(channel.in());
        return channel->params();
      };

      virtual ChannelParams& params() throw()
      {
        assert(channel.in());
        return channel->params();
      };

      virtual unsigned long triggered(
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap* weighted_triggered_channels,
        const char* status_set = "A",
        ChannelUseCountMap* uc_tbl = 0,
        ChannelIdSet* matched_bp = 0) const
        throw(Exception);

      virtual bool
      use(
        ChannelUseCountMap& uc_tbl,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set = "A",
        ChannelIdSet* matched_bp = 0) const
        throw(Exception);

      virtual void
      triggered_named_channels(
        ChannelIdSet& responded_channels,
        const ChannelIdHashSet& triggered_channels) const
        throw(Exception);

      virtual bool
      triggered_expression(
        std::ostream& responded_expr,
        const ChannelIdHashSet& triggered_channels)
        const
        throw(Exception, eh::Exception);

      virtual void
      get_cmp_channels(
        ExpressionChannelList& cmp_channels,
        const ChannelIdHashSet& simple_channels)
        throw(Exception);

      virtual void get_all_cmp_channels(
        ExpressionChannelList& cmp_channels)
        throw(Exception);

      virtual void get_all_channels(
        ChannelIdSet& channels)
        throw();

      virtual ConstExpressionChannel_var
      expression_channel() const throw();
      
      virtual ConstSimpleChannel_var
      simple_channel() const throw();

      virtual ReferenceCounting::SmartPtr<ExpressionChannelBase>
      optimize()
      {
        if (channel.in())
        {
          channel = channel->optimize();
        }

        return channel;
      };

      ExpressionChannelBase_var channel;

    protected:
      virtual
      ~ExpressionChannelHolder() throw () = default;
    };

    typedef ReferenceCounting::SmartPtr<ExpressionChannelHolder>
      ExpressionChannelHolder_var;

    class ExpressionChannelBaseWithParams:
      public ExpressionChannelBase
    {
    public:
      ExpressionChannelBaseWithParams() {}
      
      ExpressionChannelBaseWithParams(const ChannelParams& channel_params)
        : channel_params_(channel_params)
      {}

      virtual bool has_params() const throw()
      {
        return true;
      }
      
      virtual const ChannelParams& params() const throw();

      virtual ChannelParams& params() throw();

      void params(const ChannelParams& channel_params)
      {
        channel_params_ = channel_params;
      }
      
    protected:
      virtual
      ~ExpressionChannelBaseWithParams() throw () = default;

      ChannelParams channel_params_;      
    };

    typedef ReferenceCounting::SmartPtr<ExpressionChannelBaseWithParams>
      ExpressionChannelBaseWithParams_var;
    
    class SimpleChannel:
      public ExpressionChannelBaseWithParams,
      public ReferenceCounting::AtomicImpl
    {
    public:
      SimpleChannel() {}

      SimpleChannel(const ChannelParams& channel_params)
        : ExpressionChannelBaseWithParams(channel_params)
      {}

      virtual unsigned long triggered(
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap* weighted_triggered_channels,
        const char* status_set = "A",
        ChannelUseCountMap* uc_tbl = 0,
        ChannelIdSet* matched_bp = 0) const
        throw(Exception);

      virtual bool use(
        ChannelUseCountMap& uc_tbl,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set = "A",
        ChannelIdSet* matched_bp = 0) const
        throw(Exception);

      virtual void
      triggered_named_channels(
        ChannelIdSet& responded_channels,
        const ChannelIdHashSet& triggered_channels) const
        throw(Exception);

      virtual bool
      triggered_expression(
        std::ostream& responded_expr,
        const ChannelIdHashSet& triggered_channels)
        const
        throw(Exception, eh::Exception);

      virtual void
      get_cmp_channels(
        ExpressionChannelList& cmp_channels,
        const ChannelIdHashSet& simple_channels)
        throw(Exception);

      virtual void get_all_cmp_channels(
        ExpressionChannelList& cmp_channels)
        throw(Exception);

      virtual void get_all_channels(
        ChannelIdSet& channels)
        throw();

      virtual ConstSimpleChannel_var
      simple_channel() const throw()
      {
        return ReferenceCounting::add_ref(this);
      }

      static bool
      use(
        ChannelId channel_id,
        ChannelUseCountMap& uc_tbl,
        const ChannelIdHashSet& triggered_channels,
        ChannelIdSet* matched_channels)
        throw(Exception);

    protected:
      virtual
      ~SimpleChannel() throw () = default;
    };

    void print(std::ostream& out, const ExpressionChannelBase* channel, bool expand = false)
      throw();

    class ExpressionChannel:
      public ExpressionChannelBaseWithParams,
      public ReferenceCounting::AtomicCopyImpl
    {
    public:
      friend void print(std::ostream& out, const ExpressionChannelBase* channel, bool expand)
        throw();
      
      enum Operation
      {
        NOP = '-',
        AND = '&',
        OR = '|',
        AND_NOT = '^',
        TRUE = 'T'
      };
      
      struct Expression
      {
        typedef std::vector<Expression> ExpressionList;

        Operation op;
        ExpressionChannelBase_var channel;
        ExpressionList sub_channels;

        Expression(): op(NOP) {}

        Expression(ExpressionChannelBase* channel)
          : op(NOP), channel(ReferenceCounting::add_ref(channel))
        {}

#if __GNUC__ == 4 && __GNUC_MINOR__ == 4
        Expression(const Expression&) = default;

        Expression(Expression&& expr)
          : op(expr.op), channel(std::move(expr.channel)),
            sub_channels(std::move(expr.sub_channels))
        {}
#endif

        bool
        operator==(const Expression& right) const throw();

        static const Expression EMPTY;
      };

    public:
      ExpressionChannel()
      {}

      ExpressionChannel(const ChannelParams& channel_params)
        : ExpressionChannelBaseWithParams(channel_params)
      {}

      ExpressionChannel(const Expression& expression_val)
        throw()
        : expr_(expression_val)
      {};

      ExpressionChannel(Expression&& expression_val)
        throw()
        : expr_(std::move(expression_val))
      {};

      ExpressionChannel(
        const ChannelParams& channel_params,
        const Expression& expression_val)
        throw()
        : ExpressionChannelBaseWithParams(channel_params),
          expr_(expression_val)
      {};

      ExpressionChannel(
        const ChannelParams& channel_params,
        Expression&& expression_val)
        throw()
        : ExpressionChannelBaseWithParams(channel_params),
          expr_(std::move(expression_val))
      {};

      virtual unsigned long triggered(
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap* weighted_triggered_channels,
        const char* status_set = "A",
        ChannelUseCountMap* uc_tbl = 0,
        ChannelIdSet* matched_bp = 0) const
        throw(Exception);

      virtual bool
      use(
        ChannelUseCountMap& uc_tbl,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set,
        ChannelIdSet* matched_bp = 0) const
        throw(Exception);

      virtual void
      triggered_named_channels(
        ChannelIdSet& responded_channels,
        const ChannelIdHashSet& triggered_channels) const
        throw(Exception);

      virtual bool
      triggered_expression(
        std::ostream& responded_expr,
        const ChannelIdHashSet& triggered_channels)
        const
        throw(Exception, eh::Exception);

      virtual void
      get_cmp_channels(
        ExpressionChannelList& cmp_channels,
        const ChannelIdHashSet& simple_channels)
        throw(Exception);

      virtual void get_all_cmp_channels(
        ExpressionChannelList& cmp_channels)
        throw(Exception);

      virtual void get_all_channels(
        ChannelIdSet& channels)
        throw();

      virtual ConstExpressionChannel_var expression_channel() const throw()
      {
        return ReferenceCounting::add_ref(this);
      }

      const Expression&
      expression() const throw()
      {
        return expr_;
      }

      virtual ReferenceCounting::SmartPtr<ExpressionChannelBase>
      optimize();

      bool
      equal(const ExpressionChannel* right) const throw();

    protected:
      virtual
      ~ExpressionChannel() throw () = default;

    private:
      static void
      store_use_count_(
        ChannelUseCountMap* uc_tbl,
        const ExpressionChannelBase* channel,
        const ChannelIdSet& local_matched_channels)
        throw(Exception);

      static unsigned long
      triggered_expr_(
        const Expression& expr,
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap* weighted_triggered_channels,
        const char* status_set,
        ChannelUseCountMap* uc_tbl,
        ChannelIdSet* matched_channels)
        throw(Exception);

      static unsigned long
      triggered_(
        const Expression& expr,
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap* weighted_triggered_channels,
        const char* status_set,
        ChannelUseCountMap* uc_tbl,
        ChannelIdSet* matched_bp)
        throw(Exception);

      static void
      triggered_named_channels_(
        ChannelIdSet& triggered_named_channels,
        const Expression& expr,
        const ChannelIdHashSet& triggered_channels)
        throw(Exception);

      static void
      get_cmp_channels_(
        ExpressionChannelList& cmp_channels,
        const ChannelIdHashSet& simple_channels,
        const Expression& expr)
        throw(Exception);

      static void
      get_all_cmp_channels_(
        ExpressionChannelList& cmp_channels,
        const Expression& expr)
        throw(Exception);

      static void get_all_channels_(
        ChannelIdSet& channels,
        const Expression& expr)
        throw();

      bool triggered_expression_(
        std::ostream& responded_expr,
        const Expression& expr,
        const ChannelIdHashSet& triggered_channels)
        const
        throw(Exception, eh::Exception);

      static bool use_(
        ChannelUseCountMap& uc_tbl,
        const Expression& expr,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set,
        ChannelIdSet* matched_channels)
        throw(Exception);

      static bool
      optimize_expr_(Expression& expr);

    private:
      Expression expr_;
    };

    typedef std::unordered_map<unsigned long, ExpressionChannelHolder_var>
      ExpressionChannelHolderMap;

    /**
     * Implementation optimized by CPU usage
     */
    class FastExpressionChannel:
      public ExpressionChannelBase,
      public ReferenceCounting::AtomicImpl
    {
    public:
      FastExpressionChannel(const ExpressionChannelBase* channel)
        throw (eh::Exception);

      virtual bool
      has_params() const throw();

      virtual const ChannelParams&
      params() const throw();

      virtual ChannelParams&
      params() throw();

      virtual unsigned long
      triggered(
        const ChannelIdHashSet* triggered_channels,
        const ChannelWeightMap*,
        const char* = "A",
        ChannelUseCountMap* = 0,
        ChannelIdSet* = 0) const
        throw(Exception);

      virtual bool
      use(
        ChannelUseCountMap& uc_tbl,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set = "A",
        ChannelIdSet* matched_channels = 0) const
        throw(Exception);

      virtual void
      triggered_named_channels(
        ChannelIdSet&,
        const ChannelIdHashSet&) const
        throw(Exception)
      {}

      virtual bool
      triggered_expression(
        std::ostream&,
        const ChannelIdHashSet&) const
        throw(Exception, eh::Exception)
      {
        return false;
      }

      virtual void
      get_cmp_channels(
        ExpressionChannelList&,
        const ChannelIdHashSet&)
        throw(Exception)
      {}

      virtual void
      get_all_cmp_channels(
        ExpressionChannelList&)
        throw(Exception)
      {}

      virtual void
      get_all_channels(
        ChannelIdSet& channels)
        throw();

      virtual std::ostream&
      print(std::ostream& os) const throw (eh::Exception);

    protected:
      struct Expression
      {
        ExpressionChannel::Operation op;
        ChannelId channel_id;
        std::vector<Expression> sub_channels;
        std::vector<ChannelId> simple;

        Expression() throw ()
          : op(ExpressionChannel::NOP), channel_id(0)
        {}

        Expression(Expression&& arg) throw ()
          : op(arg.op), channel_id(arg.channel_id),
            sub_channels(std::move(arg.sub_channels)),
            simple(std::move(arg.simple))
        {}

        Expression(const Expression& arg) throw ()
          : op(arg.op), channel_id(arg.channel_id),
            sub_channels(arg.sub_channels),
            simple(arg.simple)
        {}

        Expression& operator= (const Expression& arg) throw ()
        {
          if (this != &arg)
          {
            op = arg.op;
            channel_id = arg.channel_id;
            sub_channels = arg.sub_channels;
            simple = arg.simple;
          }

          return *this;
        }
      };

    protected:
      virtual
      ~FastExpressionChannel() throw () = default;

      static bool
      match_cell_(
        const Expression& expr,
        const ChannelIdHashSet& channels);

      static void
      get_all_channels_(
        ChannelIdSet& channels,
        const Expression& expr)
        throw (eh::Exception);

      static bool
      use_(
        ChannelUseCountMap& uc_tbl,
        const Expression& expr,
        const ChannelIdHashSet& triggered_channels,
        const char* status_set,
        ChannelIdSet* matched_channels)
        throw(Exception);

      static Expression
      make_cell_(const ExpressionChannel::Expression& expr)
        throw (eh::Exception);

      static std::ostream&
      print_(
        std::ostream& os,
        const Expression& expr)
        throw (eh::Exception);

      static
      void
      reordering_(
        Expression& expr,
        ChannelIdSet& basis)
        throw (eh::Exception);

    protected:
      Expression expr_;
    };
  }
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    // ChannelParams
    inline
    bool
    ChannelParams::CommonParams::operator==(const CommonParams& right) const throw()
    {
      return account_id == right.account_id &&
        language == right.language &&
        flags == right.flags &&
        is_public == right.is_public &&
        freq_cap_id == right.freq_cap_id;
    }

    inline
    ChannelParams::DiscoverParams::DiscoverParams(
      const char* query_val,
      const char* annotation_val) throw()
      : query(query_val),
        annotation(annotation_val)
    {}

    inline
    bool
    ChannelParams::DiscoverParams::operator==(
      const DiscoverParams& right) const throw()
    {
      return query == right.query &&
        annotation == right.annotation &&
        create_time == right.create_time;
    }

    inline
    ChannelParams::CMPParams::CMPParams(
      unsigned long channel_rate_id_val,
      const RevenueDecimal& imp_revenue_val,
      const RevenueDecimal& click_revenue_val)
      throw()
      : channel_rate_id(channel_rate_id_val),
        imp_revenue(imp_revenue_val),
        click_revenue(click_revenue_val)
    {}

    inline
    bool
    ChannelParams::CMPParams::operator==(const CMPParams& right) const throw()
    {
      return channel_rate_id == right.channel_rate_id &&
        imp_revenue == right.imp_revenue &&
        click_revenue == right.click_revenue;
    }

    inline
    bool ChannelParams::equal(
      const ChannelParams& right,
      bool status_check) const throw()
    {
      return channel_id == right.channel_id &&
        type == right.type &&
        country == right.country &&
        (!status_check || status == right.status) &&
        ((common_params.in() && right.common_params.in() &&
          *common_params == *right.common_params) ||
          (!common_params.in() && !right.common_params.in())) &&
        ((descriptive_params.in() && right.descriptive_params.in() &&
          *descriptive_params == *right.descriptive_params) ||
          (!descriptive_params.in() && !right.descriptive_params.in())) &&
        ((discover_params.in() && right.discover_params.in() &&
          *discover_params == *right.discover_params) ||
          (!discover_params.in() && !right.discover_params.in())) &&
        ((cmp_params.in() && right.cmp_params.in() &&
          *cmp_params == *right.cmp_params) ||
          (!cmp_params.in() && !right.cmp_params.in()));
    }
    
    inline
    void print_expression(std::ostream& out,
      const ExpressionChannel::Expression& expr,
      bool expand)
      throw()
    {
      if(expr.op == ExpressionChannel::NOP)
      {
        if(expr.channel)
        {
          print(out, expr.channel);
        }
        else
        {
          out << "NULL";
        }
      }
      else if(expr.op == ExpressionChannel::TRUE)
      {
        out << "TRUE";
      }
      else
      {
        out << "(";
        for(ExpressionChannel::Expression::ExpressionList::const_iterator eit =
              expr.sub_channels.begin();
            eit != expr.sub_channels.end(); ++eit)
        {
          if(eit != expr.sub_channels.begin())
          {
            out << " " << static_cast<char>(expr.op) << " ";
          }

          print_expression(out, *eit, expand);
        }
        out << ")";
      }
    }
    
    inline
    void print(std::ostream& out, const ExpressionChannelBase* channel, bool expand)
      throw()
    {
      ConstSimpleChannel_var simple_channel = channel->simple_channel();
      if(simple_channel.in())
      {
        out << "[" << simple_channel->params().channel_id << "]";
      }
      else
      {
        ConstExpressionChannel_var expression_channel =
          channel->expression_channel();
        if(expression_channel.in())
        {
          print_expression(out, expression_channel->expr_, expand);
        }
        else
        {
          out << "NULL"; // non intialized holder
        }
      }
    }

    inline
    bool
    channel_equal(ExpressionChannelBase* left, ExpressionChannelBase* right)
      throw()
    {
      ConstSimpleChannel_var left_simple_channel = left->simple_channel();
      ConstSimpleChannel_var right_simple_channel = right->simple_channel();
      if(left_simple_channel.in() && right_simple_channel.in())
      {
        return left_simple_channel->params().equal(
          right_simple_channel->params());
      }
      else if(left_simple_channel.in() || right_simple_channel.in())
      {
        return false;
      }

      ConstExpressionChannel_var left_expression_channel = left->expression_channel();
      ConstExpressionChannel_var right_expression_channel = right->expression_channel();
      if(left_expression_channel.in() && right_expression_channel.in())
      {
        return left_expression_channel->equal(right_expression_channel);
      }

      return false;
    }
    
    inline
    ExpressionChannelHolder::ExpressionChannelHolder() throw()
    {}

    inline
    ExpressionChannelHolder::ExpressionChannelHolder(
      ExpressionChannelBase* channel_val) throw()
      : channel(ReferenceCounting::add_ref(channel_val))
    {}
    
    inline
    ConstExpressionChannel_var
    ExpressionChannelHolder::expression_channel() const throw()
    {
      return channel.in() ? channel->expression_channel() :
        ConstExpressionChannel_var();
    }

    inline
    ConstSimpleChannel_var
    ExpressionChannelHolder::simple_channel() const throw()
    {
      return channel.in() ? channel->simple_channel() :
        ConstSimpleChannel_var();
    }

    inline
    const ChannelParams& ExpressionChannelBaseWithParams::params() const throw()
    {
      return channel_params_;
    }
    
    inline
    ChannelParams& ExpressionChannelBaseWithParams::params() throw()
    {
      return channel_params_;
    }

    inline
    bool ExpressionChannel::Expression::operator==(const Expression& right) const
      throw()
    {
      return op == right.op &&
        (op == NOP ?
         (channel.in() && right.channel.in() &&
           channel->params().channel_id == right.channel->params().channel_id) ||
         (!channel.in() && !right.channel.in()) :
        std::equal(sub_channels.begin(),
          sub_channels.end(), right.sub_channels.begin()));
    }
  }
}

#endif /*_EXPRESSIONCHANNEL_HPP_*/
