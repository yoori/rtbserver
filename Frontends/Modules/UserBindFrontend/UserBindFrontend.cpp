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

#include <Logger/StreamLogger.hpp>
#include <HTTP/HTTPCookie.hpp>

#include <Generics/Uuid.hpp>

#include <CORBACommons/CorbaAdapters.hpp>

#include <Commons/UserInfoManip.hpp>
#include <Commons/Algs.hpp>

#include <Commons/ErrorHandler.hpp>
#include <Commons/CorbaConfig.hpp>
#include <Commons/CorbaAlgs.hpp>
#include <Commons/ExternalUserIdUtils.hpp>
#include <Commons/Base32.hpp>

#include <Frontends/CommonModule/CommonModule.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>

#include "UserBindFrontend.hpp"

namespace Config
{
  const char ENABLE[] = "UserBindFrontend_Enable";
  const char CONFIG_FILES[] = "UserBindFrontend_Config";
  const char CONFIG_FILE[] = "UserBindFrontend_ConfigFile";
}

namespace Aspect
{
  const char USER_BIND_FRONTEND[] = "UserBindFrontend";
}

namespace Response
{
  /*
  namespace Header
  {
    const char CONTENT_TYPE[] = "Content-Type";
  }

  namespace Type
  {
    const char TEXT_HTML[] = "text/html";
  }
  */

  namespace Parameters
  {
    const String::SubString USER_ID("uid");
  }
}

namespace TemplateParams
{
  const String::SubString MARKER("##");

  const char SSPUID[] = "SSPUID";
  const char UNSIGNEDSSPUID[] = "UNSIGNEDSSPUID";

  const char UID[] = "UID";
  const char UNSIGNEDUID[] = "UNSIGNEDUID";

  const char EXTERNALID[] = "EXTERNALID";
  const char SHORTEXTERNALID[] = "SHORTEXTERNALID";
  const char YANDEXSIGN[] = "YANDEXSIGN";
  const char PASSBACK_URL[] = "PASSBACK_URL";
  const char RANDOM[] = "RANDOM";
  const String::SubString PUSH_DATA("PUSH_DATA");
  const String::SubString BINDREQUESTID("BINDREQUESTID");
}

namespace
{
  const String::SubString HTTP_PREFIX("http:");
  const String::SubString HTTPS_PREFIX("https:");
  const String::SubString IDFA_KNOWN_KEYWORD("rtbidfaknown");


  const String::SubString GOOGLE_ERRORS[] = {
    String::SubString(
      "User has a Google cookie, but has opted "
      "out of any tracking using this cookie"),
    String::SubString(
      "No valid operations specified. e.g., a "
      "no-op request was received"),
    String::SubString(
      "User does not have a Google cookie. "
      "Google will not set the cookie via the "
      "Cookie Matching Service"),
    String::SubString(
      "Conflicting operations specified. You are "
      "not allowed to specify both the google_push "
      "and google_cm flags on the same request since "
      "they have conflicting purposes"),
    String::SubString(
      "An invalid google_push parameter was passed in "
      "a redirect to a Google server as part of a Pixel "
      "Matching Service request. Your redirect must set "
      "google_push to the same value passed to you in "
      "the initial pixel request")
  };

  struct ChannelMatch
  {
    ChannelMatch(unsigned long channel_id_val,
                 unsigned long channel_trigger_id_val)
      :
      channel_id(channel_id_val),
      channel_trigger_id(channel_trigger_id_val)
    {}

    bool operator<(const ChannelMatch& right) const
    {
      return
        (channel_id < right.channel_id ||
         (channel_id == right.channel_id &&
          channel_trigger_id < right.channel_trigger_id));
    }

    unsigned long channel_id;
    unsigned long channel_trigger_id;
  };

  struct GetChannelTriggerId
  {
    ChannelMatch
    operator() (
      const AdServer::ChannelSvcs::ChannelServerBase::ChannelAtom& atom)
      throw()
    {
      return ChannelMatch(atom.id, atom.trigger_channel_id);
    }
  };

  struct ExternalUserIdResolveHolder
  {
    ExternalUserIdResolveHolder(
      const std::string* external_id_val,
      bool set_uid_val,
      bool can_be_in_cookie_val)
      : external_id(external_id_val),
        set_uid(set_uid_val),
        can_be_in_cookie(can_be_in_cookie_val)
    {}

    const std::string* external_id;
    bool set_uid;
    bool can_be_in_cookie;
  };

  void google_error(
    Stream::Error& error_strm,
    unsigned long error_code)
  {
    error_strm << "'" << error_code << "': ";
    if (error_code <= sizeof(GOOGLE_ERRORS) / sizeof(GOOGLE_ERRORS[0]) &&
        error_code > 0)
    {
      error_strm << GOOGLE_ERRORS[error_code - 1];
    }
    else
    {
      error_strm << "unknown";
    }
  }

  namespace WebStats
  {
    const String::SubString APPLICATION("adserver");
    const String::SubString SOURCE("userbind");
    const String::SubString OPERATION("mapping");
    const String::SubString INVALID_MAPPING_OPERATION("invalid-mapping");    
  }
}

namespace AdServer
{
  enum ResultUserIdType
  {
    RUIT_COOKIE,
    RUIT_CRESOLVE,
    RUIT_EXTIDRESOLVE,
    RUIT_EXTIDRESOLVE_NOCOOKIE
  };

  class KeyArgsCallback: public String::TextTemplate::ArgsCallback
  {
    virtual bool
    get_argument(const String::SubString& key, std::string& result,
      bool /*value*/) const throw (eh::Exception)
    {
      key.assign_to(result);
      return true;
    }
  };

  struct UserBindFrontend::BindResult
  {
    AdServer::Commons::UserId ssp_user_id; // will be used for init redirect template
    AdServer::Commons::UserId result_user_id;
  };

  struct UserBindFrontend::BindResultHolder
  {
  public:
    void
    set(const BindResult& result)
    {
      SyncPolicy::WriteGuard lock(lock_);
      result_ = result;
    }

    BindResult
    get() const
    {
      SyncPolicy::ReadGuard lock(lock_);
      return result_;
    }

  protected:
    typedef Sync::Policy::PosixThread SyncPolicy;

  protected:
    mutable SyncPolicy::Mutex lock_;
    BindResult result_;
  };

  //
  // UserBindFrontend::RequestTask
  //
  class UserBindFrontend::RequestTask:
    public Generics::Task,
    public ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(Invalid, Exception);

    RequestTask(
      UserBindFrontend* userbind_frontend,
      const AdServer::UserBind::RequestInfo* request_info,
      const String::SubString& dns_bind_request_id,
      const Generics::Time& start_processing_time)
      throw (Invalid)
      : to_interrupt(false),
        finished(false),
        bad_request(false),
        userbind_frontend_(userbind_frontend),
        request_info_(ReferenceCounting::add_ref(request_info)),
        dns_bind_request_id_(dns_bind_request_id.str()),
        start_processing_time_(start_processing_time)
    {}

