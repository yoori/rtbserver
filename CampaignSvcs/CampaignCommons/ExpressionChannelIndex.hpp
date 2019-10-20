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

#ifndef EXPRESSIONCHANNELINDEX_HPP
#define EXPRESSIONCHANNELINDEX_HPP

#include <list>
#include <set>
#include <map>
#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include "ExpressionChannel.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
  class ExpressionChannelIndex:
    public ReferenceCounting::AtomicImpl
  {
  public:
    typedef std::map<unsigned long, ExpressionChannelBase_var>
      ExpressionChannelBaseMap;

    typedef std::list<ExpressionChannelBase_var>
      ExpressionChannelBaseList;

    struct CheckChannel
    {
      ChannelId channel_id;
      bool cpm_flag;
      ExpressionChannelBase_var channel;

      CheckChannel(ExpressionChannelBase* ch) throw (eh::Exception);
    };

  public:
    ExpressionChannelIndex() throw();

    void index(const ExpressionChannelHolderMap& channels)
      throw();

    unsigned long size() const throw();

    unsigned long avg_seq_size_() const throw();

    void
    match(
      ChannelIdSet& result_channels,
      const ChannelIdHashSet& channels,
      ChannelIdSet* result_cpm_channels = 0) const throw (eh::Exception);

    void
    print(
      std::ostream& out,
      const ChannelIdSet* custom_channels = 0,
      bool expand = false) const
      throw();

  protected:
    typedef std::map<ChannelId, const CheckChannel*> CheckChannelMap;

    struct ExpressionChannelMatch;

    typedef ReferenceCounting::NonNullPtr<ExpressionChannelMatch>::Ptr
      ExpressionChannelMatch_var;

    typedef std::unordered_map<
      unsigned long,
      ExpressionChannelMatch_var>
      ExpressionChannelMatchMap;

    struct ExpressionChannelMatch: public ReferenceCounting::DefaultImpl<>
    {
      std::vector<ChannelId> matched_channels;
      std::vector<ChannelId> cpm_matched_channels;
      std::vector<CheckChannel> check_channels;
      ExpressionChannelMatchMap channels_by_simple_channel_id;

    protected:
      virtual
      ~ExpressionChannelMatch() throw()
      {}
    };

  protected:
    virtual
    ~ExpressionChannelIndex() throw()
    {}

    static ExpressionChannelMatch&
    init_match_cell_(
      ExpressionChannelMatchMap& channels,
      unsigned long channel_id)
      throw();

    static void
    make_fast_expression_(ExpressionChannelMatch& channel_match)
      throw (eh::Exception);

    static void
    match_(
      ChannelIdSet& result_channels,
      ChannelIdSet* result_cpm_channels,
      CheckChannelMap& check_channels,
      const ExpressionChannelMatch& expr_channel_match,
      const ChannelIdHashSet& channels)
      throw();

    static bool
    index_(
      ExpressionChannelBaseList& check_channels,
      ExpressionChannelMatchMap& channels_by_simple_channel_id,
      const ExpressionChannelBaseMap& expr_channels,
      unsigned long depth)
      throw();

    bool
    top_level_index_(
      ExpressionChannelMatchMap& channels_by_simple_channel_id,
      const ExpressionChannelBaseMap& expr_channels)
      throw();

    void
    sub_index_(
      ExpressionChannelMatch& index_node,
      unsigned long depth)
      throw();

    // cpu usage optimization (isn't required, but simplify indexing)
    void index_simple_channels_(
      ExpressionChannelBaseMap& expr_channels,
      const ExpressionChannelHolderMap& channels)
      throw();

    static bool
    reduce_channel_(
      ChannelIdSet& used_simple_channels,
      const ExpressionChannelBase* holder,
      const ChannelIdSet& indexed_simple_channels)
      throw();

    static bool
    reduce_channel_(
      ChannelIdSet& used_simple_channels,
      const ExpressionChannel::Expression& expr,
      const ChannelIdSet& indexed_simple_channels)
      throw();

    static ExpressionChannelBase_var
    create_reduced_channel_(
      ExpressionChannelBase* channel, // const
      const ChannelId indexed_simple_channel)
      throw();

    static bool
    create_reduced_expression_(
      ExpressionChannel::Expression& expr,
      const ChannelId indexed_simple_channel)
      throw();

    static void
    get_required_channels_(
      ChannelIdSet& simple_channels,
      const ExpressionChannel::Expression& expr)
      throw();

  protected:
    ExpressionChannelMatchMap channels_;
  };

  typedef ReferenceCounting::SmartPtr<ExpressionChannelIndex>
    ExpressionChannelIndex_var;
}
}

#endif /*EXPRESSIONCHANNELINDEX_HPP*/
