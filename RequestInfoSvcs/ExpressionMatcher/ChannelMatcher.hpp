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

#ifndef _EXPRESSION_MATCHER_CHANNEL_MATCHER_HPP_
#define _EXPRESSION_MATCHER_CHANNEL_MATCHER_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>

#include <Logger/Logger.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/Time.hpp>

#include <CampaignSvcs/CampaignCommons/ExpressionChannel.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannelIndex.hpp>

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class ChannelMatcher:
      public ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      typedef CampaignSvcs::ExpressionChannelHolderMap ChannelMap;
      typedef AdServer::CampaignSvcs::ChannelIdSet ChannelIdSet;
      typedef std::map<unsigned long, unsigned long> ChannelActionMap;

      struct Config: public ReferenceCounting::AtomicImpl
      {
        Generics::Time fill_time;
        ChannelMap expression_channels;
        ChannelIdSet all_channels;

      private:
        virtual ~Config() throw() {}
      };

      typedef ReferenceCounting::SmartPtr<Config> Config_var;

      class MatchKeyHolder;

      struct MatchKey
      {
      public:
        //MatchKey(CampaignSvcs::ChannelIdSet&& history_channels);

        MatchKey(const CampaignSvcs::ChannelIdSet& history_channels);

        virtual ~MatchKey() throw();

      protected:
        ReferenceCounting::SmartPtr<MatchKeyHolder> match_key_holder_;
      };

      struct MatchResult: public ReferenceCounting::AtomicImpl
      {
        typedef std::vector<unsigned long> ChannelIdArray;

        MatchResult(
          const ChannelIdSet& result_channels_val,
          const ChannelIdSet& result_estimate_channels_val,
          const ChannelActionMap& result_channel_actions_val)
          throw();

        const ChannelIdArray result_channels;
        const ChannelIdArray result_estimate_channels;
        const ChannelActionMap result_channel_actions;

      protected:
        virtual ~MatchResult() throw()
        {}
      };

      typedef ReferenceCounting::SmartPtr<MatchResult>
        MatchResult_var;

    public:
      ChannelMatcher(
        Logging::Logger* logger,
        unsigned long cache_limit,
        const Generics::Time& cache_timeout)
        throw(Exception);

      Config_var config() const throw(Exception);

      void config(Config* config) throw(Exception);

      void process_request(
        const ChannelIdSet& history_channels,
        ChannelIdSet& result_channels,
        ChannelIdSet* result_cpm_channels = 0,
        ChannelActionMap* channel_actions = 0)
        throw(Exception);

    private:
      typedef Sync::Policy::PosixThread SyncPolicy;
      typedef AdServer::CampaignSvcs::ExpressionChannelIndex_var
        ExpressionChannelIndex_var;

      class MatchCache;
      typedef ReferenceCounting::SmartPtr<MatchCache>
        MatchCache_var;

      struct ChannelActionConfig: public ReferenceCounting::AtomicImpl
      {
        typedef std::map<unsigned long, unsigned long> ChannelActionMap;

        ChannelActionMap channel_actions;

      protected:
        virtual ~ChannelActionConfig() throw() = default;
      };

      typedef ReferenceCounting::SmartPtr<ChannelActionConfig>
        ChannelActionConfig_var;

    private:
      virtual
      ~ChannelMatcher() throw();

    void
    process_request_(
      const AdServer::CampaignSvcs::ExpressionChannelIndex* channel_index,
      const ChannelActionConfig* channel_action_config,
      ChannelIdSet& result_channels,
      ChannelIdSet* result_estimate_channels,
      ChannelActionMap* channel_actions,
      const AdServer::CampaignSvcs::ChannelIdHashSet& history_channels);

    ExpressionChannelIndex_var get_channel_index_() const
        throw(Exception);

    private:
      Logging::Logger_var logger_;
      const unsigned long cache_limit_;

      mutable SyncPolicy::Mutex lock_;
      Config_var config_;
      MatchCache_var match_cache_;
      ExpressionChannelIndex_var channel_index_;
      ChannelActionConfig_var channel_action_config_;
    };

    typedef ReferenceCounting::SmartPtr<ChannelMatcher>
      ChannelMatcher_var;

  } // namespace RequestInfoSvcs
} // namespace AdServer

#endif /*_EXPRESSION_MATCHER_CHANNEL_MATCHER_HPP_*/