    virtual void
    execute() throw ()
    {
      userbind_frontend_->process_request_(
        bind_result_holder_,
        *request_info_,
        dns_bind_request_id_);

      {
        Sync::ConditionalGuard guard(cond);
        if (!to_interrupt)
        {
          finished = true;
          //result_ = bind_result;
          cond.signal();
          return;
        }
      }
    }

    virtual bool
    interrupt() const throw()
    {
      Sync::PosixGuard lock(cond);
      return to_interrupt;
    }

    const Generics::Time&
    start_processing_time() const throw()
    {
      return start_processing_time_;
    }

    BindResult
    result() const throw()
    {
      return bind_result_holder_.get();
    }

  public:
    mutable Sync::Condition cond;
    bool to_interrupt;
    bool finished;
    bool bad_request;
    /// The host performed last unbreakable operation.

  protected:
    virtual ~RequestTask() throw() {}

  protected:
    UserBindFrontend* userbind_frontend_;
    AdServer::UserBind::CRequestInfo_var request_info_;
    const std::string dns_bind_request_id_;
    const Generics::Time start_processing_time_;
    BindResultHolder bind_result_holder_;
  };

  class UserBindFrontend::UserMatchTask:
    public Generics::Task,
    public ReferenceCounting::AtomicImpl
  {
  public:
    UserMatchTask(
      UserBindFrontend* user_bind_frontend,
      const Commons::UserId& result_user_id,
      const Commons::UserId& merge_user_id,
      bool create_user_profile,
      const String::SubString& keywords,
      const String::SubString& cohort,
      const String::SubString& referer,
      unsigned long colo_id)
      throw ()
      : user_bind_frontend_(user_bind_frontend),
        result_user_id_(result_user_id),
        merge_user_id_(merge_user_id),
        create_user_profile_(create_user_profile),
        keywords_(keywords.str()),
        cohort_(cohort.str()),
        referer_(referer.str()),
        colo_id_(colo_id)
    {}

    virtual
    void
    execute() throw ()
    {
      user_bind_frontend_->user_match_(
        result_user_id_,
        merge_user_id_,
        create_user_profile_,
        keywords_,
        cohort_,
        referer_,
        colo_id_);
    }

  protected:
    virtual
    ~UserMatchTask() throw ()
    {}

  private:
    UserBindFrontend* user_bind_frontend_;
    const AdServer::Commons::UserId result_user_id_;
    const AdServer::Commons::UserId merge_user_id_;
    const bool create_user_profile_;
    const std::string keywords_;
    const std::string cohort_;
    const std::string referer_;
    const unsigned long colo_id_;
  };

  //
  // UserBindFrontend implementation
  //
  UserBindFrontend::UserBindFrontend(
    Configuration* frontend_config,
    Logging::Logger* logger,
    CommonModule* common_module)
    throw(eh::Exception)
    : Logging::LoggerCallbackHolder(
        Logging::Logger_var(
          new Logging::SeveritySelectorLogger(
            logger,
            0,
            frontend_config->get().UserBindFeConfiguration()->Logger().log_level())),
        "UserBindFrontend",
        Aspect::USER_BIND_FRONTEND,
        0),
      frontend_config_(ReferenceCounting::add_ref(frontend_config)),
      common_module_(ReferenceCounting::add_ref(common_module)),
      campaign_managers_(this->logger(), Aspect::USER_BIND_FRONTEND)
  {}

  bool
  UserBindFrontend::will_handle(const String::SubString& uri) throw ()
  {
    std::string found_uri;

    bool result =
      FrontendCommons::find_uri(
        config_->PathUriList().Uri(), uri, found_uri, 0, false) ||
      FrontendCommons::find_uri(
        config_->UriList().Uri(), uri, found_uri) ||
      FrontendCommons::find_uri(
        config_->PixelUriList().Uri(), uri, found_uri);

    if(logger()->log_level() >= TraceLevel::MIDDLE)
    {
      Stream::Error ostr;
      ostr << "UserBindFrontend::will_handle(" << uri <<
        "), service: '" << found_uri << "'";

      logger()->log(ostr.str());
    }

    return result;
  }

  void
  UserBindFrontend::parse_configs_() throw(Exception)
  {
    static const char* FUN = "UserBindFrontend::parse_configs_()";

    /* load common configuration */
    Config::ErrorHandler error_handler;

    try
    {
      typedef Configuration::FeConfig Config;
      const Config& fe_config = frontend_config_->get();

      if(!fe_config.CommonFeConfiguration().present())
      {
        throw Exception("CommonFeConfiguration not presented.");
      }

      common_config_.reset(
        new CommonFeConfiguration(*fe_config.CommonFeConfiguration()));

      if(!fe_config.UserBindFeConfiguration().present())
      {
        throw Exception("BiddingFeConfiguration isn't present");
      }

      config_.reset(
        new UserBindFeConfiguration(*fe_config.UserBindFeConfiguration()));

    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": Can't parse config file '" << fe_config_path_ << "': " <<
        e.what();
      throw Exception(ostr);
    }
  }

  void
  UserBindFrontend::init() throw(eh::Exception)
  {
    static const char* FUN = "UserBindFrontend::init()";

    if(true) // module_used())
    {
      try
      {
        parse_configs_();

        corba_client_adapter_ = new CORBACommons::CorbaClientAdapter();

        if(!common_config_->UserBindControllerGroup().empty())
        {
          user_bind_client_ = new FrontendCommons::UserBindClient(
            common_config_->UserBindControllerGroup(),
            corba_client_adapter_.in(),
            logger());
          add_child_object(user_bind_client_);
        }

        user_info_client_ = new FrontendCommons::UserInfoClient(
          common_config_->UserInfoManagerControllerGroup(),
          corba_client_adapter_.in(),
          logger());
        add_child_object(user_info_client_);


        CORBACommons::CorbaObjectRefList channel_manager_controller_refs;

        Config::CorbaConfigReader::read_multi_corba_ref(
          common_config_->ChannelManagerControllerRefs().get(),
          channel_manager_controller_refs);

        channel_servers_.reset(
          new FrontendCommons::ChannelServerSessionPool(
            channel_manager_controller_refs,
            corba_client_adapter_,
            callback()));

        campaign_managers_.resolve(
          *common_config_, corba_client_adapter_);

        cookie_manager_.reset(
          new FrontendCommons::CookieManager<
            FCGI::HttpRequest, FCGI::HttpResponse>(
              common_config_->Cookies()));

        for (UserBindFeConfiguration::Source_sequence::const_iterator
             it = config_->Source().begin(); it != config_->Source().end(); ++it)
        {
          SourceEntity_var& source = sources_[it->id()];
          source = new SourceEntity();

          for(auto redirect_rule_it = it->KeywordRedirect().begin();
            redirect_rule_it != it->KeywordRedirect().end();
            ++redirect_rule_it)
          {
            std::string keywords = redirect_rule_it->keywords();
            String::SubString keywords_ss(keywords);
            source->rules.push_back(
              init_redirect_rule_(
                redirect_rule_it->redirect(),
                &keywords_ss,
                redirect_rule_it->passback()));
          }

          if(!it->redirect().empty())
          {
            source->rules.push_back(
              init_redirect_rule_(
                it->redirect(),
                0,
                it->passback()));
          }
        }

        const char* geo_ip_path = 0;

        if(common_config_->GeoIP().present())
        {
          geo_ip_path = common_config_->GeoIP()->path().c_str();
        }

        UserBind::RequestInfoFiller::ExternalUserIdSet skip_external_ids;

        if (common_config_->SkipExternalIds().present())
        {
          for(CommonFeConfiguration::SkipExternalIds_type::Id_sequence::const_iterator
                it = common_config_->SkipExternalIds()->Id().begin();
              it != common_config_->SkipExternalIds()->Id().end(); ++it)
          {
            skip_external_ids.insert(it->value());
          }
          
          String::SubString skip_ids =
            common_config_->SkipExternalIds()->skip_external_ids();

          if (!skip_ids.empty())
          {
            String::StringManip::SplitNL tokenizer(skip_ids);
            for (String::SubString skip_id; tokenizer.get_token(skip_id);)
            {
              skip_external_ids.insert(skip_id.str());
            }
          }
        }

        UserBind::RequestInfoFiller::AllowedPassbackDomainArray
          allowed_passback_domains;

        for(auto it = config_->AllowedPassback().begin();
          it != config_->AllowedPassback().end(); ++it)
        {
          if(!it->domain().empty())
          {
            allowed_passback_domains.push_back(it->domain());
          }
        }

        request_info_filler_.reset(
          new UserBind::RequestInfoFiller(
            logger(),
            common_module_,
            geo_ip_path,
            skip_external_ids,
            allowed_passback_domains,
            common_config_->colo_id()));

        if (config_->pixel_path().present())
        {
          pixel_ = FileCachePtr(new FileCache((*config_->pixel_path()).c_str()));
        }

        pixel_content_type_ = config_->pixel_content_type();

        bind_task_runner_ = new Generics::TaskRunner(
          callback(),
          config_->threads(),
          800*1024,
          config_->bind_pending_task_limit());
        add_child_object(bind_task_runner_);

        match_task_runner_ = new Generics::TaskRunner(
          callback(),
          config_->threads(),
          800*1024,
          config_->match_pending_task_limit());

        add_child_object(match_task_runner_);

        activate_object();
      }
      catch(const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": eh::Exception caught: " << ex.what();
        throw Exception(ostr);
      }

      logger()->log(String::SubString(
          "UserBindFrontend::init(): frontend is running ..."),
        Logging::Logger::INFO,
        Aspect::USER_BIND_FRONTEND);
    }
  }

  void
  UserBindFrontend::shutdown() throw ()
  {
    try
    {
      deactivate_object();
      wait_object();

      Stream::Error ostr;
      ostr << "UserBindFrontend::shutdown(): frontend terminated (pid = " <<
        ::getpid() << ").";

      logger()->log(ostr.str(),
        Logging::Logger::INFO,
        Aspect::USER_BIND_FRONTEND);
    }
    catch(...)
    {}
  }

  uint32_t
  UserBindFrontend::calc_yandex_sign_(
    const UserBind::RequestInfo& request_info,
    const std::string& user_data,
    const std::string& secure_key)
    throw ()
  {
    std::string res;
    size_t pos = static_cast<size_t>(-1);
    for(auto i = 0; i < 3; ++i)
    {
      pos = request_info.peer_ip.find('.', pos + 1);
      if(pos == std::string::npos)
      {//not valid ip
        return 0;
      }
    }
    res = request_info.peer_ip.substr(0, pos);
    res += request_info.referer;
    res += request_info.user_agent;
    //res += request_info.location;
    res += user_data;
    res += secure_key;
    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      logger()->sstream(Logging::Logger::TRACE, Aspect::USER_BIND_FRONTEND)
        << "string for calculation of yandex sum '" << res << "'";
    }
    return Generics::CRC::reversed(0, res.data(), res.size());
  }

  int
  UserBindFrontend::handle_request(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response) throw ()
  {
    int http_status = handle_request_(
      request,
      response);

    std::string found_uri;

    if(http_status == 204 &&
      FrontendCommons::find_uri(config_->PixelUriList().Uri(), request.uri(), found_uri) &&
      pixel_.in())
    {
      http_status = 200;
      response.set_content_type(pixel_content_type_);

      FileCache::BufferHolder_var buffer = pixel_->get();
      response.get_output_stream().write((*buffer)->data(), (*buffer)->size());
    }

    return http_status;
  }

  int
  UserBindFrontend::handle_delete_request_(
    const UserBind::RequestInfo& request_info)
    throw ()
  {
    //static const char* FUN = "UserBindFrontend::handle_delete_request_()";
    int http_status = 204;

    if(!request_info.external_id.empty())
    {
      try
      {
        AdServer::UserInfoSvcs::UserBindMapper_var user_bind_mapper =
          user_bind_client_->user_bind_mapper();

        // reconstruct cookie
        AdServer::UserInfoSvcs::UserBindMapper::AddUserRequestInfo
          add_user_request_info;
        add_user_request_info.id << request_info.external_id;
        add_user_request_info.user_id = CorbaAlgs::pack_user_id(
          Commons::UserId());
        add_user_request_info.timestamp = CorbaAlgs::pack_time(
          request_info.time);

        AdServer::UserInfoSvcs::UserBindServer::AddUserResponseInfo_var
          prev_user_bind_info =
            user_bind_mapper->add_user_id(add_user_request_info);

        (void)prev_user_bind_info;
      }
      catch(const AdServer::UserInfoSvcs::UserBindMapper::NotReady&)
      {
        http_status = 500;
      }
      catch(const AdServer::UserInfoSvcs::UserBindMapper::ChunkNotFound& )
      {
        http_status = 500;
      }
      catch(const AdServer::UserInfoSvcs::UserBindMapper::ImplementationException& ex)
      {
        http_status = 500;
      }
      catch(const CORBA::SystemException& e)
      {
        http_status = 500;
      }
      catch(...)
      {
        assert(0);
      }
    }

    return http_status;
  }

  int
  UserBindFrontend::handle_request_(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response) throw ()
  {
    static const char* FUN = "UserBindFrontend::handle_request()";
    int http_status = 204;

    const Generics::Time start_process_time = Generics::Time::get_time_of_day();

    try
    {
      std::string path_args;
      std::string found_uri;
      if(FrontendCommons::find_uri(
           config_->PathUriList().Uri(), request.uri(), found_uri, 0, false))
      {
        path_args = request.uri().substr(found_uri.length()).str();
      }

      UserBind::RequestInfo_var request_info_holder = new UserBind::RequestInfo();

      request_info_filler_->fill(
        *request_info_holder,
        request,
        path_args);

      const UserBind::RequestInfo& request_info = *request_info_holder;

      //request_info.dump(std::cerr);

      FrontendCommons::CORS::set_headers(request, response);

      if (request_info.google_error != 0)
      {
        /*
        Stream::Error ostr;
        ostr << FUN << ": Google error: ";
        google_error(ostr, request_info.google_error);
        logger()->log(ostr.str(),
          Logging::Logger::DEBUG,
          Aspect::USER_BIND_FRONTEND);
        */
        return 204;
      }

      bool opted_out = (request_info.user_status == AdServer::CampaignSvcs::US_OPTOUT);

      if (opted_out)
      {
        if (!request_info.user_id.is_null())
        {
          throw InvalidParamException("");
        }
      }
      else
      {
        if(request_info.external_id.empty() &&
          !request_info.generate_external_id)
        {
          throw InvalidParamException("");
        }

        // select redirect rule (for get init_bind_request)
        UserBindFrontend::RedirectRule_var redirect_rule;

        {
          const SourceMap::iterator source_it = sources_.find(request_info.source_id);

          if(source_it != sources_.end())
          {
            const SourceEntity& source = *source_it->second;

            std::vector<std::string> keywords;
            FrontendCommons::get_ip_keywords(keywords, request_info.peer_ip);

            for(auto bind_rule_it = source.rules.begin();
              bind_rule_it != source.rules.end() && !redirect_rule.in();
              ++bind_rule_it)
            {
              if((*bind_rule_it)->use_keywords)
              {
                if(!(*bind_rule_it)->passback ||
                  ((*bind_rule_it)->passback && request_info.passback))
                {
                  if((*bind_rule_it)->keywords.empty())
                  {
                    redirect_rule = *bind_rule_it;
                  }
                  else
                  {
                    for(auto keyword_it = keywords.begin(); keyword_it != keywords.end(); ++keyword_it)
                    {
                      if((*bind_rule_it)->keywords.find(*keyword_it) != (*bind_rule_it)->keywords.end())
                      {
                        redirect_rule = *bind_rule_it;
                        break;
                      }
                    }
                  }
                }
              }
              else if(!(*bind_rule_it)->passback ||
                ((*bind_rule_it)->passback && request_info.passback))
              {
                redirect_rule = *bind_rule_it;
              }
            }
          }
        }

        std::string dns_bind_request_id;

        if(redirect_rule && redirect_rule->init_bind_request)
        {
          AdServer::Commons::RequestId gen_id =
            AdServer::Commons::RequestId::create_random_based();
          AdServer::Commons::base32_encode(
            dns_bind_request_id,
            String::SubString(reinterpret_cast<const char*>(&*gen_id.begin()), 16));
          dns_bind_request_id = std::string("r") + dns_bind_request_id;
        }

        const Generics::Time task_expire_time(
          start_process_time + Generics::Time(config_->max_request_time()) / 1000);

        // enqueue task & wait
        RequestTask_var request_task = new RequestTask(
          this,
          request_info_holder,
          dns_bind_request_id,
          start_process_time);

        try
        {
          bind_task_runner_->enqueue_task(request_task);

          {
            Sync::ConditionalGuard guard(request_task->cond);

            do
            {
              if(!guard.timed_wait(&task_expire_time))
              {
                // timed out
                request_task->to_interrupt = true;
                //finished = false;
                break;
              }
            }
            while(!request_task->finished);

            //bad_request = request_task->bad_request;
          }
        }
        catch(const Generics::TaskRunner::Overflow&)
        {
          request_task->to_interrupt = true;
        }

        // fill response
        const BindResult bind_result = request_task->result();

        // result_user_id contains user id that will be stored into cookie
        try
        {
          if(redirect_rule.in())
          {
            String::TextTemplate::Args templ_args;

            // fill params by request
            templ_args[TemplateParams::EXTERNALID] = request_info.external_id;
            templ_args[TemplateParams::SHORTEXTERNALID] = request_info.short_external_id;
            templ_args[TemplateParams::RANDOM] =
              String::StringManip::IntToStr(
                Generics::safe_rand()).str().str();

            if(!request_info.passback_url.empty())
            {
              templ_args[TemplateParams::PASSBACK_URL] = request_info.passback_url;
            }

            // fill params got by processing
            std::string ssp_user_id_str;

            if(!bind_result.ssp_user_id.is_null())
            {
              ssp_user_id_str = common_module_->user_id_controller()->ssp_uuid_string(
                bind_result.ssp_user_id);

              templ_args[TemplateParams::SSPUID] =
                common_module_->user_id_controller()->sign(
                  bind_result.ssp_user_id, UserIdController::SSP).str();
              templ_args[TemplateParams::UNSIGNEDSSPUID] = ssp_user_id_str;
            }

            if(!bind_result.result_user_id.is_null())
            {
              const Generics::SignedUuid signed_uid =
                common_module_->user_id_controller()->sign(bind_result.result_user_id);
              templ_args[TemplateParams::UID] = signed_uid.str();
              templ_args[TemplateParams::UNSIGNEDUID] = bind_result.result_user_id.to_string();
            }

            uint32_t yandex_sign = 0;
            if(request_info.external_id.empty() && config_->Keys().present())
            {
              yandex_sign = calc_yandex_sign_(
                request_info,
                ssp_user_id_str,
                config_->Keys().get().yandex_key().present() ?
                  config_->Keys().get().yandex_key().get() : "" );
            }

            templ_args[TemplateParams::YANDEXSIGN] =
              String::StringManip::IntToStr(yandex_sign).str().str();

            if(!request_info.push_data.empty())
            {
              templ_args[TemplateParams::PUSH_DATA] = request_info.push_data;
            }

            if(!dns_bind_request_id.empty())
            {
              templ_args[TemplateParams::BINDREQUESTID] = dns_bind_request_id;
            }

            String::TextTemplate::DefaultValue args_with_default(&templ_args);
            String::TextTemplate::ArgsEncoder args_with_encoding(&args_with_default);

            std::string redirect =
              redirect_rule->redirect.instantiate(args_with_encoding);

            if(!redirect.empty())
            {
              if(!request_info.ssp_id.empty())
              {
                std::string mimed_ssp_id;
                String::StringManip::mime_url_encode(request_info.ssp_id, mimed_ssp_id);

                if(redirect.find('?') == std::string::npos)
                {
                  redirect += '?';
                }
                else
                {
                  redirect += '&';
                }

                redirect += "ssp=";
                redirect += mimed_ssp_id;
              }

              const std::string str = FrontendCommons::normalize_abs_url(
                HTTP::BrowserAddress(redirect),
                HTTP::HTTPAddress::VW_FULL,
                FrontendCommons::is_secure_request(request) || request_info.secure ?
                HTTPS_PREFIX : HTTP_PREFIX);

              http_status = FrontendCommons::redirect(str, response);
            }
          } // redirect_rule
        }
        catch(const eh::Exception& e)
        {
          Stream::Error ostr;
          ostr << FUN << ": source processing failed for external_id = '" <<
            request_info.external_id << "'; " <<
            e.what();
          logger()->log(ostr.str(),
            Logging::Logger::ERROR,
            Aspect::USER_BIND_FRONTEND);
        }

        if(!bind_result.result_user_id.is_null())
        {
          // set new user id
          const Generics::SignedUuid signed_uid =
            common_module_->user_id_controller()->sign(bind_result.result_user_id);
          cookie_manager_->set(
            response,
            request,
            Response::Parameters::USER_ID,
            signed_uid.str());
        }
      } // !opted_out

      //log_cookie_mapping_(request_info);

      if(http_status == 204 && !request_info.passback_url.empty())
      {
        http_status = FrontendCommons::redirect(request_info.passback_url, response);
      }
    }
    catch(const InvalidParamException&)
    {
      http_status = 400;
    }
    catch(const eh::Exception& e)
    {
      http_status = 500;

      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << e.what();

      logger()->log(ostr.str(),
        Logging::Logger::EMERGENCY,
        Aspect::USER_BIND_FRONTEND,
        "ADS-IMPL-109");
    }

    return http_status;
  }

  int
  UserBindFrontend::process_request_(
    BindResultHolder& bind_result_holder,
    const UserBind::RequestInfo& request_info,
    const String::SubString& dns_bind_request_id)
    throw()
  {
    static const char* FUN = "UserBindFrontend::process_request_()";

    int http_status = 200; // non used on response

    if (request_info.delete_op)
    {
      return handle_delete_request_(request_info);
    }

    try
    {
      bool country_filtered = false;

      FrontendCommons::CountryFilter_var country_filter =
        common_module_->country_filter();

      if(country_filter.in() && (
           request_info.location.in() == 0 ||
           !country_filter->enabled(request_info.location->country)))
      {
        country_filtered = true;
      }

      AdServer::Commons::UserId result_user_id = request_info.user_id;
      AdServer::Commons::UserId result_ssp_user_id;
      ResultUserIdType result_user_id_type = RUIT_COOKIE;

      AdServer::Commons::UserId merge_user_id;
      bool app_request = (request_info.external_id.compare(0, 4, "ifa/") == 0);

      if(app_request) // skip cookie in application request
      {
        result_user_id = AdServer::Commons::UserId();
      }

      if(!country_filtered)
      {
        bool opted_out = (request_info.user_status == AdServer::CampaignSvcs::US_OPTOUT);
        bool cresolve_failed = false;
        bool create_user_profile = false;

        if(!result_user_id.is_null())
        {
          if(user_bind_client_)
          {
            try
            {
              AdServer::UserInfoSvcs::UserBindMapper_var user_bind_mapper =
                user_bind_client_->user_bind_mapper();

              const std::string cookie_external_id_str =
                std::string("c/") + result_user_id.to_string();

              AdServer::UserInfoSvcs::UserBindMapper::GetUserRequestInfo get_request_info;
              get_request_info.id << cookie_external_id_str;
              get_request_info.timestamp = CorbaAlgs::pack_time(request_info.time);
              get_request_info.silent = true;
              get_request_info.generate_user_id = false;
              get_request_info.for_set_cookie = true;
              get_request_info.create_timestamp = CorbaAlgs::pack_time(Generics::Time::ZERO);
              get_request_info.current_user_id = CorbaAlgs::pack_user_id(result_user_id);
              AdServer::UserInfoSvcs::UserBindServer::GetUserResponseInfo_var
                prev_user_bind_info =
                  user_bind_mapper->get_user_id(get_request_info);

              if(prev_user_bind_info->invalid_operation)
              {
                cresolve_failed = true;
              }
              else
              {
                Commons::UserId cresolved_user_id =
                  CorbaAlgs::unpack_user_id(prev_user_bind_info->user_id);
                if(!cresolved_user_id.is_null())
                {
                  result_user_id = cresolved_user_id;
                  result_user_id_type = RUIT_CRESOLVE;
                }
              }
            }
            catch(const AdServer::UserInfoSvcs::UserBindMapper::NotReady&)
            {
              cresolve_failed = true;
              http_status = 500;
            }
            catch(const AdServer::UserInfoSvcs::UserBindMapper::ChunkNotFound& )
            {
              cresolve_failed = true;
              http_status = 500;
            }
            catch(const AdServer::UserInfoSvcs::UserBindMapper::ImplementationException& ex)
            {
              cresolve_failed = true;
              http_status = 500;
            }
            catch(const CORBA::SystemException& e)
            {
              cresolve_failed = true;
              http_status = 500;
            }
            catch(...)
            {
              assert(0);
            }
          }
          else
          {
            cresolve_failed = true;
          }
        }

        if(!cresolve_failed)
        {
          std::set<std::string> resolve_failed_external_ids;

          if(user_bind_client_)
          {
            // use first resolved user_id if cookie user id not present
            // next ids link to resolved
            const ExternalUserIdResolveHolder use_external_ids[] = {
              ExternalUserIdResolveHolder(&request_info.ga_user_id, false, true),
              ExternalUserIdResolveHolder(&request_info.gclu_user_id, false, true),
              ExternalUserIdResolveHolder(&request_info.ym_user_id, false, true),
              ExternalUserIdResolveHolder(&request_info.external_id, config_->set_uid(), !app_request)
            };

            unsigned long resolved_ext_user_i = 0;

            if(result_user_id.is_null())
            {
              for(unsigned long ext_user_i = 0;
                ext_user_i < sizeof(use_external_ids) / sizeof(use_external_ids[0]);
                ++ext_user_i)
              {
                const std::string& cur_external_id =
                  *(use_external_ids[ext_user_i].external_id);

                if(!cur_external_id.empty())
                {
                  try
                  {
                    AdServer::UserInfoSvcs::UserBindMapper_var user_bind_mapper =
                      user_bind_client_->user_bind_mapper();

                    const std::string& external_id_str = cur_external_id;

                    // reconstruct cookie
                    AdServer::UserInfoSvcs::UserBindMapper::GetUserRequestInfo
                      get_request_info;
                    get_request_info.id << external_id_str;
                    get_request_info.timestamp = CorbaAlgs::pack_time(request_info.time);
                    get_request_info.silent = true;
                    get_request_info.generate_user_id = use_external_ids[ext_user_i].set_uid;
                    get_request_info.for_set_cookie = !app_request;
                    get_request_info.create_timestamp = CorbaAlgs::pack_time(Generics::Time::ZERO);
                    // get_request_info.current_user_id is null

                    AdServer::UserInfoSvcs::UserBindServer::GetUserResponseInfo_var
                      prev_user_bind_info = user_bind_mapper->get_user_id(
                        get_request_info);

                    if(prev_user_bind_info->invalid_operation)
                    {
                      resolve_failed_external_ids.insert(external_id_str);

                      // report into webstats
                      report_bad_user_(request_info);
                    }
                    else
                    {
                      AdServer::Commons::UserId resolved_user_id =
                        CorbaAlgs::unpack_user_id(prev_user_bind_info->user_id);
                      if(!resolved_user_id.is_null())
                      {
                        result_user_id = resolved_user_id;
                        result_user_id_type = use_external_ids[ext_user_i].can_be_in_cookie ?
                          RUIT_EXTIDRESOLVE : RUIT_EXTIDRESOLVE_NOCOOKIE;
                        common_module_->user_id_controller()->null_blacklisted(
                          result_user_id);

                        if(config_->create_profile())
                        {
                          create_user_profile = prev_user_bind_info->created;
                        }
                      }
                    } // prev_user_bind_info->invalid_operation

                    if(!result_user_id.is_null())
                    {
                      resolved_ext_user_i = ext_user_i;
                      break;
                    }
                  }
                  catch(const AdServer::UserInfoSvcs::UserBindMapper::NotReady&)
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught UserBindServer::NotReady";
                    logger()->log(ostr.str(),
                      Logging::Logger::EMERGENCY,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-IMPL-109");
                    */
                  }
                  catch(const AdServer::UserInfoSvcs::UserBindMapper::ChunkNotFound& )
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught UserBindMapper::ChunkNotFound";
                    logger()->log(ostr.str(),
                      Logging::Logger::ERROR,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-IMPL-109");
                    */
                  }
                  catch(const AdServer::UserInfoSvcs::UserBindMapper::ImplementationException& /*ex*/)
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught UserBindMapper::ImplementationException: " <<
                      ex.description;
                    logger()->log(ostr.str(),
                      Logging::Logger::ERROR,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-IMPL-109");
                    */
                  }
                  catch(const CORBA::SystemException& /*e*/)
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught CORBA::SystemException: " << e;
                    logger()->log(ostr.str(),
                      Logging::Logger::ERROR,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-ICON-6");
                    */
                  }
                  catch(...)
                  {
                    assert(0);
                  }
                } // cur_external_id.empty()
              } // for use_external_ids
            } // result_user_id.is_null

            if(opted_out || !result_user_id.is_null())
            {
              for(unsigned long ext_user_i = 0;
                ext_user_i < sizeof(use_external_ids) / sizeof(use_external_ids[0]);
                ++ext_user_i)
              {
                const std::string& cur_external_id =
                  *(use_external_ids[ext_user_i].external_id);

                if(!cur_external_id.empty() &&
                  ext_user_i != resolved_ext_user_i &&
                  resolve_failed_external_ids.find(cur_external_id) ==
                    resolve_failed_external_ids.end())
                {
                  try
                  {
                    AdServer::UserInfoSvcs::UserBindMapper_var user_bind_mapper =
                      user_bind_client_->user_bind_mapper();

                    AdServer::UserInfoSvcs::UserBindMapper::AddUserRequestInfo
                      add_user_request_info;
                    add_user_request_info.id << cur_external_id;
                    add_user_request_info.user_id = CorbaAlgs::pack_user_id(
                      !opted_out ? result_user_id : Commons::UserId());
                    add_user_request_info.timestamp = CorbaAlgs::pack_time(
                      request_info.time);

                    AdServer::UserInfoSvcs::UserBindServer::AddUserResponseInfo_var
                      prev_user_bind_info =
                        user_bind_mapper->add_user_id(add_user_request_info);

                    AdServer::Commons::UserId prev_user_id =
                      CorbaAlgs::unpack_user_id(prev_user_bind_info->merge_user_id);

                    if(prev_user_bind_info->invalid_operation)
                    {
                      // report to web stats
                      report_bad_user_(request_info);
                    }
                  }
                  catch(const AdServer::UserInfoSvcs::UserBindMapper::NotReady&)
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught UserBindServer::NotReady";
                    logger()->log(ostr.str(),
                      Logging::Logger::EMERGENCY,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-IMPL-109");
                    */
                  }
                  catch(const AdServer::UserInfoSvcs::UserBindMapper::ChunkNotFound& )
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught UserBindMapper::ChunkNotFound";
                    logger()->log(ostr.str(),
                      Logging::Logger::ERROR,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-IMPL-109");
                    */
                  }
                  catch(const AdServer::UserInfoSvcs::UserBindMapper::ImplementationException& /*ex*/)
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught UserBindMapper::ImplementationException: " <<
                      ex.description;
                    logger()->log(ostr.str(),
                      Logging::Logger::ERROR,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-IMPL-109");
                    */
                  }
                  catch(const CORBA::SystemException& e)
                  {
                    http_status = 500;

                    /*
                    Stream::Error ostr;
                    ostr << FUN << ": caught CORBA::SystemException: " << e;
                    logger()->log(ostr.str(),
                      Logging::Logger::ERROR,
                      Aspect::USER_BIND_FRONTEND,
                      "ADS-ICON-6");
                    */
                  }
                  catch(...)
                  {
                    assert(0);
                  }
                } // cur_external_id.empty()
              } // for use_external_ids
            } // opted_out || !result_user_id.is_null()
          } // user_bind_client_

          // don't generate user id if some resolve failed and user id not defined
          if(!opted_out &&
            !app_request && (
              !result_user_id.is_null() || resolve_failed_external_ids.empty()))
          {
            // request_info.generate_external_id is true
            // user bind request from RTB's that don't provide own id's
            // and only keep id that give adserver
            Generics::Uuid generated_user_id = result_user_id.is_null() ?
              Generics::Uuid::create_random_based() :
              result_user_id;

            if(request_info.generate_external_id)
            {
              result_ssp_user_id = common_module_->user_id_controller()->ssp_uuid(
                generated_user_id,
                request_info.source_id);
            }

            if(config_->set_uid())
            {
              result_user_id = generated_user_id;
              result_user_id_type = RUIT_COOKIE;
            }
          } // !opted_out
        } // !cresolve_failed

        // set result
        {
          BindResult bind_result;
          bind_result.result_user_id = result_user_id;
          if(!result_ssp_user_id.is_null())
          {
            bind_result.ssp_user_id = result_ssp_user_id;
          }
          else if(!app_request && !result_user_id.is_null())
          {
            bind_result.ssp_user_id = common_module_->user_id_controller()->ssp_uuid(
              result_user_id,
              request_info.source_id);
          }

          bind_result_holder.set(bind_result);
        }
        
        std::string ifa_str;
        if(request_info.external_id.compare(0, 4, "ifa/") == 0)
        {
          ifa_str = request_info.external_id.substr(4);
          String::AsciiStringManip::to_lower(ifa_str);
        }

        try
        {
          if(config_->enable_profiling() &&
             !result_user_id.is_null() && (
               !request_info.referer.empty() ||
               !ifa_str.empty() ||
               !merge_user_id.is_null() ||
               create_user_profile))
          {
            // delay match channels
            match_task_runner_->enqueue_task(new UserMatchTask(
              this,
              result_user_id,
              merge_user_id, // is null - merging on userbind disabled
              create_user_profile,
              !ifa_str.empty() ? IDFA_KNOWN_KEYWORD : String::SubString(),
              ifa_str, // push to cohort
              request_info.referer,
              request_info.colo_id));
          }
        }
        catch (const Generics::TaskRunner::Overflow&)
        {}

        if(create_user_profile)
        {
          // send request to CampaignManager
          campaign_managers_.verify_opt_operation(
            request_info.time.tv_sec,
            request_info.colo_id,
            "",
            AdServer::CampaignSvcs::CampaignManager::OO_FORCED_IN,
            0, // status
            CampaignSvcs::US_UNDEFINED,//unused for OO_FORCED_IN
            false, // log_as_test
            "", // browser
            "", // os
            "", // ct
            "", // curct
            "", // aspect
            result_user_id);
        }
      } // !country_filtered

      if(!dns_bind_request_id.empty())
      {
        AdServer::Commons::ExternalUserIdArray user_ids;

        {
          // fill user ids for bind request
          if(!result_user_id.is_null())
          {
            if(result_user_id_type == RUIT_CRESOLVE ||
              result_user_id_type == RUIT_EXTIDRESOLVE_NOCOOKIE)
            {
              user_ids.push_back(
                std::string("/") + result_user_id.to_string());
            }
            else // RUIT_COOKIE || RUIT_EXTIDRESOLVE
            {
              user_ids.push_back(
                std::string("c/") + result_user_id.to_string());
            }
          }

          if(!app_request &&
            !request_info.user_id.is_null() &&
            !(request_info.user_id == result_user_id))
          {
            user_ids.push_back(
              std::string("c/") + request_info.user_id.to_string());
          }

          if(!request_info.ga_user_id.empty())
          {
            user_ids.push_back(request_info.ga_user_id);
          }

          if(!request_info.ym_user_id.empty())
          {
            user_ids.push_back(request_info.ym_user_id);
          }

          if(!request_info.external_id.empty())
          {
            user_ids.push_back(request_info.external_id);
          }

          if(!request_info.add_user_id.is_null() &&
            request_info.add_user_id != result_user_id &&
            request_info.add_user_id != request_info.user_id)
          {
            user_ids.push_back(std::string("/") + request_info.add_user_id.to_string());
          }
        }

        try
        {
          AdServer::UserInfoSvcs::UserBindMapper_var user_bind_mapper =
            user_bind_client_->user_bind_mapper();

          AdServer::UserInfoSvcs::UserBindServer::BindRequestInfo bind_request;

          bind_request.bind_user_ids.length(user_ids.size());
          CORBA::ULong bind_user_id_i = 0;
          for(auto it = user_ids.begin(); it != user_ids.end(); ++it, ++bind_user_id_i)
          {
            bind_request.bind_user_ids[bind_user_id_i] << *it;
          }

          user_bind_mapper->add_bind_request(
            dns_bind_request_id.str().c_str(),
            bind_request,
            CorbaAlgs::pack_time(request_info.time));
        }
        catch(const AdServer::UserInfoSvcs::UserBindMapper::ChunkNotFound& )
        {}
        catch(const AdServer::UserInfoSvcs::UserBindMapper::ImplementationException& ex)
        {}
        catch(const CORBA::SystemException& e)
        {}
      } // !dns_bind_request_id.empty()
    }
    catch(const eh::Exception& e)
    {
      http_status = 500;

      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << e.what();

      logger()->log(ostr.str(),
        Logging::Logger::EMERGENCY,
        Aspect::USER_BIND_FRONTEND,
        "ADS-IMPL-109");
    }    

    return http_status;
  }

  void
  UserBindFrontend::report_bad_user_(
    const UserBind::RequestInfo& /*request_info*/)
    throw()
  {
    try
    {
      /*
      AdServer::CampaignSvcs::CampaignManager::WebOperationInfo web_op;
      web_op.time = CorbaAlgs::pack_time(request_info.time);
      web_op.colo_id = request_info.colo_id;
      web_op.tag_id = 0;
      web_op.cc_id = 0;
      web_op.curct << request_info.external_id;
      web_op.app << WebStats::APPLICATION;
      web_op.source << WebStats::SOURCE;
      web_op.operation << WebStats::INVALID_MAPPING_OPERATION;
      web_op.user_bind_src << request_info.source_id;
      web_op.result = 'F';
      web_op.user_status = static_cast<unsigned long>(request_info.user_status);
      web_op.test_request = false;

      campaign_managers_.consider_web_operation(web_op);
      */
    }
    catch (AdServer::CampaignSvcs::CampaignManager::IncorrectArgument&)
    {}
    catch (const Exception&)
    {}
    catch(const eh::Exception&)
    {}
  }

  void
  UserBindFrontend::log_cookie_mapping_(
    const UserBind::RequestInfo& request_info)
    throw ()
  {
    static const char* FUN = "UserBindFrontend::log_cookie_mapping_()";

    if (config_->cookie_mapping_log())
    {
      try
      {
        CampaignSvcs::CampaignManager::WebOperationInfo web_op_info;
        web_op_info.time = CorbaAlgs::pack_time(request_info.time);
        web_op_info.colo_id = request_info.colo_id;
        web_op_info.tag_id = 0;
        web_op_info.cc_id = 0;
        web_op_info.app << WebStats::APPLICATION;
        web_op_info.source << WebStats::SOURCE;
        web_op_info.operation << WebStats::OPERATION;
        web_op_info.result = 'U';
        web_op_info.user_status = static_cast<unsigned long>(request_info.user_status);
        web_op_info.test_request = false;
        web_op_info.user_bind_src << request_info.source_id;
        campaign_managers_.consider_web_operation(web_op_info);
      }
      catch (AdServer::CampaignSvcs::CampaignManager::IncorrectArgument&)
      {
        Stream::Error ostr;
        ostr << FUN << ": CampaignManager::IncorrectArgument caught";
        logger()->log(ostr.str(),
          Logging::Logger::ERROR,
          Aspect::USER_BIND_FRONTEND,
          "ADS-IMPL-7805");
      }
      catch (eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": eh::Exception caught: " << ex.what();
        logger()->log(ostr.str(),
          Logging::Logger::ERROR,
          Aspect::USER_BIND_FRONTEND,
          "ADS-IMPL-7805");
      }
    }
  }

  void
  UserBindFrontend::user_match_(
    const Commons::UserId& result_user_id,
    const Commons::UserId& merge_user_id,
    bool create_user_profile,
    const String::SubString& keywords,
    const String::SubString& cohort,
    const String::SubString& referer,
    unsigned long colo_id)
    throw()
  {
    static const char* FUN = "UserBindFrontend::user_match_()";

    AdServer::ChannelSvcs::ChannelServerBase::MatchResult_var trigger_match_result;

    if((!referer.empty() || !keywords.empty()) &&
      !result_user_id.is_null())
    {
      // do trigger match
      try
      {
        AdServer::ChannelSvcs::ChannelServerBase::MatchQuery query;
        query.non_strict_word_match = false;
        query.non_strict_url_match = false;
        query.return_negative = false;
        query.simplify_page = false;
        query.statuses[0] = 'A';
        query.statuses[1] = '\0';
        query.fill_content = false;
        query.pwords << keywords;
        query.first_url << referer;

        channel_servers_->match(query, trigger_match_result);
      }
      catch(const FrontendCommons::ChannelServerSessionPool::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN <<
          ": caught ChannelServerSessionPool::Exception: " <<
          ex.what();
        logger()->log(ostr.str(),
          Logging::Logger::EMERGENCY,
          Aspect::USER_BIND_FRONTEND,
          "ADS-IMPL-117");
      }
    }

    if(!merge_user_id.is_null() || create_user_profile ||
      !cohort.empty() || !keywords.empty())
    {
      // merge merge_user_id into user_id
      AdServer::UserInfoSvcs::UserInfoMatcher_var
        uim_session = user_info_client_->user_info_session();

      if(uim_session.in())
      {
        try
        {
          AdServer::UserInfoSvcs::UserInfoMatcher::MatchParams match_params;

          if(trigger_match_result.ptr())
          {
            typedef std::set<ChannelMatch> ChannelMatchSet;
            ChannelMatchSet page_channels;

            std::transform(
              trigger_match_result->matched_channels.page_channels.get_buffer(),
              trigger_match_result->matched_channels.page_channels.get_buffer() +
                trigger_match_result->matched_channels.page_channels.length(),
              std::inserter(page_channels, page_channels.end()),
              GetChannelTriggerId());

            match_params.page_channel_ids.length(page_channels.size());
            CORBA::ULong res_ch_i = 0;
            for (ChannelMatchSet::const_iterator ch_it = page_channels.begin();
              ch_it != page_channels.end(); ++ch_it, ++res_ch_i)
            {
              match_params.page_channel_ids[res_ch_i].channel_id = ch_it->channel_id;
              match_params.page_channel_ids[res_ch_i].channel_trigger_id =
                ch_it->channel_trigger_id;
            }
          }

          match_params.use_empty_profile = false;
          match_params.silent_match = false;
          match_params.no_match = false;
          match_params.no_result = true;
          match_params.ret_freq_caps = false;
          match_params.provide_channel_count = false;
          match_params.provide_persistent_channels = false;
          match_params.change_last_request = false;
          match_params.filter_contextual_triggers = false;
          match_params.publishers_optin_timeout = CorbaAlgs::pack_time(
            Generics::Time::ZERO);
          match_params.cohort << cohort;

          AdServer::UserInfoSvcs::UserInfo user_info;
          user_info.user_id = CorbaAlgs::pack_user_id(result_user_id);
          user_info.last_colo_id = colo_id;
          user_info.request_colo_id = colo_id;
          user_info.current_colo_id = -1;
          user_info.temporary = false;
          user_info.time = Generics::Time::get_time_of_day().tv_sec;

          bool do_match = true;

          if(!merge_user_id.is_null())
          {
            AdServer::UserInfoSvcs::UserProfiles_var merge_user_profile;

            AdServer::UserInfoSvcs::ProfilesRequestInfo profiles_request;
            profiles_request.base_profile = true;
            profiles_request.add_profile = true;
            profiles_request.history_profile = true;
            profiles_request.freq_cap_profile = true;

            if(uim_session->get_user_profile(
                 CorbaAlgs::pack_user_id(merge_user_id),
                 false, // persistent profile
                 profiles_request,
                 merge_user_profile.out()))
            {
              bool merge_success;
              CORBACommons::TimestampInfo_var last_request;

              uim_session->merge(
                user_info,
                match_params,
                merge_user_profile.in(),
                merge_success,
                last_request);

              do_match = false;

              uim_session->remove_user_profile(
                CorbaAlgs::pack_user_id(merge_user_id));
            }
          }

          if(do_match) // create_user_profile or not found merge profile
          {
            AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult_var match_result;
            uim_session->match(user_info, match_params, match_result.out());
          }
        }
        catch(const AdServer::UserInfoSvcs::UserInfoMatcher::ImplementationException& e)
        {
          Stream::Error ostr;
          ostr << FUN << ": caught UserInfoMatcher::ImplementationException: " <<
            "user_id = '" << result_user_id.to_string() << "'; " <<
            e.description;
          logger()->log(ostr.str(),
            Logging::Logger::EMERGENCY,
            Aspect::USER_BIND_FRONTEND,
            "ADS-IMPL-7803");
        }
        catch(const AdServer::UserInfoSvcs::UserInfoMatcher::NotReady& e)
        {
          Stream::Error ostr;
          ostr << FUN << ": caught UserInfoMatcher::NotReady.";
          logger()->log(ostr.str(),
            Logging::Logger::EMERGENCY,
            Aspect::USER_BIND_FRONTEND,
            "ADS-IMPL-7804");
        }
        catch(const CORBA::SystemException& e)
        {
          Stream::Error ostr;
          ostr << FUN << ": caught CORBA::SystemException: " << e;
          logger()->log(ostr.str(),
            Logging::Logger::EMERGENCY,
            Aspect::USER_BIND_FRONTEND,
            "ADS-ICON-7800");
        }
      } // uim_session
    } // merge_user_id
  }

  UserBindFrontend::RedirectRule_var
  UserBindFrontend::init_redirect_rule_(
    const String::SubString& redirect,
    const String::SubString* keywords,
    bool passback)
    throw(UserBindFrontend::InvalidSource)
  {
    static const char* FUN = "UserBindFrontend::init_redirect_rule_";

    try
    {
      UserBindFrontend::RedirectRule_var redirect_rule = new UserBindFrontend::RedirectRule();

      Stream::Parser istr(redirect);
      redirect_rule->redirect.init(istr, TemplateParams::MARKER, TemplateParams::MARKER);

      AdServer::Commons::UserId test_uid;
      std::string test_uid_str = test_uid.to_string(false);

      String::TextTemplate::Args templ_args;
      templ_args[TemplateParams::SSPUID] = "";
      templ_args[TemplateParams::UNSIGNEDSSPUID] = test_uid_str;
      templ_args[TemplateParams::EXTERNALID] = "";
      templ_args[TemplateParams::SHORTEXTERNALID] = "";
      templ_args[TemplateParams::YANDEXSIGN] = "0";
      templ_args[TemplateParams::RANDOM] = "0";
      templ_args[TemplateParams::PUSH_DATA] = "";
      templ_args[TemplateParams::BINDREQUESTID] = "r1";

      String::TextTemplate::DefaultValue args_with_default(&templ_args);
      String::TextTemplate::ArgsEncoder args_with_encoding(&args_with_default);

      const std::string str = redirect_rule->redirect.instantiate(args_with_encoding);
      if(!str.empty())
      {
        HTTP::HTTPChecker checker;
        std::string error;

        if (!checker(str, &error, false))
        {
          Stream::Error ostr;
          ostr << FUN << ": invalid redirect template = '" << redirect <<
            "', error = " << error;
          throw InvalidSource(ostr);
        }
      }

      if(keywords)
      {
        String::StringManip::Splitter<String::AsciiStringManip::SepNL> splitter(
          *keywords);
        String::SubString token;
        while(splitter.get_token(token))
        {
          String::StringManip::trim(token);
          if(!token.empty())
          {
            redirect_rule->keywords.insert(Generics::StringHashAdapter(token));
          }
        }

        redirect_rule->use_keywords = true;
      }
      else
      {
        redirect_rule->use_keywords = false;
      }

      redirect_rule->passback = passback;
      redirect_rule->init_bind_request = false;

      {
        String::TextTemplate::Keys keys;

        KeyArgsCallback null_args;
        String::TextTemplate::DefaultValue default_cont(&null_args);
        String::TextTemplate::ArgsEncoder encoder(&default_cont);
        redirect_rule->redirect.keys(encoder, keys);

        if(keys.find(TemplateParams::BINDREQUESTID.str()) != keys.end())
        {
          redirect_rule->init_bind_request = true;
        }
      }

      return redirect_rule;
    }
    catch (const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << ex.what();
      throw InvalidSource(ostr);
    }
  }
  
}
