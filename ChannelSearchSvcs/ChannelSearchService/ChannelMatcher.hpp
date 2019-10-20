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

#ifndef CHANNELSEARCHSERVICE_CHANNELMATCHER_HPP
#define CHANNELSEARCHSERVICE_CHANNELMATCHER_HPP

#include <set>

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>

#include <Logger/Logger.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/Time.hpp>

#include <Commons/UserInfoManip.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannel.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannelIndex.hpp>

namespace AdServer
{
namespace ChannelSearchSvcs
{
  class ChannelMatcher:
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      
    typedef CampaignSvcs::ExpressionChannelHolderMap ChannelMap;
    typedef AdServer::CampaignSvcs::ChannelIdSet ChannelIdSet;

    typedef std::set<unsigned long> CCGIdSet;

    struct Config: public virtual ReferenceCounting::AtomicImpl
    {
    public:
      struct ChannelTraits
      {
        CCGIdSet ccg_ids;
      };

      typedef std::map<unsigned long, ChannelTraits> ChannelTraitsMap;

    public:
      ChannelMap expression_channels;
      ChannelTraitsMap expression_channel_traits_map;

    protected:
      virtual ~Config() throw() {}
    };

    typedef ReferenceCounting::SmartPtr<Config> Config_var;

    struct ChannelMatchResult
    {
      CCGIdSet ccg_ids;
      ChannelIdSet matched_simple_channels;
    };

    typedef std::map<unsigned long, ChannelMatchResult>
      ChannelMatchResultMap;

  public:
    ChannelMatcher() throw();

    virtual ~ChannelMatcher() throw() {};

    Config_var config() const throw(Exception);

    void config(Config* config) throw(Exception);

    void match(
      ChannelMatchResultMap& result,
      const ChannelIdSet& history_channels)
      throw(Exception);

    void search(
      ChannelIdSet& result_channels,
      const ChannelIdSet& history_channels)
      throw(Exception);

  private:
    typedef Sync::Policy::PosixThread SyncPolicy;
    typedef AdServer::CampaignSvcs::ExpressionChannelIndex_var
      ExpressionChannelIndex_var;

    struct ExpressionChannelSearchIndex: public ReferenceCounting::AtomicImpl
    {
      typedef std::map<unsigned long, ChannelIdSet> ChannelSearchMap;
      ChannelSearchMap channel_search_map;

    protected:
      virtual ~ExpressionChannelSearchIndex() throw() {}
    };

    typedef ReferenceCounting::SmartPtr<ExpressionChannelSearchIndex>
      ExpressionChannelSearchIndex_var;

  private:
    ExpressionChannelIndex_var get_channel_index_() const
      throw(Exception);

    ExpressionChannelSearchIndex_var
    get_channel_search_index_() const throw(Exception);

  private:
    Logging::Logger_var logger_;
    mutable SyncPolicy::Mutex lock_;
    Config_var config_;
    ExpressionChannelIndex_var channel_index_;
    ExpressionChannelSearchIndex_var channel_search_index_;
  };

  typedef ReferenceCounting::SmartPtr<ChannelMatcher>
    ChannelMatcher_var;
}
}

#endif /*CHANNELSEARCHSERVICE_CHANNELMATCHER_HPP*/
