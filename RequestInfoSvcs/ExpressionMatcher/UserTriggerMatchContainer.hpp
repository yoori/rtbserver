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

#ifndef EXPRESSIONMATCHER_USERTRIGGERMATCHCONTAINER_HPP
#define EXPRESSIONMATCHER_USERTRIGGERMATCHCONTAINER_HPP

#include <list>
#include <vector>
#include <map>

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/PtrHolder.hpp>
#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/MemBuf.hpp>

#include <Commons/UserInfoManip.hpp>
#include <ProfilingCommons/ProfileMap/ProfileMapFactory.hpp>
#include <ProfilingCommons/PlainStorageAdapters.hpp>

#include "TriggerActionProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  class UserTriggerMatchProfileProvider:
    public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    virtual
    Generics::ConstSmartMemBuf_var
    get_user_profile(const AdServer::Commons::UserId& user_id)
      throw(Exception) = 0;

  protected:
    virtual
    ~UserTriggerMatchProfileProvider() throw () = default;
  };

  typedef ReferenceCounting::SmartPtr<UserTriggerMatchProfileProvider>
    UserTriggerMatchProfileProvider_var;

  class UserTriggerMatchContainer:
    public Generics::CompositeActiveObject,
    public ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(NotReady, Exception);

    struct Config: public ReferenceCounting::AtomicImpl
    {
    public:
      // channel_id => { page/search/url min visits, *channel_trigger_id }
      class ChannelInfo: public ReferenceCounting::AtomicImpl
      {
      public:
        typedef std::vector<unsigned long> TriggerIdArray;

        ChannelInfo()
          : page_time_to(0),
            search_time_to(0),
            url_time_to(0),
            url_keyword_time_to(0),
            page_min_visits(0),
            search_min_visits(0),
            url_min_visits(0),
            url_keyword_min_visits(0)
        {}

        unsigned long page_time_to;
        unsigned long search_time_to;
        unsigned long url_time_to;
        unsigned long url_keyword_time_to;

        unsigned long page_min_visits;
        unsigned long search_min_visits;
        unsigned long url_min_visits;
        unsigned long url_keyword_min_visits;

        TriggerIdArray page_triggers;
        TriggerIdArray search_triggers;
        TriggerIdArray url_triggers;
        TriggerIdArray url_keyword_triggers;

      protected:
        virtual
        ~ChannelInfo() throw () = default;
      };

      typedef ReferenceCounting::SmartPtr<ChannelInfo> ChannelInfo_var;
      typedef std::map<unsigned long, ChannelInfo_var> ChannelInfoMap;

    public:
      ChannelInfoMap channels;

    protected:
      virtual
      ~Config() throw () = default;
    };

    typedef ReferenceCounting::SmartPtr<Config> Config_var;

    typedef std::vector<unsigned long> MatchedTriggerIdArray;

    typedef std::map<unsigned long, MatchedTriggerIdArray> MatchMap;

    struct RequestInfo
    {
      AdServer::Commons::UserId user_id;
      AdServer::Commons::UserId merge_user_id;
      Generics::Time time;

      MatchMap page_matches;
      MatchMap search_matches;
      MatchMap url_matches;
      MatchMap url_keyword_matches;
    };

    struct ImpressionInfo
    {
      AdServer::Commons::UserId user_id;
      AdServer::Commons::RequestId request_id;
      Generics::Time time;

      ChannelIdSet channels;
    };

    static const Generics::Time DEFAULT_USER_EXPIRE_TIME;
    static const Generics::Time DEFAULT_REQUEST_EXPIRE_TIME;

  public:
    UserTriggerMatchContainer(
      Logging::Logger* logger,
      TriggerActionProcessor* processor,
      UserTriggerMatchProfileProvider* user_profile_provider,
      unsigned long common_chunks_number,
      const AdServer::ProfilingCommons::ProfileMapFactory::ChunkPathMap& chunk_folders,
      const char* user_file_prefix,
      const char* request_file_base_path,
      const char* request_file_prefix, // no request profiles mode, if == 0
      unsigned long positive_triggers_group_size,
      unsigned long negative_triggers_group_size,
      unsigned long max_trigger_visits,
      ProfilingCommons::ProfileMapFactory::Cache* cache,
      const AdServer::ProfilingCommons::LevelMapTraits& user_level_map_traits,
      const AdServer::ProfilingCommons::LevelMapTraits& request_level_map_traits)
      throw(Exception);

    void config(Config* config) throw();

    Config_var config() const throw();

    void process_request(const RequestInfo& request_info)
      throw(NotReady, Exception);

    void process_impression(
      const ImpressionInfo& imp_info)
      throw(NotReady, Exception);

    void process_click(
      const Commons::RequestId& imp_info,
      const Generics::Time& time)
      throw(Exception);

    Generics::ConstSmartMemBuf_var
    get_user_profile(const AdServer::Commons::UserId& user_id)
      throw(Exception);

    Generics::ConstSmartMemBuf_var
    get_request_profile(const AdServer::Commons::RequestId& request_id)
      throw(Exception);

    void clear_expired() throw(Exception);

  protected:
    virtual
    ~UserTriggerMatchContainer() throw ();

  private:
    typedef AdServer::ProfilingCommons::ChunkedProfileMap<
      AdServer::Commons::UserId,
      AdServer::ProfilingCommons::TransactionProfileMap<AdServer::Commons::UserId>,
      unsigned long (*)(const Generics::Uuid& uuid) >
    UserProfileMap;

    typedef ReferenceCounting::SmartPtr<UserProfileMap>
      UserProfileMap_var;

    typedef ProfilingCommons::TransactionProfileMap<
      AdServer::Commons::RequestId>
      RequestProfileMap;

    typedef ReferenceCounting::SmartPtr<RequestProfileMap>
      RequestProfileMap_var;

    typedef Sync::Policy::PosixThread SyncPolicy;

    typedef std::list<TriggerActionProcessor::TriggersMatchInfo>
      TriggersMatchInfoList;

  private:
    Config_var current_config_() const throw(NotReady);

    void process_request_trans_(
      TriggersMatchInfoList& delegate_imps,
      TriggersMatchInfoList& delegate_clicks,
      const RequestInfo& request_info)
      throw(Exception);

    void process_impression_trans_(
      bool& delegate_impression,
      bool& delegate_click,
      TriggerActionProcessor::TriggersMatchInfo& imp_matches_info,
      const ImpressionInfo& imp_info)
      throw(Exception);

    void process_click_trans_(
      bool& delegate_click,
      TriggerActionProcessor::TriggersMatchInfo& click_matches_info,
      const Commons::RequestId& request_id,
      const Generics::Time& time)
      throw(Exception);

    template<typename TransactionType, typename ProfileWriterType>
    void save_profile_(
      TransactionType* transaction,
      const ProfileWriterType& profile_writer,
      const Generics::Time& time)
      throw(eh::Exception);

    void merge_user_(
      const Commons::UserId& target_user_id,
      const Commons::UserId& source_user_id)
      throw(eh::Exception);

  private:
    Logging::Logger_var logger_;
    TriggerActionProcessor_var processor_;
    UserTriggerMatchProfileProvider_var merge_profile_provider_;

    const unsigned long positive_triggers_group_size_;
    const unsigned long negative_triggers_group_size_;
    const unsigned long max_trigger_visits_;
    const Generics::Time user_expire_time_;
    const Generics::Time request_expire_time_;
    Config::ChannelInfo_var default_channel_info_;

    UserProfileMap_var user_map_;
    RequestProfileMap_var request_map_;

    ReferenceCounting::PtrHolder<Config_var> config_;
  };

  typedef ReferenceCounting::SmartPtr<UserTriggerMatchContainer>
    UserTriggerMatchContainer_var;
}
}

#endif /*EXPRESSIONMATCHER_USERTRIGGERMATCHCONTAINER_HPP*/
