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

#ifndef FRONTENDS_PROFILINGSERVER_PROFILINGSERVER_HPP_
#define FRONTENDS_PROFILINGSERVER_PROFILINGSERVER_HPP_

#include <memory>

#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Singleton.hpp>
#include <eh/Exception.hpp>
#include <Logger/Logger.hpp>

#include <Commons/DelegateActiveObject.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>
#include <Commons/zmq.hpp>
//#include <Frontends/Modules/AdFrontend/RequestInfo.hpp>

#include <Frontends/FrontendCommons/ChannelServerSessionPool.hpp>
#include <Frontends/FrontendCommons/UserBindClient.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <xsd/Frontends/ProfilingServerConfig.hpp>
#include <Frontends/ProfilingServer/RequestInfoFiller.hpp>
#include <Frontends/ProfilingServer/HashFilter.hpp>
#include <Frontends/ProfilingServer/ProfilingServerStats.hpp>

namespace AdServer
{
namespace Profiling
{
  class ProfilingServer:
    public AdServer::Commons::ProcessControlVarsLoggerImpl,
    private Generics::CompositeActiveObject
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  public:
    ProfilingServer() throw (eh::Exception);

    void
    main(int& argc, char** argv) throw();

    //
    // IDL:CORBACommons/IProcessControl/shutdown:1.0
    //
    virtual void
    shutdown(CORBA::Boolean wait_for_completion)
      throw();

    //
    // IDL:CORBACommons/IProcessControl/is_alive:1.0
    //
    virtual CORBACommons::IProcessControl::ALIVE_STATUS
    is_alive() throw();

  private:
    virtual
    ~ProfilingServer() throw()
    {}

  private:
    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

    typedef xsd::AdServer::Configuration::ProfilingServerConfigType
      ProfilingServerConfig;

    typedef std::unique_ptr<ProfilingServerConfig> ProfilingServerConfigPtr;
    typedef std::function<void(zmq::message_t&)> Worker;

    class ZmqRouter : public Commons::DelegateActiveObject
    {
    public:
      ZmqRouter(
        Generics::ActiveObjectCallback* callback,
        zmq::context_t& zmq_context,
        const xsd::AdServer::Configuration::ZmqSocketType& socket_config,
        const char* inproc_address)
        throw (eh::Exception);

    protected:
      virtual
      ~ZmqRouter() throw ()
      {}

      virtual void
      work_() throw ();

    private:
      zmq::context_t& zmq_context_;
      zmq::socket_t bind_socket_;
      zmq::socket_t inproc_socket_;
      Generics::ActiveObjectCallback_var callback_;
    };

    class ZmqWorker : public Commons::DelegateActiveObject
    {
    public:
      ZmqWorker(
        Worker worker,
        Generics::ActiveObjectCallback* callback,
        zmq::context_t& zmq_context,
        const char* inproc_address,
        std::size_t work_threads)
        throw (eh::Exception);

    protected:
      virtual
      ~ZmqWorker() throw ()
      {}

      void
      do_work_(const char* inproc_address) throw ();

    private:
      zmq::context_t& zmq_context_;
      Generics::ActiveObjectCallback_var callback_;
      Worker worker_;
    };

  private:
    void
    read_config_(
      const char *filename,
      const char* argv0)
      throw(Exception, eh::Exception);

    void
    init_corba_() throw(Exception);

    void
    init_zeromq_() throw(Exception);

    void
    process_dmp_profiling_info_(zmq::message_t& msg) throw ();

    // user bind utils
    bool
    resolve_user_id_(
      AdServer::Commons::UserId& match_user_id,
      std::string& resolved_ext_user_id,
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      RequestInfo& request_info)
      throw();

    void
    rebind_user_id_(
      const AdServer::Commons::UserId& user_id,
      const AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      const RequestInfo& request_info,
      const String::SubString& resolved_ext_user_id,
      const Generics::Time& time)
      throw();

    // trigger match utils
    void
    trigger_match_(
      AdServer::ChannelSvcs::ChannelServerBase::MatchResult_out trigger_matched_channels,
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      const RequestInfo& request_info,
      const AdServer::Commons::UserId& user_id)
      throw();

    // user info utils
    static AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult*
    get_empty_history_matching_() throw(eh::Exception);

    void
    prepare_ui_match_params_(
      AdServer::UserInfoSvcs::UserInfoMatcher::MatchParams& match_params,
      const AdServer::ChannelSvcs::ChannelServerBase::MatchResult& match_result)
      throw(eh::Exception);

    void
    history_match_(
      AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult_out history_match_result,
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      const RequestInfo& request_info,
      const AdServer::ChannelSvcs::ChannelServerBase::MatchResult* trigger_match_result)
      throw();

    void
    merge_users_(
      const AdServer::Commons::UserId& target_user_id,
      const AdServer::Commons::UserId& source_user_id,
      bool source_temporary,
      const AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params)
      throw();

    // request campaign manager
    void
    request_campaign_manager_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      AdServer::ChannelSvcs::ChannelServerBase::MatchResult*
        trigger_matched_channels,
      AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult*
        history_match_result)
      throw (Exception);

  private:
    CORBACommons::CorbaConfig corba_config_;
    ProfilingServerConfigPtr config_;
    std::unique_ptr<RequestInfoFiller> request_info_filler_;
    CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
    Logging::LoggerCallbackHolder logger_callback_holder_;

    std::unique_ptr<zmq::context_t> zmq_context_;

    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    std::unique_ptr<FrontendCommons::ChannelServerSessionPool> channel_servers_;
    FrontendCommons::UserBindClient_var user_bind_client_;
    FrontendCommons::UserInfoClient_var user_info_client_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    DMPKafkaStreamer_var streamer_;
    HashFilter_var hash_filter_;
    ProfilingServerStats_var stats_;
    unsigned long hash_time_precision_;
  };

  typedef ReferenceCounting::QualPtr<ProfilingServer>
    ProfilingServer_var;

  typedef Generics::Singleton<ProfilingServer, ProfilingServer_var>
    ProfilingServerApp;
}
}

#endif /* FRONTENDS_PROFILINGSERVER_PROFILINGSERVER_HPP_ */
