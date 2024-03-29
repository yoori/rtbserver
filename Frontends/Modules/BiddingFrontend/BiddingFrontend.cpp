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

#include <sstream>
#include <algorithm>
#include <set>

#include <HTTP/HTTPCookie.hpp>
#include <String/StringManip.hpp>
#include <String/AsciiStringManip.hpp>

#include <Generics/Uuid.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Generics/HashTableAdapters.hpp>

#include <CORBACommons/CorbaAdapters.hpp>

#include <Commons/UserInfoManip.hpp>
#include <Commons/Algs.hpp>
#include <Commons/DelegateTaskGoal.hpp>
#include <Commons/ExternalUserIdUtils.hpp>

#include <Commons/CorbaConfig.hpp>
#include <Commons/CorbaAlgs.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <LogCommons/AdRequestLogger.hpp>
#include <ChannelSvcs/ChannelCommons/ChannelUtils.hpp>
#include <ChannelSvcs/ChannelManagerController/ChannelSessionFactory.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

#include "BiddingFrontend.hpp"

namespace Response
{
  namespace OpenX
  {
    const String::SubString AD_OX_CATS("ad_ox_cats");
    const String::SubString MATCHING_AD_ID("matching_ad_id");
  }

  namespace OpenRtb
  {
    const String::SubString ID("id");
    const String::SubString DEAL_ID("dealid");
    const String::SubString BID("bid");
    const String::SubString BIDID("bidid");
    const String::SubString CUR("cur");
    const String::SubString EXT("ext");
    const String::SubString PROTOCOL("protocol");
    const String::SubString SEATBID("seatbid");
    const String::SubString ADID("adid");
    const String::SubString PRICE("price");
    const String::SubString IMPID("impid");
    const String::SubString CRID("crid");
    const String::SubString ADOMAIN("adomain");
    const String::SubString ADVERTISER_NAME("advertiser_name");
    const String::SubString VAST_URL("vast_url");
    const String::SubString ADM("adm");
    const String::SubString NURL("nurl");
    const String::SubString CAT("cat");
    const String::SubString IURL("iurl");

    const String::SubString WIDTH("w");
    const String::SubString HEIGHT("h");
    const String::SubString CID("cid");
    const String::SubString SEAT("seat");
    const String::SubString ATTR("attr");

    // ADSC-10918 Native ads
    const String::SubString NATIVE("native");
    const String::SubString NATIVE_VER("ver");
    const String::SubString CURRENT_NATIVE_VER("1");
    const String::SubString NATIVE_LINK("link");
    const String::SubString NATIVE_LINK_URL("url");
    const String::SubString NATIVE_LINK_CLICK_TRACKERS("clicktrackers");
    const String::SubString NATIVE_IMP_TRACKERS("imptrackers");
    const String::SubString NATIVE_JS_TRACKER("jstracker");
    const String::SubString NATIVE_ASSETS("assets");
    const String::SubString NATIVE_ASSET_ID("id");
    const String::SubString NATIVE_ASSET_REQUIRED("required");
    const String::SubString NATIVE_ASSET_IMG("img");
    const String::SubString NATIVE_ASSET_IMG_URL("url");
    const String::SubString NATIVE_ASSET_IMG_W("w");
    const String::SubString NATIVE_ASSET_IMG_H("h");
    const String::SubString NATIVE_ASSET_DATA("data");
    const String::SubString NATIVE_ASSET_DATA_VALUE("value");
    const String::SubString NATIVE_ASSET_TITLE("title");
    const String::SubString NATIVE_ASSET_TITLE_TEXT("text");
    const String::SubString NATIVE_ASSET_VIDEO("video");
    const String::SubString NATIVE_ASSET_VIDEO_TAG("vasttag");
  }

  namespace Yandex
  {
    const String::SubString ID("id");
    const String::SubString SETUSERDATA("setuserdata");
    const String::SubString BIDSET("bidset");
    const String::SubString BID("bid");
    const String::SubString BIDID("bidid");
    const String::SubString CUR("cur");
    const String::SubString UNITS("units");
    const String::SubString ADID("adid");
    const String::SubString PRICE("price");
    const String::SubString ADM("adm");
    const String::SubString VIEW_NOTICES("view_notices");
    const String::SubString NURL("nurl");

    const String::SubString BANNER("banner");
    const String::SubString WIDTH("w");
    const String::SubString HEIGHT("h");
    const String::SubString SETSKIPTOKEN("setskiptoken");

    const String::SubString DSP_PARAMS("dsp_params");
    const String::SubString URL_PARAM3("url_param3");
    const String::SubString URL_PARAM4("url_param4");
    const String::SubString URL_PARAM17("url_param17");
    const String::SubString URL_PARAM18("url_param18");
  }

  namespace AppNexus
  {
    const String::SubString BID_RESPONSE("bid_response");
    const String::SubString RESPONSES("responses");
    const String::SubString AUCTION_ID_64("auction_id_64");
    const String::SubString MEMBER_ID("member_id");
    const String::SubString PRICE("price");
    const String::SubString CREATIVE_CODE("creative_code");
    const String::SubString CUSTOM_MACROS("custom_macros");
    const String::SubString CM_NAME("name");
    const String::SubString CM_VALUE("value");
  }

  // https://storage.googleapis.com/adx-rtb-dictionaries/buyer-declarable-creative-attributes.txt
  namespace Google
  {
    const ::google::protobuf::int32 CREATIVE_ATTR[] =
    {
      8, // CookieTargeting: IsCookieTargeted
      9, // UserInterestTargeting: IsUserInterestTargeted
    };

    // RichMediaCapabilityType: RichMediaCapabilitySSL
    const ::google::protobuf::int32 CREATIVE_SECURE = 47;

    // Expanding attributes
    //   ExpandingDirection: ExpandingNone
    const ::google::protobuf::int32 CREATIVE_EXPAND_NONE = 12;
    //   ExpandingDirection: ExpandingUp
    const ::google::protobuf::int32 CREATIVE_EXPAND_UP = 13;
    //   ExpandingDirection: ExpandingDown
    const ::google::protobuf::int32 CREATIVE_EXPAND_DOWN = 14;
    //   ExpandingDirection: ExpandingLeft
    const ::google::protobuf::int32 CREATIVE_EXPAND_LEFT = 15;
    //   ExpandingDirection: ExpandingRight
    const ::google::protobuf::int32 CREATIVE_EXPAND_RIGHT = 16;
    //   ExpandingDirection: ExpandingUpLeft
    const ::google::protobuf::int32 CREATIVE_EXPAND_UP_LEFT = 17;
    //   ExpandingDirection: ExpandingUpRight
    const ::google::protobuf::int32 CREATIVE_EXPAND_UP_RIGHT = 18;
    //   ExpandingDirection: ExpandingDownLeft
    const ::google::protobuf::int32 CREATIVE_EXPAND_DOWN_LEFT = 19;
    //   ExpandingDirection: ExpandingDownRight
    const ::google::protobuf::int32 CREATIVE_EXPAND_DOWN_RIGHT = 20;
    //   ExpandingDirection: ExpandingUpOrDown
    const ::google::protobuf::int32 CREATIVE_EXPAND_UP_OR_DOWN = 25;
    //   ExpandingDirection: ExpandingLeftOrRight
    const ::google::protobuf::int32 CREATIVE_EXPAND_LEFT_OR_RIGHT = 26;
    //   ExpandingDirection: ExpandingAnyDiagonal
    const ::google::protobuf::int32 CREATIVE_EXPAND_ANY_DIAGONAL = 27;

    const ::google::protobuf::int32 CREATIVE_EXPAND_MAP[] =
    {
      // expanding = 0
      CREATIVE_EXPAND_NONE,
      // expanding = 1 - CREATIVE_EXPANDING_LEFT
      CREATIVE_EXPAND_LEFT,
      // expanding = 2 - CREATIVE_EXPANDING_RIGHT
      CREATIVE_EXPAND_RIGHT,
      // expanding = 3 - CREATIVE_EXPANDING_RIGHT | CREATIVE_EXPANDING_LEFT
      CREATIVE_EXPAND_LEFT_OR_RIGHT,
      // expanding = 4 - CREATIVE_EXPANDING_UP
      CREATIVE_EXPAND_UP,
      // expanding = 5 - CREATIVE_EXPANDING_UP | CREATIVE_EXPANDING_LEFT
      CREATIVE_EXPAND_UP_LEFT,
      // expanding = 6 - CREATIVE_EXPANDING_UP | CREATIVE_EXPANDING_RIGHT
      CREATIVE_EXPAND_UP_RIGHT,
      // expanding = 7 - CREATIVE_EXPANDING_UP | CREATIVE_EXPANDING_RIGHT | CREATIVE_EXPANDING_LEFT
      -1,
      // expanding = 8 - CREATIVE_EXPANDING_DOWN
      CREATIVE_EXPAND_DOWN,
      // expanding = 9 - CREATIVE_EXPANDING_DOWN | CREATIVE_EXPANDING_LEFT
      CREATIVE_EXPAND_DOWN_LEFT,
      // expanding = 10 - CREATIVE_EXPANDING_DOWN | CREATIVE_EXPANDING_RIGHT
      CREATIVE_EXPAND_DOWN_RIGHT,
      // expanding = 11 - CREATIVE_EXPANDING_DOWN | CREATIVE_EXPANDING_RIGHT | CREATIVE_EXPANDING_LEFT
      -1,
      // expanding = 12 - CREATIVE_EXPANDING_DOWN | CREATIVE_EXPANDING_TOP
      CREATIVE_EXPAND_UP_OR_DOWN,
      // expanding = 13 - CREATIVE_EXPANDING_DOWN | CREATIVE_EXPANDING_TOP | CREATIVE_EXPANDING_LEFT
      -1,
      // expanding = 14 - CREATIVE_EXPANDING_DOWN | CREATIVE_EXPANDING_TOP | CREATIVE_EXPANDING_RIGHT
      -1,
      // expanding = 15 - ALL
      CREATIVE_EXPAND_ANY_DIAGONAL,
    };
  }
}

namespace
{
  const String::AsciiStringManip::Caseless WWW_PREFIX("www.");
}

namespace Config
{
  const char ENABLE[] = "BiddingFrontend_Enable";
  const char CONFIG_FILES[] = "BiddingFrontend_Config";
  const char CONFIG_FILE[] = "BiddingFrontend_ConfigFile";
}

namespace Aspect
{
  extern const char BIDDING_FRONTEND[] = "BiddingFrontend";
}

namespace Response
{
  namespace Header
  {
    const String::SubString CONTENT_TYPE("Content-Type");
    const String::SubString OPENRTB_VERSION("x-openrtb-version");
    const String::SubString OPENRTB_VERSION_VALUE("2.3");
  }

  namespace Type
  {
    const String::SubString TEXT_HTML("text/html");
    const String::SubString JSON("application/json");
    const String::SubString OCTET_STREAM("application/octet-stream");
  }
}



namespace AdServer
{
namespace Bidding
{
  namespace
  {

    template <typename T, size_t Size, typename Function>
    void for_range(
      const T(&range) [Size],
      Function fn)
    {
      std::for_each (range, range + Size, fn);
    }

    template <typename AddRepeatedFn>
    void categories_to_repeated(
      const AdServer::CampaignSvcs::CampaignManager::
        ExternalCreativeCategoryIdSeq& categories,
      AddRepeatedFn fn)
    {
      for(CORBA::ULong cat_i = 0; cat_i < categories.length(); ++cat_i)
      {
        int32_t cat_id = 0;
        if(String::StringManip::str_to_int(
             String::SubString(categories[cat_i].in()),
             cat_id))
        {
          fn(cat_id);
        }
      }
    }
 
    void 
    print_int_category_seq(
      AdServer::Commons::JsonObject& parent,
      const String::SubString& seq_name,
      const AdServer::CampaignSvcs::CampaignManager::
        ExternalCreativeCategoryIdSeq& categories)
    {
      AdServer::Commons::JsonObject array(parent.add_array(seq_name));
      for(CORBA::ULong cat_i = 0; cat_i < categories.length(); ++cat_i)
      {
        int cat_id = 0;
        if(String::StringManip::str_to_int(
             String::SubString(categories[cat_i].in()), cat_id))
        {
          array.add_number(cat_id);
        }
      }
    }

    const CampaignSvcs::RevenueDecimal MAX_CPM_CONF_MULTIPLIER(false, 100, 0);
    static const UserInfoSvcs::CampaignIdSeq EMPTY_CAMPAIGN_ID_SEQ;

    class TimeGuard
    {
    public:
      TimeGuard() throw ();

      Generics::Time consider() throw ();

    private:
      Generics::Timer timer_;
    };

    //
    // TimeGuard implementation
    //
    TimeGuard::TimeGuard() throw ()
    {
      timer_.start();
    }

    Generics::Time
    TimeGuard::consider() throw ()
    {
      timer_.stop();
      return timer_.elapsed_time();
    }

    struct GetId
    {
      template<typename ObjectType>
      unsigned int
      operator() (const ObjectType& obj) const throw()
      {
        return obj.id;
      }
    };

    AdServer::CampaignSvcs::CampaignManager::ChannelTriggerMatchInfo
    convert_channel_atom(
      const AdServer::ChannelSvcs::ChannelServerBase::ChannelAtom& atom)
      throw()
    {
      AdServer::CampaignSvcs::CampaignManager::ChannelTriggerMatchInfo out;
      out.channel_id = atom.id;
      out.channel_trigger_id = atom.trigger_channel_id;
      return out;
    }

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

    struct ContextualChannelConverter
    {
      const AdServer::UserInfoSvcs::UserInfoMatcher::ChannelWeight&
      operator()(const AdServer::UserInfoSvcs::UserInfoMatcher::ChannelWeight& ch_weight)
        const
      {
        return ch_weight;
      }

      AdServer::UserInfoSvcs::UserInfoMatcher::ChannelWeight
      operator()(const AdServer::ChannelSvcs::
        ChannelServerBase::ContentChannelAtom& contextual_channel)
        const
      {
        AdServer::UserInfoSvcs::UserInfoMatcher::ChannelWeight res;
        res.channel_id = contextual_channel.id;
        res.weight = contextual_channel.weight;
        return res;
      }
    };

    // Google add expanding attributes util
    void
    fill_google_expanding_attributes(
      Google::BidResponse_Ad* ad,
      const CORBA::Octet& expanding)
    {
      if (expanding > 15)
      {
        // Wrong expanding value from CM,
        // May be we need assert or error log here
        // No, we just see empty expanding
        return;
      }
      if (expanding == 0)
      {
        ad->add_attribute(
          Response::Google::CREATIVE_EXPAND_MAP[expanding]);
      }
      else
      {
        for (short i = 1; i <= expanding; ++i)
        {
          ::google::protobuf::int32 current =
            Response::Google::CREATIVE_EXPAND_MAP[i];
          
          if ((i & expanding) == i && current != -1)
          {
            ad->add_attribute(current);
          }
        }
      }
    }

    Commons::Interval<Generics::Time>
    construct_time_interval(const String::SubString& str)
      throw (eh::Exception)
    {
      const std::size_t pos = str.find('-');

      if (pos == String::SubString::NPOS)
      {
        Stream::Error ostr;
        ostr << "Separator not found in '" << str << "'";
        throw Frontend::Exception(ostr);
      }

      const Generics::Time min_val = Generics::Time(str.substr(0, pos), "%H:%M");
      const Generics::Time max_val = Generics::Time(str.substr(pos + 1), "%H:%M");
      return Commons::Interval<Generics::Time>(min_val, max_val);
    }
  }

  class Frontend::UpdateConfigTask: public Generics::TaskGoal
  {
  public:
    UpdateConfigTask(
      Frontend* bid_frontend,
      Generics::TaskRunner* task_runner)
      throw (eh::Exception)
      : Generics::TaskGoal(task_runner),
        bid_frontend_(bid_frontend)
    {}

    virtual void
    execute() throw ()
    {
      bid_frontend_->update_config_();
    }

  private:
    Frontend* bid_frontend_;
  };

  class Frontend::FlushStateTask: public Generics::TaskGoal
  {
  public:
    FlushStateTask(
      Frontend* bid_frontend,
      Generics::TaskRunner* task_runner)
      throw (eh::Exception)
      : Generics::TaskGoal(task_runner),
        bid_frontend_(bid_frontend)
    {}

    virtual void
    execute() throw ()
    {
      bid_frontend_->flush_state_();
    }

  private:
    Frontend* bid_frontend_;
  };

  //
  // Frontend::RequestTask
  //
  class Frontend::RequestTask:
    public Interrupter,
    public Generics::Task,
    public ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(Invalid, Exception);

    RequestTask(
      Frontend* bid_frontend,
      const FCGI::HttpRequest& http_request,
      const Generics::Time& start_processing_time)
      throw (Invalid)
      : to_interrupt(false),
        finished(false),
        bad_request(false),
        hostname(CORBA::string_dup("")),
        request_params(new RequestParamsHolder),
        bid_frontend_(bid_frontend),
        start_processing_time_(start_processing_time)
    {
      bid_frontend->request_info_filler_->fill(
        request_info_, http_request, start_processing_time);
    }

    virtual void
    execute() throw ()
    {
      execute_impl_();

      bid_frontend_->bid_task_count_ += -1;

      {
        Sync::ConditionalGuard guard(cond);
        if (!to_interrupt)
        {
          finished = true;
          cond.signal();
          return;
        }
      }
      // timed out message
      bid_frontend_->interrupt_("RequestTask::execute()", "final check", this);
    }

    virtual bool
    interrupt() const throw()
    {
      Sync::PosixGuard lock(cond);
      return to_interrupt;
    }

    virtual int
    write_response(FCGI::HttpResponse& response)
      throw() = 0;

    virtual int
    write_empty_response(
      FCGI::HttpResponse& response,
      bool bad_request)
      throw() = 0;

    virtual void
    print_request(std::ostream& out) const throw() = 0;

    const Generics::Time&
    start_processing_time() const throw()
    {
      return start_processing_time_;
    }

  public:
    mutable Sync::Condition cond;
    bool to_interrupt;
    bool finished;
    bool bad_request;
    /// The host performed last unbreakable operation.
    CORBA::String_var hostname;
    RequestParamsHolder_var request_params;

  protected:
    virtual ~RequestTask() throw() {}

    virtual void
    execute_impl_() throw() = 0;

  protected:
    Frontend* bid_frontend_;
    RequestInfo request_info_;
    const Generics::Time start_processing_time_;
  };

  //
  // Frontend::GoogleRequestTask
  //
  class Frontend::GoogleRequestTask: public RequestTask
  {
  public:
    GoogleRequestTask(
      Frontend* bid_frontend,
      const FCGI::HttpRequest& request,
      const Generics::Time& start_processing_time)
      throw (Invalid)
      : RequestTask(
          bid_frontend, request, start_processing_time)
    {
      static const char* FUN = "Frontend::GoogleRequestTask::GoogleRequestTask()";

      Stream::BinaryStreamReader request_reader(
        &request.get_input_stream());

      bid_request_.ParseFromIstream(&request_reader);

      if(bid_frontend->logger()->log_level() >= Logging::Logger::TRACE)
      {
        Generics::Time end_process_time = Generics::Time::get_time_of_day();
        Stream::Error ostr;
        ostr << FUN << ": Google task initialized after = " <<
          (end_process_time - start_processing_time);
        bid_frontend->logger()->log(ostr.str(),
          Logging::Logger::TRACE,
          Aspect::BIDDING_FRONTEND);
      }
    }

    virtual void
    execute_impl_() throw ()
    {
      bid_frontend_->process_google_request_(this, request_info_, bid_request_);
    }

    virtual int
    write_response(FCGI::HttpResponse& response)
      throw()
    {
      response.set_content_type(Response::Type::OCTET_STREAM);
      Stream::BinaryStreamWriter response_writer(
        &response.get_output_stream());
      bid_response.set_processing_time_ms(
        (Generics::Time::get_time_of_day() - start_processing_time_).microseconds() / 1000);
      bid_response.SerializeToOstream(&response_writer);
      response_writer.flush();
      return 0; //OK;
    }

    virtual int
    write_empty_response(
      FCGI::HttpResponse& response,
      bool /*bad_request*/)
      throw()
    {
      response.set_content_type(Response::Type::OCTET_STREAM);
      Stream::BinaryStreamWriter response_writer(
        &response.get_output_stream());
      Google::BidResponse empty_bid_response;
      empty_bid_response.set_processing_time_ms(
        (Generics::Time::get_time_of_day() - start_processing_time_).microseconds() / 1000);
      empty_bid_response.SerializeToOstream(&response_writer);
      response_writer.flush();
      return 0; //OK;
    }

    virtual void
    print_request(std::ostream& out) const throw()
    {
      out << bid_request_.DebugString();
    }

  public:
    Google::BidResponse bid_response;

  protected:
    virtual ~GoogleRequestTask() throw() {}

  private:
    Google::BidRequest bid_request_;
  };


  //
  // Frontend::OpenRtbRequestTask
  //
  class Frontend::OpenRtbRequestTask: public RequestTask
  {
  public:
    OpenRtbRequestTask(
      Frontend* bid_frontend,
      const FCGI::HttpRequest& request,
      const Generics::Time& start_processing_time)
      throw (Invalid)
      : RequestTask(
          bid_frontend, request, start_processing_time),
        uri(request.uri().str())
    {
      Stream::BinaryStreamReader request_reader(
        &request.get_input_stream());

      char buf[16 * 1024];

      while(!request_reader.eof() && !request_reader.bad())
      {
        request_reader.read(buf, sizeof(buf));
        bid_request_.append(buf, request_reader.gcount());
      }
    }

    virtual void
    execute_impl_() throw ()
    {
      bool bad_request_val = false;

      bid_frontend_->process_openrtb_request_(
        bad_request_val,
        this,
        request_info_,
        bid_request_.c_str());

      bad_request = bad_request_val;
    }

    virtual int
    write_response(FCGI::HttpResponse& response)
      throw()
    {
      response.set_content_type(Response::Type::JSON);
      response.add_header(
        Response::Header::OPENRTB_VERSION,
        Response::Header::OPENRTB_VERSION_VALUE);

      if(!bid_response.empty())
      {
        FCGI::OutputStream& output = response.get_output_stream();
        output.write(bid_response.data(), bid_response.size());
        //output.flush();
        return 0; //OK;
      }

      return 204; //HTTP_NO_CONTENT;
    }

    virtual int
    write_empty_response(
      FCGI::HttpResponse& response,
      bool bad_request)
      throw()
    {
      // no-bid is No content by OpenRTB 2.0 spec
      response.set_content_type(Response::Type::JSON);
      response.add_header(
        Response::Header::OPENRTB_VERSION,
        Response::Header::OPENRTB_VERSION_VALUE);
      return !bad_request ?
        204 /* HTTP_NO_CONTENT */ : 400 /* HTTP_BAD_REQUEST */;
    }

    virtual void
    print_request(std::ostream& out) const throw()
    {
      out << bid_request_;
    }

  public:
    std::string bid_response;
    const std::string uri;

  protected:
    virtual ~OpenRtbRequestTask() throw() {}

  private:
    std::string bid_request_;
  };

  //
  // Frontend::AppNexusRequestTask
  //
  class Frontend::AppNexusRequestTask: public RequestTask
  {
  public:
    AppNexusRequestTask(
      Frontend* bid_frontend,
      const FCGI::HttpRequest& request,
      const Generics::Time& start_processing_time)
      throw (Invalid)
      : RequestTask(
          bid_frontend, request, start_processing_time)
    {
      Stream::BinaryStreamReader request_reader(
        &request.get_input_stream());

      char buf[16 * 1024];

      while(!request_reader.eof() && !request_reader.bad())
      {
        request_reader.read(buf, sizeof(buf));
        bid_request_.append(buf, request_reader.gcount());
      }
    }

    virtual void
    execute_impl_() throw ()
    {
      bool bad_request_val = false;

      bid_frontend_->process_appnexus_request_(
        bad_request_val,
        this,
        request_info_,
        bid_request_.c_str());

      bad_request = bad_request_val;
    }

    virtual int
    write_response(FCGI::HttpResponse& response)
      throw()
    {
      if(!bid_response.empty())
      {
        response.set_content_type(Response::Type::JSON);
        FCGI::OutputStream& output = response.get_output_stream();
        output.write(bid_response.data(), bid_response.size());
        return 0; //OK;
      }

      return write_empty_response(response, false);
    }

    virtual int
    write_empty_response(
      FCGI::HttpResponse& response,
      bool bad_request)
      throw()
    {
      response.set_content_type(Response::Type::JSON);

      if (bad_request)
      {
        return 400; //HTTP_BAD_REQUEST;
      }

      const char no_bid[] =
        "{\n"
        "  \"bid_response\":{\n"
        "    \"no_bid\":true\n"
        "  }\n"
        "}";

      FCGI::OutputStream& output = response.get_output_stream();
      output.write(no_bid, sizeof(no_bid) - 1);
      return 0; //OK;
    }

    virtual void
    print_request(std::ostream& out) const throw()
    {
      out << bid_request_;
    }

  public:
    std::string bid_response;

  protected:
    virtual ~AppNexusRequestTask() throw() {}

  private:
    std::string bid_request_;
  };

  //
  // Frontend::InterruptPassbackTask
  //
  class Frontend::InterruptPassbackTask:
    public Generics::Task,
    public ReferenceCounting::AtomicImpl
  {
  public:
    InterruptPassbackTask(
      FrontendCommons::CampaignManagersPool<Exception>& campaign_managers,
      const RequestParamsHolder* request_params,
      const CORBA::String_var& hostname)
      throw (eh::Exception) :
      campaign_managers_(campaign_managers),
      request_params_var_(ReferenceCounting::add_ref(request_params)),
      hostname_(hostname)
    {}

    virtual void
    execute() throw ()
    {
      try
      {
        AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_var
          campaign_match_result;

        campaign_managers_.get_campaign_creative(
          *request_params_var_,
          hostname_,
          campaign_match_result.out());
      }
      catch(const eh::Exception&)
      {
        // Skip all CM exceptions
      }
    }

  protected:
    virtual ~InterruptPassbackTask() throw() {}

  private:
    FrontendCommons::CampaignManagersPool<Exception>& campaign_managers_;
    const ConstRequestParamsHolder_var request_params_var_;
    CORBA::String_var hostname_;
  };

  //
  // Frontend implementation
  //
  Frontend::Frontend(
    Configuration* frontend_config,
    Logging::Logger* logger,
    CommonModule* common_module,
    StatHolder* stats) throw (eh::Exception)
    : GroupLogger(
        Logging::Logger_var(
          new Logging::SeveritySelectorLogger(
            logger,
            0,
            frontend_config->get().BidFeConfiguration()->Logger().log_level())) ,
        "Bidding::Frontend", Aspect::BIDDING_FRONTEND, 0),
      frontend_config_(ReferenceCounting::add_ref(frontend_config)),
      common_module_(ReferenceCounting::add_ref(common_module)),
      colo_id_(0),
      campaign_managers_(this->logger(), Aspect::BIDDING_FRONTEND),
      stats_(ReferenceCounting::add_ref(stats)),
      bid_task_count_(0),
      reached_max_pending_tasks_(0)
  {}

  bool
  Frontend::will_handle(const String::SubString& uri) throw ()
  {
    std::string found_uri;
    bool result = false;

    if (!uri.empty())
    {
      result =
        FrontendCommons::find_uri(
          config_->GoogleUriList().Uri(), uri, found_uri) ||
        FrontendCommons::find_uri(
          config_->OpenRtbUriList().Uri(), uri, found_uri) ||
        FrontendCommons::find_uri(
          config_->AppNexusUriList().Uri(), uri, found_uri);
    }

    if(logger()->log_level() >= TraceLevel::MIDDLE)
    {
      Stream::Error ostr;
      ostr << "Bidding::Frontend::will_handle(" << uri <<
        "), service: '" << found_uri << "'";

      logger()->log(ostr.str());
    }

    return result;
  }

  void Frontend::parse_configs_() throw(Exception)
  {
    static const char* FUN = "Bidding::Frontend::parse_configs_()";

    /* load common configuration */

    try
    {
      typedef Configuration::FeConfig Config;
      const Config& fe_config = frontend_config_->get();

      if(!fe_config.CommonFeConfiguration().present())
      {
        throw Exception("CommonFeConfiguration isn't present");
      }

      common_config_.reset(
        new CommonFeConfiguration(*fe_config.CommonFeConfiguration()));

      colo_id_ = common_config_->colo_id();

      if(!fe_config.BidFeConfiguration().present())
      {
        throw Exception("BidFeConfiguration isn't present");
      }

      config_.reset(
        new BiddingFeConfiguration(*fe_config.BidFeConfiguration()));

      fill_account_traits_();
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << "': " <<
        e.what();
      throw Exception(ostr);
    }
  }

  void
  Frontend::init() throw(eh::Exception)
  {
    static const char* FUN = "Bidding::Frontend::init()";

    if(true) // module_used()
    {
      try
      {
        parse_configs_();

        corba_client_adapter_ = new CORBACommons::CorbaClientAdapter();

        planner_ = new Generics::Planner(callback());
        add_child_object(planner_);

        task_runner_ = new Generics::TaskRunner(
          callback(),
          config_->threads(),
          1024*1024 // stack size
          );
        add_child_object(task_runner_);

        control_task_runner_ = new Generics::TaskRunner(callback(), 3);
        add_child_object(control_task_runner_);

        // ADSC-10554
        // Interrupted requests queue
        passback_task_runner_ =
          new Generics::TaskRunner(
            callback(),
            config_->interrupted_threads(), // threads
            0, // stack_size
            config_->interrupted_max_pending_tasks() // max_pending_tasks
            );
        add_child_object(passback_task_runner_);

        Generics::Planner_var task_scheduler(new Generics::Planner(callback()));
        add_child_object(task_scheduler);

        // FlushLoggerTask
        Generics::Time flush_period(config_->flush_period().present() ? *config_->flush_period() : 10);
        Commons::make_goal_task(
          std::bind(
            &Commons::MessagePacker<CellsKey, MessageOut>::dump,
            group_logger(), Logging::Logger::ERROR, "", ""),
          task_runner_,
          task_scheduler,
          flush_period)->schedule(flush_period);

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

        user_info_client_ = new FrontendCommons::UserInfoClient(
          common_config_->UserInfoManagerControllerGroup(),
          corba_client_adapter_.in(),
          logger());
        add_child_object(user_info_client_);

        if(!common_config_->UserBindControllerGroup().empty())
        {
          user_bind_client_ = new FrontendCommons::UserBindClient(
            common_config_->UserBindControllerGroup(),
            corba_client_adapter_.in(),
            logger());
          add_child_object(user_bind_client_);
        }

        for(BiddingFeConfiguration::Source_sequence::const_iterator
              it = config_->Source().begin();
            it != config_->Source().end(); ++it)
        {
          SourceTraits source_traits;
          if (it->default_account_id().present())
          {
            source_traits.default_account_id = *(it->default_account_id());
          }
          source_traits.instantiate_type =
            AdServer::CampaignSvcs::AIT_BODY;
          // disable notice if notice_url defined
          source_traits.enabled_notice = it->enable_notice() && (
            !it->notice_url().present() || it->notice_url()->empty());
          source_traits.vast_instantiate_type =
            AdServer::CampaignSvcs::AIT_BODY;
          source_traits.vast_enabled_notice = it->vast_enable_notice();
          source_traits.ipw_extension = it->ipw_extension();
          source_traits.truncate_domain = it->truncate_domain();
          source_traits.fill_adid = it->fill_adid();
          if(it->seat().present())
          {
            source_traits.seat = *(it->seat());
          }

          if(it->appnexus_member_id().present())
          {
            source_traits.appnexus_member_id = *(it->appnexus_member_id());
          }

          if(it->request_type().present())
          {
            std::string type = *(it->request_type());
            if(type == "openrtb")
            {
              source_traits.request_type = AdServer::CampaignSvcs::AR_OPENRTB;
            }
            else if(type == "openrtb with click url")
            {
              source_traits.request_type = AdServer::CampaignSvcs::AR_OPENRTB_WITH_CLICKURL;
            }
            else if(type == "openx")
            {
              source_traits.request_type = AdServer::CampaignSvcs::AR_OPENX;
            }
            else if(type == "liverail")
            {
              source_traits.request_type = AdServer::CampaignSvcs::AR_LIVERAIL;
            }
            else if(type == "adriver")
            {
              source_traits.request_type = AdServer::CampaignSvcs::AR_ADRIVER;
            }
            else if(type == "yandex")
            {
              source_traits.request_type = AdServer::CampaignSvcs::AR_YANDEX;
            }
          }

          source_traits.instantiate_type = adapt_instantiate_type_(
            it->instantiate_type());
          source_traits.vast_instantiate_type = adapt_instantiate_type_(
            it->vast_instantiate_type());
          source_traits.native_ads_instantiate_type = adapt_native_ads_instantiate_type_(
            it->native_instantiate_type());
          if (it->native_impression_tracker_type().present())
          {
            source_traits.native_ads_impression_tracker_type =
              adapt_native_ads_impression_tracker_type_(
                *it->native_impression_tracker_type());
          }

          if(it->max_bid_time().present())
          {
            source_traits.max_bid_time = Generics::Time(*(it->max_bid_time()));
            (*source_traits.max_bid_time) /= 1000;
          }

          source_traits.skip_ext_category = it->skip_ext_category();
          if(it->notice_url().present())
          {
            source_traits.notice_url = *(it->notice_url());
          }

          sources_.insert(std::make_pair(it->id(), source_traits));
        }

        {
          const String::SubString intervalsBlacklist = config_->intervalsBlacklist();
          String::StringManip::SplitNL tokenizer(intervalsBlacklist);

          for (String::SubString interval; tokenizer.get_token(interval);)
          {
            blacklisted_time_intervals_.insert(construct_time_interval(interval.str()));
          }
        }

        RequestInfoFiller::ExternalUserIdSet skip_external_ids;

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

        request_info_filler_.reset(
          new RequestInfoFiller(
            logger(),
            common_config_->colo_id(),
            common_module_.in(),
            common_config_->GeoIP().present() ?
              common_config_->GeoIP()->path().c_str() : 0,
            "", //user_agent_filter_path.c_str()
            skip_external_ids,
            common_config_->ip_logging_enabled(),
            common_config_->ip_salt().c_str(),
            sources_,
            config_->enable_profile_referer(),
            account_traits_));

        if(config_->request_timeout().present())
        {
          request_timeout_ = Generics::Time(*(config_->request_timeout()));
          request_timeout_ /= 1000;
        }

        google::protobuf::SetLogHandler(&Frontend::protobuf_log_handler_);

        control_task_runner_->enqueue_task(
          Generics::Task_var(new UpdateConfigTask(this, control_task_runner_)));

        control_task_runner_->enqueue_task(
          Generics::Task_var(new FlushStateTask(this, control_task_runner_)));

        activate_object();
      }
      catch (const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": eh::Exception caught: " << ex.what();
        throw Exception(ostr);
      }

      logger()->log(String::SubString(
          "Bidding::Frontend::init(): frontend is running ..."),
        Logging::Logger::INFO,
        Aspect::BIDDING_FRONTEND);
    }
  }

  void
  Frontend::shutdown() throw ()
  {
    try
    {
      deactivate_object();
      wait_object();
      clear();

      Stream::Error ostr;
      ostr << "Bidding::Frontend::shutdown(): frontend terminated (pid = " <<
        ::getpid() << ").";

      logger()->log(ostr.str(),
        Logging::Logger::INFO,
        Aspect::BIDDING_FRONTEND);

      common_module_->shutdown();
    }
    catch(...)
    {}
  }

  Generics::Time
  Frontend::get_request_timeout_(const FCGI::HttpRequest& request) throw ()
  {
    const HTTP::ParamList& params = request.params();

    for (auto it = params.begin(); it != params.end(); ++it)
    {
      if (it->name == Request::Context::SOURCE_ID)
      {
        const auto source_it = sources_.find(it->value);

        if (source_it != sources_.end() && source_it->second.max_bid_time.present())
        {
          return *(source_it->second.max_bid_time);
        }
      }
    }

    return request_timeout_;
  }

  int
  Frontend::handle_request(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response) throw ()
  {
    static const char* FUN = "Bidding::Frontend::handle_request()";

    bool bad_request = false;
    bool finished = true;
    RequestTask_var request_task;
    Generics::Time start_process_time = Generics::Time::get_time_of_day();

    try
    {
      const Generics::Time expire_time(
        start_process_time + get_request_timeout_(request));

      std::string found_uri;

      if (
        FrontendCommons::find_uri(
        config_->GoogleUriList().Uri(), request.uri(), found_uri))
      {
        try
        {
          // Google request
          request_task = new GoogleRequestTask(
            this,
            request,
            start_process_time);
        }
        catch (RequestTask::Invalid& e)
        {
          Stream::Error ostr;
          ostr << FUN << ": RequestTask::Invalid caught: " << e.what();

          logger()->log(ostr.str(),
            Logging::Logger::INFO,
            Aspect::BIDDING_FRONTEND);

          return 400; //HTTP_BAD_REQUEST;
        }
      }          
      else if (FrontendCommons::find_uri(
        config_->AppNexusUriList().Uri(), request.uri(), found_uri))
      {
        request_task = new AppNexusRequestTask(
          this,
          request,
          start_process_time);
      }
      else
      {
        // OpenRTB request
        request_task = new OpenRtbRequestTask(
          this,
          request,
          start_process_time);
      }

      unsigned long cur_task_count = bid_task_count_.exchange_and_add(1) + 1;

      if(cur_task_count > config_->max_pending_tasks() + config_->threads())
      {
        bid_task_count_ += -1;

        MaxPendingSyncPolicy::WriteGuard lock(reached_max_pending_tasks_lock_);
        reached_max_pending_tasks_ = std::max(
          reached_max_pending_tasks_, cur_task_count);
        if (stats_.in())
        {
          stats_->add_skipped();
        }
      }
      else
      {
        task_runner_->enqueue_task(request_task);

        {
          Sync::ConditionalGuard guard(request_task->cond);

          do
          {
            if(!guard.timed_wait(&expire_time))
            {
              // timed out
              request_task->to_interrupt = true;
              finished = false;
              break;
            }
          }
          while(!request_task->finished);

          bad_request = request_task->bad_request;
        }

        if(finished && !bad_request)
        {
          int res = request_task->write_response(response);

          if(logger()->log_level() >= Logging::Logger::TRACE)
          {
            Generics::Time end_process_time = Generics::Time::get_time_of_day();
            Stream::Error ostr;
            ostr << FUN << ": full request processing time = " <<
              (end_process_time - start_process_time);
            logger()->log(ostr.str(),
              Logging::Logger::TRACE,
              Aspect::BIDDING_FRONTEND);
          }

          return res;
        }
      }
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << e.what();

      logger()->log(ostr.str(),
        Logging::Logger::EMERGENCY,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-109");
    }

    return request_task->write_empty_response(response, bad_request);
  }

  void
  Frontend::process_google_request_(
    GoogleRequestTask* request_task,
    RequestInfo& request_info,
    const Google::BidRequest& bid_request)
    throw()
  {
    static const char* FUN = "Bidding::Frontend::process_google_request_()";

    Google::BidResponse& bid_response = request_task->bid_response;

    if(check_interrupt_(FUN, "processing start", request_task))
    {
      return;
    }

    if(bid_request.has_is_ping() && bid_request.is_ping())
    {
      return;
    }

    AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_var
      campaign_match_result;
    AdServer::Commons::UserId user_id;
    GoogleAdSlotContextArray ad_slots_context;
    
    {
      AdServer::CampaignSvcs::CampaignManager::RequestParams&
        request_params(*request_task->request_params);
      
      std::string keywords;
     
      request_info_filler_->fill_by_google_request(
        request_params,
        request_info,
        keywords,
        ad_slots_context,
        bid_request);

      if (!process_bid_request_(
         FUN,
         campaign_match_result.out(),
         user_id,
         request_task,
         request_info,
         keywords))
      {
        return;
      }
    }

    assert(request_task->request_params.in());
    
    // ATTENTION!
    // Use only const reference to the RequestParam here.
    const AdServer::CampaignSvcs::CampaignManager::RequestParams&
      request_params(*request_task->request_params);
    
    // Fill response
    if(campaign_match_result)
    {
      if (!consider_campaign_selection_(
         user_id,
         request_info.current_time,
         *campaign_match_result,
         request_task->hostname))
      {
        return;
      }

      if(check_interrupt_(FUN, "campaign selection considering", request_task))
      {
        return;
      }

      assert(
        campaign_match_result->ad_slots.length() == ad_slots_context.size());

      for(CORBA::ULong ad_slot_i = 0;
          ad_slot_i < campaign_match_result->ad_slots.length();
          ++ad_slot_i)
      {
        const AdServer::CampaignSvcs::CampaignManager::
          AdSlotResult& ad_slot_result = campaign_match_result->ad_slots[ad_slot_i];

        const Google::BidRequest_AdSlot& adslot = bid_request.adslot(ad_slot_i);

        if(ad_slot_result.selected_creatives.length() > 0)
        {
          // campaigns selected
          CampaignSvcs::RevenueDecimal sum_pub_ecpm = CampaignSvcs::RevenueDecimal::ZERO;

          Google::BidResponse_Ad* ad = bid_response.add_ad();
          
          for(CORBA::ULong creative_i = 0;
              creative_i < ad_slot_result.selected_creatives.length();
              ++creative_i)
          {
            sum_pub_ecpm += CorbaAlgs::unpack_decimal<CampaignSvcs::RevenueDecimal>(
              ad_slot_result.selected_creatives[creative_i].pub_ecpm);

            ad->add_click_through_url(
              ad_slot_result.selected_creatives[creative_i].destination_url.in());
          }

          categories_to_repeated(
            ad_slot_result.external_content_categories,
            std::bind1st(
              std::mem_fun(&Google::BidResponse_Ad::add_category), ad));

          limit_max_cpm_(sum_pub_ecpm, request_params.publisher_account_ids);

          CampaignSvcs::ExtRevenueDecimal google_price = CampaignSvcs::ExtRevenueDecimal::mul(
            CampaignSvcs::ExtRevenueDecimal(sum_pub_ecpm.str()),
            CampaignSvcs::ExtRevenueDecimal(false, 10000, 0),
            Generics::DMR_ROUND);
          

          int64_t max_cpm_micros(google_price.integer<int64_t>());
          
          Google::BidResponse_Ad_AdSlot* r_adslot = ad->add_adslot();
          const GoogleAdSlotContext& ad_slot_context = ad_slots_context[ad_slot_i];
          
          if (ad_slot_context.direct_deal_id &&
            max_cpm_micros >= ad_slot_context.fixed_cpm_micros)
          {
            r_adslot->set_max_cpm_micros(ad_slot_context.fixed_cpm_micros);
            r_adslot->set_deal_id(ad_slot_context.direct_deal_id);
          }
          else
          {
            r_adslot->set_max_cpm_micros(max_cpm_micros);
          }
          r_adslot->set_id(adslot.id());

          
          if (ad_slot_context.width && ad_slot_context.height)
          {
            ad->set_width(ad_slot_context.width);
            ad->set_height(ad_slot_context.height);
          }

          // choose billing_id
          int64_t billing_id = 0;
          for(auto publisher_account_it = request_info.publisher_account_ids.begin();
            publisher_account_it != request_info.publisher_account_ids.end();
            ++publisher_account_it)
          {
            auto account_it = account_traits_.find(*publisher_account_it);
            if(account_it != account_traits_.end())
            {
              if(bid_request.has_video())
              {
                billing_id = account_it->second->video_billing_id;
              }
              else
              {
                billing_id = account_it->second->display_billing_id;
              }

              break;
            }
          }

          if(billing_id != 0 &&
            ad_slot_context.billing_ids.find(billing_id) != ad_slot_context.billing_ids.end())
          {
            r_adslot->set_billing_id(billing_id);
          }
          else if(!ad_slot_context.billing_ids.empty())
          {
            r_adslot->set_billing_id(*ad_slot_context.billing_ids.begin());            
          }

          // Fill attributes
          for_range(
            Response::Google::CREATIVE_ATTR,
            std::bind1st(
              std::mem_fun(&Google::BidResponse_Ad::add_attribute), ad));

          // Fill external attributes
          categories_to_repeated(
            ad_slot_result.external_visual_categories,
            std::bind1st(
              std::mem_fun(&Google::BidResponse_Ad::add_attribute), ad));
         
          {
            // buyer_creative_id
            const AdServer::CampaignSvcs::CampaignManager::
              CreativeSelectResult& creative = ad_slot_result.selected_creatives[0];

            std::ostringstream creative_version_ostr;

            creative_version_ostr << creative.creative_version_id.in() << "-" <<
              creative.creative_size << "S";

            ad->set_buyer_creative_id(creative_version_ostr.str());

            // Expanding attributes
            fill_google_expanding_attributes(
              ad,
              creative.expanding);

            // Secure attribute
            if (creative.https_safe_flag)
            {
              ad->add_attribute(Response::Google::CREATIVE_SECURE);
            }
          }

          // Video
          if(request_params.ad_instantiate_type == AdServer::CampaignSvcs::AIT_VIDEO_URL &&
            ad_slot_result.creative_url[0])
          {
            ad->set_video_url(ad_slot_result.creative_url);
          }
          // Banner
          else
          {
            ad->set_html_snippet(ad_slot_result.creative_body.in());
          }
        }
      }
    }

    // Fill SNMP stats
    Generics::Time finish_processing_time = Generics::Time::get_time_of_day();
    Generics::Time processing_time = finish_processing_time - request_task->start_processing_time();

    if (stats_.in())
    {
      stats_->flush(request_params, campaign_match_result, processing_time);
    }
  }


  void
  Frontend::process_openrtb_request_(
    bool& bad_request,
    OpenRtbRequestTask* request_task,
    RequestInfo& request_info,
    const char* bid_request)
    throw()
  {
    if(logger()->log_level() >= TraceLevel::MIDDLE)
    {
      logger()->log(
        String::SubString("Bidding::Frontend::process_openrtb_request_(): entered"),
        TraceLevel::MIDDLE,
        Aspect::BIDDING_FRONTEND);
    }

    static const char* FUN = "Bidding::Frontend::process_request_()";

    if(check_interrupt_(FUN, "processing start", request_task))
    {
      return;
    }

    AdServer::Commons::UserId user_id;

    AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_var
      campaign_match_result;
    JsonProcessingContext context;
    {
      AdServer::CampaignSvcs::CampaignManager::RequestParams&
        request_params(*request_task->request_params);
      std::string keywords;

      try
      {
        request_info_filler_->fill_by_openrtb_request(
          request_params,
          request_info,
          keywords,
          context,
          bid_request);
      }
      catch(const InvalidParamException& ex)
      {
        bad_request = true;
        
        Stream::Error ostr;
        ostr << FUN << ": bad request, " << ex.what() <<
          ", request: '" << bid_request << "'" << ", uri: '" <<
          request_task->uri << "'";
        
        logger()->log(
          ostr.str(),
          Logging::Logger::ERROR,
          Aspect::BIDDING_FRONTEND,
          "ADS-IMPL-7601");
        
        return;
      }

      if(check_interrupt_(FUN, "request parsing", request_task))
      {
        return;
      }
      
      if (!process_bid_request_(
        FUN,
        campaign_match_result.out(),
        user_id,
        request_task,
        request_info,
        keywords))
      {
        return;
      }
    }

    assert(request_task->request_params.in());

    // ATTENTION!
    // Use only const reference to the RequestParam here.
    const AdServer::CampaignSvcs::CampaignManager::RequestParams&
      request_params(*request_task->request_params);
      
    if(campaign_match_result)
    {
      if (!consider_campaign_selection_(
        user_id,
        request_info.current_time,
        *campaign_match_result,
        request_task->hostname))
      {
        return;
      }

      if(check_interrupt_(FUN, "campaign selection considering", request_task))
      {
        return;
      }

      // check that any campaign selected (in any slot)
      bool ad_selected = false;

      for(CORBA::ULong ad_slot_i = 0;
          ad_slot_i < campaign_match_result->ad_slots.length();
          ++ad_slot_i)
      {
        const AdServer::CampaignSvcs::CampaignManager::
          AdSlotResult& ad_slot_result = campaign_match_result->ad_slots[ad_slot_i];

        if(ad_slot_result.selected_creatives.length() > 0)
        {
          ad_selected = true;
          break;
        }
      }

      if(ad_selected)
      {
        std::ostringstream response_ostr;
        if(request_params.common_info.request_type !=
           AdServer::CampaignSvcs::AR_YANDEX)
        {
          // standard OpenRTB, OpenX
          fill_openrtb_response_(
            response_ostr,
            request_info,
            request_params,
            context,
            *campaign_match_result);
        }
        else
        {
          fill_yandex_response_(
            response_ostr,
            request_info,
            request_params,
            context,
            *campaign_match_result);
        }
        request_task->bid_response = response_ostr.str();
      } // if(ad_selected)
    } // if(campaign_match_result)


    Generics::Time processing_time =
      Generics::Time::get_time_of_day() - request_task->start_processing_time();
    if (stats_.in())
    {
      stats_->flush(request_params, campaign_match_result, processing_time);
    }
  }

  void
  Frontend::process_appnexus_request_(
    bool& bad_request,
    AppNexusRequestTask* request_task,
    RequestInfo& request_info,
    const char* bid_request)
    throw()
  {
    if(logger()->log_level() >= TraceLevel::MIDDLE)
    {
      logger()->log(
        String::SubString("Bidding::Frontend::process_appnexus_request_(): entered"),
        TraceLevel::MIDDLE,
        Aspect::BIDDING_FRONTEND);
    }

    static const char* FUN = "Bidding::Frontend::process_appnexus_request_()";

    if(check_interrupt_(FUN, "processing start", request_task))
    {
      return;
    }

    AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_var
      campaign_match_result;
    AdServer::Commons::UserId user_id;
    JsonProcessingContext context;
    {
      // parse request
      AdServer::CampaignSvcs::CampaignManager::RequestParams&
        request_params(*request_task->request_params);
      std::string keywords;

      try
      {
        request_info_filler_->fill_by_appnexus_request(
          request_params,
          request_info,
          keywords,
          context,
          bid_request);
      }
      catch(const InvalidParamException& ex)
      {
        bad_request = true;
        
        Stream::Error ostr;
        ostr << FUN << ": bad request, " << ex.what() <<
          ", request: '" << bid_request << "'";

        logger()->log(
          ostr.str(),
          Logging::Logger::ERROR,
          Aspect::BIDDING_FRONTEND,
          "ADS-IMPL-7601");
        
        return;
      }

      if(check_interrupt_(FUN, "request parsing", request_task))
      {
        return;
      }

      if (!process_bid_request_(
        FUN,
        campaign_match_result.out(),
        user_id,
        request_task,
        request_info,
        keywords))
      {
        return;
      }
    }

    assert(request_task->request_params.in());

    // ATTENTION!
    // Use only const reference to the RequestParam here.
    const AdServer::CampaignSvcs::CampaignManager::RequestParams&
      request_params(*request_task->request_params);
      
    if(campaign_match_result)
    {
      if (!consider_campaign_selection_(
        user_id,
        request_info.current_time,
        *campaign_match_result,
        request_task->hostname))
      {
        return;
      }

      if(check_interrupt_(FUN, "campaign selection considering", request_task))
      {
        return;
      }

      // check that any campaign selected (in any slot)
      bool ad_selected = false;

      for(CORBA::ULong ad_slot_i = 0;
          ad_slot_i < campaign_match_result->ad_slots.length();
          ++ad_slot_i)
      {
        const AdServer::CampaignSvcs::CampaignManager::
          AdSlotResult& ad_slot_result = campaign_match_result->ad_slots[ad_slot_i];

        if(ad_slot_result.selected_creatives.length() > 0)
        {
          ad_selected = true;
          break;
        }
      }

      if(ad_selected)
      {
        std::ostringstream response_ostr;
        try
        {
          AdServer::Commons::JsonFormatter root_response(response_ostr);
          AdServer::Commons::JsonObject bid_response(
            root_response.add_object(Response::AppNexus::BID_RESPONSE));
          AdServer::Commons::JsonObject responses(
            bid_response.add_array(Response::AppNexus::RESPONSES));
          // std::string escaped_request_id =
          //  String::StringManip::json_escape(context.request_id);

          assert(campaign_match_result->ad_slots.length() ==
            context.ad_slots.size());
          JsonAdSlotProcessingContextList::const_iterator slot_it =
            context.ad_slots.begin();

          for(CORBA::ULong ad_slot_i = 0;
              ad_slot_i < campaign_match_result->ad_slots.length();
              ++ad_slot_i, ++slot_it)
          {
            const AdServer::CampaignSvcs::CampaignManager::
              AdSlotResult& ad_slot_result = campaign_match_result->ad_slots[ad_slot_i];

            if(ad_slot_result.selected_creatives.length() > 0)
            {

              AdServer::Commons::JsonObject bid_response(
                responses.add_object());
              // campaigns selected
              CampaignSvcs::RevenueDecimal sum_pub_ecpm = CampaignSvcs::RevenueDecimal::ZERO;
              const AdServer::CampaignSvcs::CampaignManager::CreativeSelectResult& creative =
                ad_slot_result.selected_creatives[0];

              sum_pub_ecpm += CorbaAlgs::unpack_decimal<CampaignSvcs::RevenueDecimal>(
                creative.pub_ecpm);

              limit_max_cpm_(sum_pub_ecpm, request_params.publisher_account_ids);
              // result price in RUB/1000, ecpm is in 0.01/1000
              CampaignSvcs::RevenueDecimal appnexus_price = CampaignSvcs::RevenueDecimal::div(
                sum_pub_ecpm,
                CampaignSvcs::RevenueDecimal(false, 100, 0));

              std::string escaped_creative_body =
                String::StringManip::json_escape(
                  String::SubString(ad_slot_result.creative_body));

              unsigned long member_id = 0;
              if(request_info.appnexus_member_id.present())
              {
                member_id = *request_info.appnexus_member_id;
              }
              else
              {
                member_id = (
                  !context.member_ids.empty() ? *context.member_ids.begin() : 0);
              }

              bid_response.add_number(Response::AppNexus::AUCTION_ID_64, slot_it->id);
              bid_response.add_number(Response::AppNexus::MEMBER_ID, member_id);
              bid_response.add_number(Response::AppNexus::PRICE, appnexus_price);
              bid_response.add_as_string(Response::AppNexus::CREATIVE_CODE, creative.creative_id);

              AdServer::Commons::JsonObject custom_macros(bid_response.add_array(
                Response::AppNexus::CUSTOM_MACROS));
              AdServer::Commons::JsonObject custom_macros_elem(custom_macros.add_object());
              custom_macros_elem.add_string(
                Response::AppNexus::CM_NAME, String::SubString("EXT_DATA"));
              custom_macros_elem.add_escaped_string(
                Response::AppNexus::CM_VALUE, String::SubString(ad_slot_result.creative_body));
            } // if(ad_slot_result.selected_creatives.length() > 0)
          } // for(CORBA::ULong ad_slot_i = 0, ...
        } // try JsonObject root
        catch(const AdServer::Commons::JsonObject::Exception& ex)
        {
          Stream::Error ostr;
          ostr << FUN << " Error on formatting Json response: '" << ex.what() << "'";
          logger()->log(ostr.str(), Logging::Logger::EMERGENCY, Aspect::BIDDING_FRONTEND);
        }
        request_task->bid_response = response_ostr.str();
      } // if(ad_selected)
    } // if(campaign_match_result)
  }

  void
  Frontend::resolve_user_id_(
    AdServer::Commons::UserId& match_user_id,
    AdServer::CampaignSvcs::CampaignManager::CommonAdRequestInfo& common_info,
    RequestInfo& request_info)
    throw()
  {
    static const char* FUN = "Bidding::Frontend::resolve_user_id_()";

    Generics::Time start_process_time;

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      start_process_time = Generics::Time::get_time_of_day();
    }

    if(request_info.filter_request)
    {
      common_info.user_status = static_cast<CORBA::ULong>(
        AdServer::CampaignSvcs::US_FOREIGN);
    }
    else if(common_info.signed_user_id[0] &&
      common_info.user_status != AdServer::CampaignSvcs::US_PROBE)
    {
      common_info.user_status = static_cast<CORBA::ULong>(
        AdServer::CampaignSvcs::US_OPTIN);
      match_user_id = CorbaAlgs::unpack_user_id(common_info.user_id);
    }
    else if(!request_info.advertising_id.empty() ||
      !request_info.idfa.empty() ||
      common_info.external_user_id[0] ||
      !request_info.external_user_ids.empty())
    {
      if(user_bind_client_)
      {
        // external user ids (first will be used as base)
        std::vector<std::string> external_user_ids;
        external_user_ids.reserve(request_info.external_user_ids.size() + 2);

        if(!request_info.idfa.empty())
        {
          std::string resolve_idfa = request_info.idfa;
          String::AsciiStringManip::to_lower(resolve_idfa);
          external_user_ids.push_back(std::string("ifa/") + resolve_idfa);
        }

        if(!request_info.advertising_id.empty())
        {
          std::string resolve_idfa = request_info.advertising_id;
          String::AsciiStringManip::to_lower(resolve_idfa);
          external_user_ids.push_back(std::string("ifa/") + resolve_idfa);
        }

        if(common_info.external_user_id[0])
        {
          external_user_ids.push_back(common_info.external_user_id.in());
        }

        external_user_ids.insert(
          external_user_ids.end(),
          request_info.external_user_ids.begin(),
          request_info.external_user_ids.end());

        assert(!external_user_ids.empty());

        try
        {
          AdServer::UserInfoSvcs::UserBindMapper_var user_bind_mapper =
            user_bind_client_->user_bind_mapper();

          auto base_ext_user_id_it = external_user_ids.begin();

          AdServer::Commons::UserId local_match_user_id;
          AdServer::UserInfoSvcs::UserBindServer::GetUserResponseInfo_var user_bind_info;

          bool blacklisted = false;
          bool min_age_reached = false;

          for(auto ext_user_id_it = external_user_ids.begin();
            ext_user_id_it != external_user_ids.end();
            ++ext_user_id_it)
          {
            AdServer::UserInfoSvcs::UserBindMapper::GetUserRequestInfo get_request_info;
            get_request_info.id << *ext_user_id_it;
            get_request_info.timestamp = CorbaAlgs::pack_time(request_info.current_time);
            get_request_info.silent = false;
            get_request_info.generate_user_id = false;
            get_request_info.for_set_cookie = false;
            get_request_info.create_timestamp = CorbaAlgs::pack_time(request_info.user_create_time);
            // get_request_info.current_user_id is null

            user_bind_info = user_bind_mapper->get_user_id(get_request_info);

            min_age_reached |= user_bind_info->min_age_reached;
            local_match_user_id = CorbaAlgs::unpack_user_id(user_bind_info->user_id);

            blacklisted |= common_module_->user_id_controller()->null_blacklisted(match_user_id);

            if(!local_match_user_id.is_null())
            {
              common_info.external_user_id << *ext_user_id_it;
              base_ext_user_id_it = ext_user_id_it;
              break;
            }
            else if(common_info.external_user_id[0] == 0)
            {
              common_info.external_user_id << *ext_user_id_it;
            }
          }

          match_user_id = local_match_user_id;

          common_module_->user_id_controller()->null_blacklisted(match_user_id);

          if (!match_user_id.is_null())
          { //(external_user_id is not found and user_id is not null,
            //  in other words: min_age_reached=true && bind_on_min_age=true -> user_id generated)
            //or
            //(external_user_id is found and user_id is not null)
            // link other external user ids to base
            for(auto ext_user_id_it = external_user_ids.begin();
              ext_user_id_it != external_user_ids.end();
              ++ext_user_id_it)
            {
              if(ext_user_id_it != base_ext_user_id_it)
              {
                AdServer::UserInfoSvcs::UserBindServer::AddUserRequestInfo add_user_request;
                add_user_request.id << *ext_user_id_it;
                add_user_request.timestamp = CorbaAlgs::pack_time(request_info.current_time);
                add_user_request.user_id = CorbaAlgs::pack_user_id(match_user_id);
                AdServer::UserInfoSvcs::UserBindServer::AddUserResponseInfo_var
                  prev_user_bind_info =
                    user_bind_mapper->add_user_id(add_user_request);

                (void)prev_user_bind_info;
              }
            }

            common_info.user_status = static_cast<CORBA::ULong>(
              AdServer::CampaignSvcs::US_OPTIN);

            common_info.signed_user_id <<
              common_module_->user_id_controller()->sign(match_user_id).str();
          }
          else if (blacklisted)
          {
            common_info.user_status = static_cast<CORBA::ULong>(
              AdServer::CampaignSvcs::US_UNDEFINED);
          }
          else if (user_bind_info->user_found)
          { //external_user_id is found and user_id is null
            common_info.user_status = static_cast<CORBA::ULong>(
              AdServer::CampaignSvcs::US_OPTOUT);
          }
          //external_user_id is not found and user_id is null
          else if(min_age_reached)
          {
            // uid generation on RTB requests disabled (bind_on_min_age=false)
            common_info.user_status = static_cast<CORBA::ULong>(
              AdServer::CampaignSvcs::US_UNDEFINED);
          }
          else
          {
            common_info.user_status = static_cast<CORBA::ULong>(
              AdServer::CampaignSvcs::US_EXTERNALPROBE);
          }
        }
        catch(const AdServer::UserInfoSvcs::UserBindMapper::NotReady& )
        {
          Stream::Error ostr;
            ostr << FUN << ": caught UserBindMapper::NotReady";

          logger()->log(ostr.str(),
            Logging::Logger::WARNING,
            Aspect::BIDDING_FRONTEND,
            "ADS-IMPL-10681");
        }
        catch(const AdServer::UserInfoSvcs::UserBindMapper::ChunkNotFound& )
        {
          Stream::Error ostr;
            ostr << FUN << ": caught UserBindMapper::ChunkNotFound";

          logger()->log(ostr.str(),
            Logging::Logger::ERROR,
            Aspect::BIDDING_FRONTEND,
            "ADS-IMPL-10681");
        }
        catch(const AdServer::UserInfoSvcs::UserBindMapper::ImplementationException& ex)
        {
          Stream::Error ostr;
            ostr << FUN << ": caught UserBindMapper::ImplementationException: " <<
            ex.description;

          logger()->log(ostr.str(),
            Logging::Logger::ERROR,
            Aspect::BIDDING_FRONTEND,
            "ADS-IMPL-10681");
        }
        catch(const CORBA::SystemException& e)
        {
          Stream::Error ostr;
          ostr << FUN << ": caught CORBA::SystemException: " << e;
          logger()->log(ostr.str(),
            Logging::Logger::ERROR,
            Aspect::BIDDING_FRONTEND,
            "ADS-ICON-7800");
        }
      }
    }
    else if(common_info.user_status != AdServer::CampaignSvcs::US_PROBE)
    {
      common_info.user_status = static_cast<CORBA::ULong>(
        AdServer::CampaignSvcs::US_NOEXTERNALID);
    }

    if(common_info.user_status != AdServer::CampaignSvcs::US_OPTIN)
    {
      // US_FOREIGN already filtered - filter US_NOEXTERNALID fully
      // and disable ccg keywords loading for non opt in (US_EXTERNALPROBE actually)
      // if colocation configured to passback for non opt-in
      ExtConfig_var ext_config = get_ext_config_();

      if(ext_config.in())
      {
        ExtConfig::ColocationMap::const_iterator colo_it =
          ext_config->colocations.find(common_info.colo_id);

        if(colo_it != ext_config->colocations.end() &&
           (colo_it->second.flags == CampaignSvcs::CS_NONE ||
            colo_it->second.flags == CampaignSvcs::CS_ONLY_OPTIN))
        {
          if(colo_it->second.flags == CampaignSvcs::CS_NONE ||
            common_info.user_status == AdServer::CampaignSvcs::US_NOEXTERNALID)
          {
            request_info.filter_request = true;
          }
          request_info.skip_ccg_keywords = true;
        }
      }
    }

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      Generics::Time end_process_time = Generics::Time::get_time_of_day();
      Stream::Error ostr;
      ostr << FUN << ": user is resolving time = " <<
        (end_process_time - start_process_time);
      logger()->log(ostr.str(),
        Logging::Logger::TRACE,
        Aspect::BIDDING_FRONTEND);
    }

    if (config_->trace_mapping() &&
        logger()->log_level() >= Logging::Logger::DEBUG)
    {
      Stream::Error ostr;
      ostr << FUN << ": SSP user mapping: " << match_user_id.to_string() <<
        " <-> (" << common_info.external_user_id << ", " <<
        request_info.source_id << ')';
      logger()->log(ostr.str(),
        Logging::Logger::DEBUG,
        Aspect::BIDDING_FRONTEND);
    }
  }

  void
  Frontend::trigger_match_(
    AdServer::ChannelSvcs::ChannelServerBase::MatchResult_out trigger_matched_channels,
    AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
    const RequestInfo& request_info,
    const AdServer::Commons::UserId& user_id,
    CORBA::String_var& /*hostname*/,
    const char* keywords)
    throw()
  {
    static const char* FUN = "Bidding::Frontend::trigger_match_()";

    if(!request_info.filter_request)
    {
      try
      {
        AdServer::ChannelSvcs::ChannelServerBase::MatchQuery query;
        query.non_strict_word_match = false;
        query.non_strict_url_match = false;
        query.return_negative = false;
        query.simplify_page = true;
        query.fill_content = true;
        query.statuses[0] = 'A';
        query.statuses[1] = '\0';
        query.first_url = request_params.common_info.referer;
        try
        {
          std::string ref_words;
          FrontendCommons::extract_url_keywords(
              ref_words,
              String::SubString(request_params.common_info.referer),
              common_module_->segmentor());
          
          if (!ref_words.empty())
          {
            query.first_url_words << ref_words;
          }
        }
        catch (const eh::Exception& e)
        {
          Stream::Error ostr;
          
          ostr << FUN << " url keywords extracting error: " << e.what();
          
          logger()->log(ostr.str(),
            Logging::Logger::TRACE,
            Aspect::BIDDING_FRONTEND);
        }

        // check multiline
        {
          std::ostringstream urls_ostr;
          std::ostringstream urls_words_ostr;
          bool url_word_added = false;
          for(CORBA::ULong i = 0; i < request_params.common_info.urls.length(); ++i)
          {
            urls_ostr << (i != 0 ? "\n" : "") <<
              request_params.common_info.urls[i];

            std::string url_words_res;
            FrontendCommons::extract_url_keywords(
              url_words_res,
              String::SubString(request_params.common_info.urls[i]),
              common_module_->segmentor());

            if (!url_words_res.empty())
            {
              urls_words_ostr << (url_word_added ? "\n" : "") << url_words_res;
              url_word_added = true;
            }
          }
          query.urls << urls_ostr.str();
          const std::string& tmp = urls_words_ostr.str();
          if (!tmp.empty())
          {
            query.urls_words << tmp;
          }
        }

        if(keywords)
        {
          query.pwords = keywords;
        }
        query.swords << request_info.search_words;
        query.uid = CorbaAlgs::pack_user_id(user_id);

        channel_servers_->match(query, trigger_matched_channels);

        request_params.trigger_match_result.pkw_channels.length(
          trigger_matched_channels->matched_channels.page_channels.length());
        std::transform(
          trigger_matched_channels->matched_channels.page_channels.get_buffer(),
          trigger_matched_channels->matched_channels.page_channels.get_buffer() +
            trigger_matched_channels->matched_channels.page_channels.length(),
          request_params.trigger_match_result.pkw_channels.get_buffer(),
          convert_channel_atom);
        request_params.trigger_match_result.url_channels.length(
          trigger_matched_channels->matched_channels.url_channels.length());
        std::transform(
          trigger_matched_channels->matched_channels.url_channels.get_buffer(),
          trigger_matched_channels->matched_channels.url_channels.get_buffer() +
            trigger_matched_channels->matched_channels.url_channels.length(),
          request_params.trigger_match_result.url_channels.get_buffer(),
          convert_channel_atom);
        request_params.trigger_match_result.ukw_channels.length(
          trigger_matched_channels->matched_channels.url_keyword_channels.length());
        std::transform(
          trigger_matched_channels->matched_channels.url_keyword_channels.get_buffer(),
          trigger_matched_channels->matched_channels.url_keyword_channels.get_buffer() +
            trigger_matched_channels->matched_channels.url_keyword_channels.length(),
          request_params.trigger_match_result.ukw_channels.get_buffer(),
          convert_channel_atom);
        request_params.trigger_match_result.skw_channels.length(
          trigger_matched_channels->matched_channels.search_channels.length());
        std::transform(
          trigger_matched_channels->matched_channels.search_channels.get_buffer(),
          trigger_matched_channels->matched_channels.search_channels.get_buffer() +
            trigger_matched_channels->matched_channels.search_channels.length(),
          request_params.trigger_match_result.skw_channels.get_buffer(),
          convert_channel_atom);
        CorbaAlgs::copy_sequence(
          trigger_matched_channels->matched_channels.uid_channels,
          request_params.trigger_match_result.uid_channels);

        if(request_params.common_info.user_status ==
           static_cast<CORBA::ULong>(AdServer::CampaignSvcs::US_OPTIN) &&
           (trigger_matched_channels->no_track ||
            trigger_matched_channels->no_adv))
        {
          request_params.common_info.user_status = static_cast<CORBA::ULong>(
            AdServer::CampaignSvcs::US_BLACKLISTED);
        }
      }
      catch(const FrontendCommons::ChannelServerSessionPool::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN <<
          ": caught ChannelServerSessionPool::Exception: " <<
          ex.what();
        logger()->log(ostr.str(),
          Logging::Logger::EMERGENCY,
          Aspect::BIDDING_FRONTEND,
          "ADS-IMPL-117");
      }
    }
  }

  void
  Frontend::history_match_(
    AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult_out history_match_result,
    AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
    const RequestInfo& request_info,
    const AdServer::ChannelSvcs::ChannelServerBase::MatchResult* trigger_match_result,
    const AdServer::Commons::UserId& user_id,
    const Generics::Time& time,
    CORBA::String_var& /*hostname*/)
    throw()
  {
    static const char* FUN = "Bidding::Frontend::history_match_()";

    typedef std::set<ChannelMatch> ChannelMatchSet;

    Generics::Time start_process_time;

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      start_process_time = Generics::Time::get_time_of_day();
    }

    request_params.profiling_available = false;

    if(!user_id.is_null())
    {
      AdServer::UserInfoSvcs::UserInfoMatcher_var
        uim_session = user_info_client_->user_info_session();

      if(uim_session.in())
      {
        try
        {
          AdServer::UserInfoSvcs::UserInfoMatcher::MatchParams match_params;
          match_params.use_empty_profile = false;
          match_params.silent_match = false;
          match_params.no_match = trigger_match_result && trigger_match_result->no_track;
          match_params.no_result = false;
          match_params.ret_freq_caps = true;
          match_params.provide_channel_count = false;
          match_params.provide_persistent_channels = false;
          match_params.change_last_request = true;
          match_params.publishers_optin_timeout = CorbaAlgs::pack_time(
            time - Generics::Time::ONE_DAY * 15);
          match_params.cohort << (
            !request_info.idfa.empty() ?
            request_info.idfa : request_info.advertising_id);

          CorbaAlgs::copy_sequence(
            request_params.context_info.platform_ids,
            match_params.persistent_channel_ids);

          AdServer::UserInfoSvcs::UserInfo user_info;
          user_info.user_id = CorbaAlgs::pack_user_id(user_id);
          user_info.huser_id = CorbaAlgs::pack_user_id(AdServer::Commons::UserId());
          user_info.last_colo_id = colo_id_;
          user_info.request_colo_id = colo_id_;
          user_info.current_colo_id = -1;
          user_info.temporary = false;
          user_info.time = time.tv_sec;

          if(trigger_match_result && !trigger_match_result->no_track)
          {
            ChannelMatchSet page_channels;
            ChannelMatchSet url_channels;
            ChannelMatchSet search_channels;
            ChannelMatchSet url_keyword_channels;

            std::transform(
              trigger_match_result->matched_channels.page_channels.get_buffer(),
              trigger_match_result->matched_channels.page_channels.get_buffer() +
              trigger_match_result->matched_channels.page_channels.length(),
              std::inserter(page_channels, page_channels.end()),
              GetChannelTriggerId());

            std::transform(
              trigger_match_result->matched_channels.url_channels.get_buffer(),
              trigger_match_result->matched_channels.url_channels.get_buffer() +
              trigger_match_result->matched_channels.url_channels.length(),
              std::inserter(url_channels, url_channels.end()),
              GetChannelTriggerId());

            std::transform(
              trigger_match_result->matched_channels.search_channels.get_buffer(),
              trigger_match_result->matched_channels.search_channels.get_buffer() +
              trigger_match_result->matched_channels.search_channels.length(),
              std::inserter(search_channels, search_channels.end()),
              GetChannelTriggerId());

            std::transform(
              trigger_match_result->matched_channels.url_keyword_channels.get_buffer(),
              trigger_match_result->matched_channels.url_keyword_channels.get_buffer() +
              trigger_match_result->matched_channels.url_keyword_channels.length(),
              std::inserter(url_keyword_channels, url_keyword_channels.end()),
              GetChannelTriggerId());

            match_params.page_channel_ids.length(page_channels.size());
            CORBA::ULong i = 0;
            for (ChannelMatchSet::const_iterator it = page_channels.begin();
                 it != page_channels.end(); ++it, ++i)
            {
              match_params.page_channel_ids[i].channel_id = it->channel_id;
              match_params.page_channel_ids[i].channel_trigger_id = it->channel_trigger_id;
            }

            match_params.url_channel_ids.length(url_channels.size());
            i = 0;
            for (ChannelMatchSet::const_iterator it = url_channels.begin();
                 it != url_channels.end(); ++it, ++i)
            {
              match_params.url_channel_ids[i].channel_id = it->channel_id;
              match_params.url_channel_ids[i].channel_trigger_id = it->channel_trigger_id;
            }

            match_params.search_channel_ids.length(search_channels.size());
            i = 0;
            for (ChannelMatchSet::const_iterator it = search_channels.begin();
                 it != search_channels.end(); ++it, ++i)
            {
              match_params.search_channel_ids[i].channel_id = it->channel_id;
              match_params.search_channel_ids[i].channel_trigger_id = it->channel_trigger_id;
            }
            match_params.url_keyword_channel_ids.length(url_keyword_channels.size());
            i = 0;
            for (ChannelMatchSet::const_iterator it = url_keyword_channels.begin();
                 it != url_keyword_channels.end(); ++it, ++i)
            {
              match_params.url_keyword_channel_ids[i].channel_id = it->channel_id;
              match_params.url_keyword_channel_ids[i].channel_trigger_id =
                it->channel_trigger_id;
            }
          }

          // get merge target profile, if need
          uim_session->match(
            user_info,
            match_params,
            history_match_result);

          request_params.profiling_available = true;
        }
        catch(const UserInfoSvcs::UserInfoMatcher::ImplementationException& e)
        {
          Stream::Error ostr;
          ostr << FUN <<
            ": UserInfoSvcs::UserInfoMatcher::ImplementationException caught: " <<
            e.description;

          logger()->log(ostr.str(),
            Logging::Logger::EMERGENCY,
            Aspect::BIDDING_FRONTEND,
            "ADS-IMPL-112");
        }
        catch(const UserInfoSvcs::UserInfoMatcher::NotReady& e)
        {
          logger()->log(
            String::SubString("UserInfoManager not ready for matching."),
            TraceLevel::MIDDLE,
            Aspect::BIDDING_FRONTEND);
        }
        catch(const CORBA::SystemException& ex)
        {
          Stream::Error ostr;
          ostr << FUN <<
            ": Can't match history channels. Caught CORBA::SystemException: " <<
            ex;

          logger()->log(ostr.str(),
            Logging::Logger::EMERGENCY,
            Aspect::BIDDING_FRONTEND,
            "ADS-ICON-2");
        }
      }
    }
    else if(trigger_match_result && !trigger_match_result->no_track)
    {
      // fill history channels with context channels
      history_match_result = get_empty_history_matching_();

      AdServer::UserInfoSvcs::UserInfoMatcher::ChannelWeightSeq&
        history_matched_channels = history_match_result->channels;
      const AdServer::ChannelSvcs::ChannelServerBase::ContentChannelAtomSeq&
        content_channels = trigger_match_result->content_channels;

      history_matched_channels.length(content_channels.length());
      for(CORBA::ULong i = 0; i < content_channels.length(); ++i)
      {
        history_matched_channels[i].channel_id =
          content_channels[i].id;
        history_matched_channels[i].weight =
          content_channels[i].weight;
      }
    }

    if(!history_match_result.ptr())
    {
      history_match_result = get_empty_history_matching_();

      if(trigger_match_result)
      {
        /* fill history channels with context channels */
        AdServer::UserInfoSvcs::UserInfoMatcher::ChannelWeightSeq&
          history_matched_channels = history_match_result->channels;
        const AdServer::ChannelSvcs::ChannelServerBase::ContentChannelAtomSeq&
          content_channels = trigger_match_result->content_channels;

        history_matched_channels.length(content_channels.length());

        std::copy(content_channels.get_buffer(),
          content_channels.get_buffer() + content_channels.length(),
          Algs::modify_inserter(history_matched_channels.get_buffer(),
            ContextualChannelConverter()));
      }
    }

    // resolve ip to colo_id
    FrontendCommons::IPMatcher_var ip_matcher =
      common_module_->ip_matcher();

    try
    {
      FrontendCommons::IPMatcher::MatchResult ip_match_result;
      if(ip_matcher.in() &&
         request_params.common_info.peer_ip[0] &&
         ip_matcher->match(
           ip_match_result,
           String::SubString(request_params.common_info.peer_ip),
           String::SubString()))
      {
        request_params.common_info.colo_id = ip_match_result.colo_id;
        request_params.context_info.profile_referer =
           config_->enable_profile_referer() && ip_match_result.profile_referer;
      }
    }
    catch(const FrontendCommons::IPMatcher::InvalidParameter&)
    {}

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      Generics::Time end_process_time = Generics::Time::get_time_of_day();
      Stream::Error ostr;
      ostr << FUN << ": history matching time = " <<
        (end_process_time - start_process_time);
      logger()->log(ostr.str(),
        Logging::Logger::TRACE,
        Aspect::BIDDING_FRONTEND);
    }
  }

  void
  Frontend::get_ccg_keywords_(
    AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq_var& ccg_keywords,
    const RequestInfo& request_info,
    const AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult& history_match_result)
    throw()
  {
    static const char* FUN = "Frontend::get_ccg_keywords_()";

    try
    {
      if(history_match_result.channels.length() &&
         !request_info.skip_ccg_keywords &&
         !request_info.filter_request)
      {
        AdServer::ChannelSvcs::ChannelIdSeq channel_ids;
        channel_ids.length(history_match_result.channels.length());

        for (CORBA::ULong i = 0;
          i < history_match_result.channels.length(); ++i)
        {
          channel_ids[i] = history_match_result.channels[i].channel_id;
        }

        ccg_keywords = channel_servers_->get_ccg_traits(channel_ids);
      }
    }
    catch(const FrontendCommons::ChannelServerSessionPool::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN <<
        ": caught ChannelServerSessionPool::Exception: " <<
        ex.what();
      logger()->log(ostr.str(),
        Logging::Logger::EMERGENCY,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-117");
    }
  }

  bool
  Frontend::consider_campaign_selection_(
    const AdServer::Commons::UserId& user_id, // not null
    const Generics::Time& now,
    const AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult&
      campaign_match_result,
    CORBA::String_var& /*hostname*/)
    throw()
  {
    static const char* FUN = "Bidding::Frontend::consider_campaign_selection_()";

    Generics::Time start_process_time;

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      start_process_time = Generics::Time::get_time_of_day();
    }

    bool result = false;
    if (user_id.is_null())
    {
      return true;
    }
    else
    {
      CORBA::ULong seq_order_len = 0;

      for(CORBA::ULong ad_slot_i = 0;
          ad_slot_i < campaign_match_result.ad_slots.length(); ++ad_slot_i)
      {
        const AdServer::CampaignSvcs::CampaignManager::
          AdSlotResult& ad_slot = campaign_match_result.ad_slots[ad_slot_i];

        for(CORBA::ULong i = 0;
            i < ad_slot.selected_creatives.length(); ++i)
        {
          if(ad_slot.selected_creatives[i].order_set_id)
          {
            ++seq_order_len;
          }
        }
      }

      try
      {
        AdServer::UserInfoSvcs::UserInfoMatcher_var uim_session =
          user_info_client_->user_info_session();

        if(!uim_session.in())
        {
          logger()->log(
            String::SubString("Bidding::Frontend::user_info_post_match_():"
              " non resolved user info session."),
            Logging::Logger::TRACE,
            Aspect::BIDDING_FRONTEND);

          return false;
        }

        UserInfoSvcs::UserInfoManager::SeqOrderSeq seq_orders;
        seq_orders.length(seq_order_len);
        CORBA::ULong result_seq_order_i = 0;

        for(CORBA::ULong ad_slot_i = 0;
            ad_slot_i < campaign_match_result.ad_slots.length();
            ++ad_slot_i)
        {
          const AdServer::CampaignSvcs::CampaignManager::
            AdSlotResult& ad_slot_result = campaign_match_result.ad_slots[ad_slot_i];
          UserInfoSvcs::CampaignIdSeq campaign_ids;
          campaign_ids.length(ad_slot_result.selected_creatives.length());

          for(CORBA::ULong creative_i = 0;
              creative_i < ad_slot_result.selected_creatives.length();
              ++creative_i)
          {
            const AdServer::CampaignSvcs::CampaignManager::
              CreativeSelectResult& creative =
                ad_slot_result.selected_creatives[creative_i];

            if(creative.order_set_id)
            {
              seq_orders[result_seq_order_i].ccg_id = creative.cmp_id;
              seq_orders[result_seq_order_i].set_id = creative.order_set_id;
              seq_orders[result_seq_order_i].imps = 1;

              ++result_seq_order_i;
            }

            campaign_ids[creative_i] = creative.campaign_group_id;
          }

          UserInfoSvcs::FreqCapIdSeq freq_caps;
          UserInfoSvcs::FreqCapIdSeq uc_freq_caps;

          CorbaAlgs::copy_sequence(ad_slot_result.freq_caps, freq_caps);
          CorbaAlgs::copy_sequence(ad_slot_result.uc_freq_caps, uc_freq_caps);

          uim_session->update_user_freq_caps(
            CorbaAlgs::pack_user_id(user_id),
            CorbaAlgs::pack_time(now),
            ad_slot_result.request_id,
            freq_caps,
            uc_freq_caps,
            UserInfoSvcs::FreqCapIdSeq(), // virtual_freq_caps
            seq_orders,
            ad_slot_result.track_impr ? EMPTY_CAMPAIGN_ID_SEQ : campaign_ids,
            ad_slot_result.track_impr ? campaign_ids : EMPTY_CAMPAIGN_ID_SEQ);
        }

        result = true;
      }
      catch(const UserInfoSvcs::UserInfoMatcher::ImplementationException& e)
      {
        Stream::Error ostr;
        ostr << FUN <<
          ": UserInfoSvcs::UserInfoMatcher::ImplementationException caught: " <<
          e.description;

        logger()->log(ostr.str(),
          Logging::Logger::EMERGENCY,
          Aspect::BIDDING_FRONTEND,
          "ADS-IMPL-112");
      }
      catch(const UserInfoSvcs::UserInfoMatcher::NotReady& e)
      {
        logger()->log(
          String::SubString("UserInfoManager not ready for post match."),
          TraceLevel::MIDDLE,
          Aspect::BIDDING_FRONTEND);
      }
      catch(const CORBA::SystemException& ex)
      {
        Stream::Error ostr;
        ostr << FUN <<
          ": Can't do post match. Caught CORBA::SystemException: " <<
          ex;

        logger()->log(ostr.str(),
          Logging::Logger::EMERGENCY,
          Aspect::BIDDING_FRONTEND,
          "ADS-ICON-2");
      }
    }

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      Generics::Time end_process_time = Generics::Time::get_time_of_day();
      Stream::Error ostr;
      ostr << FUN << ": campaign selection considering = " <<
        (end_process_time - start_process_time);
      logger()->log(ostr.str(),
        Logging::Logger::TRACE,
        Aspect::BIDDING_FRONTEND);
    }

    return result;
  }

  bool
  Frontend::process_bid_request_(
    const char* fn,
    AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_out
      campaign_match_result,
    AdServer::Commons::UserId& user_id,
    RequestTask* request_task,
    RequestInfo& request_info,
    const std::string& keywords) throw()
  {
    bool interrupted = false;

    AdServer::CampaignSvcs::CampaignManager::RequestParams&
        request_params(*request_task->request_params);

    // map external id to uid
    resolve_user_id_(
      user_id,
      request_params.common_info,
      request_info);

    if(check_interrupt_(fn, "user resolving", request_task))
    {
      interrupted = true;
    }

    AdServer::ChannelSvcs::ChannelServerBase::MatchResult_var trigger_match_result;

    if (!interrupted)
    {
      trigger_match_(trigger_match_result.out(), request_params, request_info, user_id,
        request_task->hostname, keywords.c_str());
      
      if(check_interrupt_(fn, "trigger matching", request_task))
      {
        interrupted = true;
      }
    }

    // process bid request source independently
    AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult_var
      history_match_result;

    if (!interrupted)
    {
      history_match_(
        history_match_result.out(),
        request_params,
        request_info,
        trigger_match_result.ptr(),
        user_id,
        request_info.current_time,
        request_task->hostname);

      if(check_interrupt_(fn, "history matching", request_task))
      {
        interrupted = true;
      }
    }
    else
    {
      history_match_result = get_empty_history_matching_();
    }

    AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq_var ccg_keywords;

    /*
    if (!interrupted)
    {
      get_ccg_keywords_(ccg_keywords, request_info, history_match_result.in());
      
      if(check_interrupt_(fn, "ccgkeywords receiving", request_task))
      {
        interrupted = true;
      }
    }
    */

    if(interrupted)
    {
      return false;
    }

    select_campaign_(
      campaign_match_result,
      *history_match_result,
      trigger_match_result.ptr(),
      ccg_keywords.ptr(),
      request_info,
      request_params,
      user_id,
      (trigger_match_result &&
        (trigger_match_result->no_track || trigger_match_result->no_adv)) ||
      request_info.filter_request,
      request_task->hostname,
      interrupted);

    if (!interrupted)
    {
      if(check_interrupt_(fn, "campaign selection", request_task))
      {
        return false;
      }

      if(campaign_match_result &&
        campaign_match_result->ad_slots.length() > 0 &&
        campaign_match_result->ad_slots[0].debug_info.trace_ccg[0] &&
        request_params.ad_slots.length() > 0 &&
        logger()->log_level() >= Logging::Logger::TRACE)
      {
        Stream::Error ostr;
        ostr << fn << ": CCG Trace of " <<
        request_params.ad_slots[0].debug_ccg <<
          " for request:" << std::endl;
        
        request_task->print_request(ostr);
        
        ostr << std::endl << campaign_match_result->ad_slots[0].debug_info.trace_ccg;
        
        logger()->log(
          ostr.str(),
          Logging::Logger::TRACE,
          Aspect::BIDDING_FRONTEND);
      }
    }
    else
    {
      // Interrupted CM selection should be the last call of the RequestTask,
      // RequestTask's request params are invalid after the call.
      interrupted_select_campaign_(
        request_task);
      
      return false;
    }

    return true;
  }

  void
  Frontend::interrupted_select_campaign_(
    RequestTask* request_task) throw()
  {
    static const char* FUN = "Bidding::Frontend::interrupted_select_campaign_()";
    
    try
    {
      ConstRequestParamsHolder_var
        request_params(request_task->request_params);

      request_task->request_params.reset();

      passback_task_runner_->enqueue_task(
        Generics::Task_var(
          new InterruptPassbackTask(
            campaign_managers_,
            request_params,
            request_task->hostname)));
    }
    catch(const Generics::TaskRunner::Overflow&)
    {
      // Skip all TaskRunner overflow errors
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << ex.what();

      logger()->log(ostr.str(),
        Logging::Logger::EMERGENCY,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-10554");
    }
  }

  void
  Frontend::select_campaign_(
    AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_out
      campaign_match_result,
    AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult&
      history_match_result,
    const AdServer::ChannelSvcs::ChannelServerBase::MatchResult* trigger_match_result,
    const AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq* ccg_keywords,
    const RequestInfo& request_info,
    AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
    const AdServer::Commons::UserId& user_id,
    bool passback,
    CORBA::String_var& hostname,
    bool interrupted)
    throw()
  {
    static const char* FUN = "Bidding::Frontend::request_campaign_manager_()";

    Generics::Time start_process_time;

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      start_process_time = Generics::Time::get_time_of_day();
    }

    // do campaign selection
    try
    {
      // Fill user info
      if(!user_id.is_null())
      {
        request_params.common_info.user_id = CorbaAlgs::pack_user_id(user_id);
        request_params.common_info.track_user_id =
          request_params.common_info.user_id;
      }

      // Fill debug-info & process fraud
      request_params.only_display_ad = false;
      if(request_params.ad_slots.length() > 0)
      {
        if (!interrupted)
        {
          request_params.need_debug_info = true;
          request_params.ad_slots[0].debug_ccg = request_info.debug_ccg;
        }
        for(size_t i = 0; i < request_params.ad_slots.length(); ++i)
        {
          request_params.ad_slots[i].passback |=
            passback || interrupted ||
              history_match_result.fraud_request;
        }
      }

      // Fill user history data
      request_params.client_create_time = history_match_result.create_time;
      request_params.session_start = history_match_result.session_start;
      request_params.full_freq_caps.swap(
        history_match_result.full_freq_caps);
      request_params.exclude_pubpixel_accounts.swap(
        history_match_result.exclude_pubpixel_accounts);
      request_params.campaign_freqs.swap(
        history_match_result.campaign_freqs);
      
      request_params.seq_orders.length(history_match_result.seq_orders.length());
      for(CORBA::ULong seq_order_i = 0;
          seq_order_i != history_match_result.seq_orders.length();
          ++seq_order_i)
      {
        request_params.seq_orders[seq_order_i].ccg_id =
          history_match_result.seq_orders[seq_order_i].ccg_id;
        request_params.seq_orders[seq_order_i].set_id =
          history_match_result.seq_orders[seq_order_i].set_id;
        request_params.seq_orders[seq_order_i].imps =
          history_match_result.seq_orders[seq_order_i].imps;
      }
      
      request_params.common_info.coord_location.length(
        history_match_result.geo_data_seq.length());
      for(CORBA::ULong i = 0;
          i < history_match_result.geo_data_seq.length(); ++i)
      {
        AdServer::CampaignSvcs::CampaignManager::GeoCoordInfo& res_loc =
          request_params.common_info.coord_location[i];
        res_loc.longitude = history_match_result.geo_data_seq[i].longitude;
        res_loc.latitude = history_match_result.geo_data_seq[i].latitude;
        res_loc.accuracy = history_match_result.geo_data_seq[i].accuracy;
      }

      /* fill input channel sequence for CampaignManager */
      std::size_t uid_channels_length = 0;

      if (trigger_match_result)
      {
        uid_channels_length = trigger_match_result->matched_channels.uid_channels.length();
      }

      request_params.channels.length(
        history_match_result.channels.length() +
          uid_channels_length);

      CORBA::ULong j = 0;
      for (CORBA::ULong i = 0; i < history_match_result.channels.length();
           ++i, ++j)
      {
          request_params.channels[j] = history_match_result.channels[i].channel_id;
      }

      if (trigger_match_result)
      {
        for (CORBA::ULong i = 0;
             i < trigger_match_result->matched_channels.uid_channels.length();
             ++i, ++j)
        {
          request_params.channels[j] =
            trigger_match_result->matched_channels.uid_channels[i];
        }
      }

      // Fill CCG keywords
      if(ccg_keywords)
      {
        request_params.ccg_keywords.length(ccg_keywords->length());
        for(CORBA::ULong i = 0; i < ccg_keywords->length(); ++i)
        {
          const AdServer::ChannelSvcs::ChannelServerBase::CCGKeyword&
            src_ccg_kw = (*ccg_keywords)[i];
          AdServer::CampaignSvcs::CampaignManager::CCGKeywordInfo&
            res_ccg_kw = request_params.ccg_keywords[i];
          res_ccg_kw.ccg_keyword_id = src_ccg_kw.ccg_keyword_id;
          res_ccg_kw.ccg_id = src_ccg_kw.ccg_id;
          res_ccg_kw.channel_id = src_ccg_kw.channel_id;
          res_ccg_kw.max_cpc = src_ccg_kw.max_cpc;
          res_ccg_kw.ctr = src_ccg_kw.ctr;
          res_ccg_kw.click_url = src_ccg_kw.click_url;
          res_ccg_kw.original_keyword = src_ccg_kw.original_keyword;
        }
      }

      // Process black list users
      const Generics::Time day_time =
        request_info.current_time - request_info.current_time.get_gm_time().get_date();

      if (blacklisted_time_intervals_.contains(day_time))
      {
        if (request_params.common_info.user_status ==
            static_cast<CORBA::ULong>(CampaignSvcs::US_OPTIN))
        {
          request_params.common_info.user_status =
            static_cast<CORBA::ULong>(CampaignSvcs::US_BLACKLISTED);
        }

        for(CORBA::ULong slot_i = 0;
            slot_i < request_params.ad_slots.length(); ++slot_i)
        {
          request_params.ad_slots[slot_i].passback = true;
        }
      }

      {
        // fill special tokens
        request_params.common_info.tokens.length(
          (request_info.bid_request_id.empty() ? 0 : 1) +
          (request_info.bid_site_id.empty() ? 0 : 1) +
          (request_info.bid_publisher_id.empty() ? 0 : 1) +
          (request_info.application_id.empty() ? 0 : 1) +
          (!request_info.advertising_id.empty() ||
            !request_info.idfa.empty() ||
            history_match_result.cohort[0] ? 1 : 0) +
          (request_params.common_info.ext_track_params[0] ? 1 : 0));

        CORBA::ULong tok_i = 0;
        if(!request_info.bid_request_id.empty())
        {
          request_params.common_info.tokens[tok_i].name = "BR_ID";
          request_params.common_info.tokens[tok_i].value << request_info.bid_request_id;
          ++tok_i;
        }

        if(!request_info.bid_site_id.empty())
        {
          request_params.common_info.tokens[tok_i].name = "BS_ID";
          request_params.common_info.tokens[tok_i].value << request_info.bid_site_id;
          ++tok_i;
        }

        if(!request_info.bid_publisher_id.empty())
        {
          request_params.common_info.tokens[tok_i].name = "BP_ID";
          request_params.common_info.tokens[tok_i].value << request_info.bid_publisher_id;
          ++tok_i;
        }

        if(!request_info.application_id.empty())
        {
          request_params.common_info.tokens[tok_i].name = "APPLICATION_ID";
          request_params.common_info.tokens[tok_i].value << request_info.application_id;
          ++tok_i;
        }

        if(!request_info.idfa.empty())
        {
          request_params.common_info.tokens[tok_i].name = "IDFA";
          request_params.common_info.tokens[tok_i].value << request_info.idfa;
          ++tok_i;
        }
        else if(!request_info.advertising_id.empty())
        {
          request_params.common_info.tokens[tok_i].name = "ADVERTISING_ID";
          request_params.common_info.tokens[tok_i].value << request_info.advertising_id;
          ++tok_i;
        }
        else if(history_match_result.cohort[0])
        {
          if(request_info.platform_names.find("ipad") !=
              request_info.platform_names.end() ||
            request_info.platform_names.find("iphone") !=
              request_info.platform_names.end() ||
            request_info.platform_names.find("ios") !=
              request_info.platform_names.end())
          {
            request_params.common_info.tokens[tok_i].name = "IDFA";
            request_params.common_info.tokens[tok_i].value = history_match_result.cohort;
            ++tok_i;
          }
          else
          {
            request_params.common_info.tokens[tok_i].name = "ADVERTISING_ID";
            request_params.common_info.tokens[tok_i].value = history_match_result.cohort;
            ++tok_i;
          }
        }

        if(request_params.common_info.ext_track_params[0])
        {
          request_params.common_info.tokens[tok_i].name = "EXT_TRACK_PARAMS";
          request_params.common_info.tokens[tok_i].value =
            request_params.common_info.ext_track_params.in();
          ++tok_i;
        }

        request_params.common_info.tokens.length(tok_i);
      }

      if (interrupted)
      { 
        request_params.required_passback = false;
      }
      else
      {
        campaign_managers_.get_campaign_creative(
          request_params,
          hostname,
          campaign_match_result);
      }
    }
     catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << ex.what();

      logger()->log(ostr.str(),
        Logging::Logger::EMERGENCY,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-118");
    }

    if(!interrupted && logger()->log_level() >= Logging::Logger::TRACE)
    {
      Generics::Time end_process_time = Generics::Time::get_time_of_day();
      Stream::Error ostr;
      ostr << FUN << ": campaign selection time = " <<
        (end_process_time - start_process_time);
      logger()->log(ostr.str(),
        Logging::Logger::TRACE,
        Aspect::BIDDING_FRONTEND);
    }
  }

  void
  Frontend::fill_openrtb_response_(
    std::ostream& response_ostr,
    const RequestInfo& request_info,
    const AdServer::CampaignSvcs::CampaignManager::
      RequestParams& request_params,
    const JsonProcessingContext& context,
    const AdServer::CampaignSvcs::CampaignManager::
      RequestCreativeResult& campaign_match_result)
    throw()
  {
    static const char* FUN = "Frontend::fill_openrtb_response_()";

    try
    {
      AdServer::Commons::JsonFormatter root_json(response_ostr);
      root_json.add_escaped_string(Response::OpenRtb::ID, context.request_id);

      {
        std::ostringstream bidid_ss;
        bidid_ss << request_params.common_info.random;
        root_json.add_string(Response::OpenRtb::BIDID, bidid_ss.str());
      }

      if (request_info.ipw_extension)
      {
        AdServer::Commons::JsonObject ext_obj(root_json.add_object(Response::OpenRtb::EXT));
        ext_obj.add_string(Response::OpenRtb::PROTOCOL, String::SubString("4.0"));
      }

      std::string pub_currency_code;
      {
        AdServer::Commons::JsonObject seatbid_array(root_json.add_array(Response::OpenRtb::SEATBID));
        AdServer::Commons::JsonObject seatbid_obj(seatbid_array.add_object());
        if(!request_info.seat.empty())
        {
          seatbid_obj.add_string(Response::OpenRtb::SEAT, request_info.seat);
        }
        AdServer::Commons::JsonObject bid_array(seatbid_obj.add_array(Response::OpenRtb::BID));

        assert(campaign_match_result.ad_slots.length() ==
          context.ad_slots.size());
        JsonAdSlotProcessingContextList::const_iterator slot_it =
          context.ad_slots.begin();

        for(CORBA::ULong ad_slot_i = 0;
            ad_slot_i < campaign_match_result.ad_slots.length();
            ++ad_slot_i, ++slot_it)
        {
          const AdServer::CampaignSvcs::CampaignManager::
            AdSlotResult& ad_slot_result = campaign_match_result.ad_slots[ad_slot_i];

          if(pub_currency_code.empty())
          {
            pub_currency_code = ad_slot_result.pub_currency_code;
            String::AsciiStringManip::to_upper(pub_currency_code);
          }

          if(ad_slot_result.selected_creatives.length() > 0)
          {
            // campaigns selected
            CampaignSvcs::RevenueDecimal sum_pub_ecpm = CampaignSvcs::RevenueDecimal::ZERO;
            for(CORBA::ULong creative_i = 0;
                creative_i < ad_slot_result.selected_creatives.length();
                ++creative_i)
            {
              sum_pub_ecpm += CorbaAlgs::unpack_decimal<CampaignSvcs::RevenueDecimal>(
                ad_slot_result.selected_creatives[creative_i].pub_ecpm);
            }

            limit_max_cpm_(
              sum_pub_ecpm, request_params.publisher_account_ids);

            // result price in RUB/1000, ecpm is in 0.01/1000
            CampaignSvcs::RevenueDecimal openrtb_price = CampaignSvcs::RevenueDecimal::div(
              sum_pub_ecpm,
              CampaignSvcs::RevenueDecimal(false, 100, 0));

//            std::string escaped_impid = String::StringManip::json_escape(slot_it->id);
            std::string escaped_creative_url;
            std::string escaped_creative_body;

            if(ad_slot_result.creative_url[0])
            {
              escaped_creative_url = String::StringManip::json_escape(
                String::SubString(ad_slot_result.creative_url));
            }
            if(!slot_it->native && ad_slot_result.creative_body[0])
            {
              escaped_creative_body = String::StringManip::json_escape(
                String::SubString(ad_slot_result.creative_body));
            }

            AdServer::Commons::JsonObject bid_object(bid_array.add_object());
            bid_object.add_as_string(Response::OpenRtb::ID, ad_slot_i);
            if (!slot_it->deal_id.empty())
            {
              bid_object.add_as_string(Response::OpenRtb::DEAL_ID, slot_it->deal_id);
            }
            bid_object.add_escaped_string(Response::OpenRtb::IMPID, slot_it->id);
            bid_object.add_number(Response::OpenRtb::PRICE, openrtb_price);

            if (request_info_filler_->fill_adid(request_info))
            {
              bid_object.add_as_string(Response::OpenRtb::ADID, ad_slot_result.selected_creatives[0].creative_id);
            }
            bid_object.add_as_string(Response::OpenRtb::CRID, ad_slot_result.selected_creatives[0].creative_version_id);

            {
              AdServer::Commons::JsonObject adomain_obj(bid_object.add_array(Response::OpenRtb::ADOMAIN));

              // standard OpenRTB, Allyes, OpenX
              for(CORBA::ULong creative_i = 0;
                  creative_i < ad_slot_result.selected_creatives.length();
                  ++creative_i)
              {
                try
                {
                  HTTP::BrowserAddress adomain(String::SubString(
                    ad_slot_result.selected_creatives[creative_i].destination_url));

                  String::SubString host = adomain.host();

                  if(request_info.truncate_domain && WWW_PREFIX.start(host))
                  {
                    host = host.substr(WWW_PREFIX.str.size());
                  }

                  if (!host.empty())
                  {
                    adomain_obj.add_escaped_string(host);
                  }
                }
                catch (HTTP::URLAddress::InvalidURL& ex)
                {
                  Stream::Error ostr;
                  ostr << FUN << ": adomain extract failed from '" <<
                    ad_slot_result.selected_creatives[creative_i].destination_url <<
                    "', " << ex.what();

                  logger()->log(
                    ostr.str(),
                    Logging::Logger::ERROR,
                    Aspect::BIDDING_FRONTEND,
                    "ADS-IMPL-7604");
                }
              }
            }

            {
              bool notice_enabled = false;
              bool need_ipw_extension = request_info.ipw_extension;
             
              // ADSC-10918 Native ads
              if (slot_it->native)
              {
                if (request_info.native_ads_instantiate_type == SourceTraits::NAIT_ADM ||
                  request_info.native_ads_instantiate_type == SourceTraits::NAIT_ADM_NATIVE)
                {
                  std::ostringstream native_response_ostr;

                  {
                    AdServer::Commons::JsonFormatter native_json(native_response_ostr);
                    fill_native_response_(
                      &native_json,
                      *slot_it->native,
                      ad_slot_result,
                      true,
                      request_info.native_ads_instantiate_type == SourceTraits::NAIT_ADM_NATIVE
                      );
                  }
                  
                  escaped_creative_body = String::StringManip::json_escape(
                    String::SubString(native_response_ostr.str()));
                }
                else if (
                  request_info.native_ads_instantiate_type == SourceTraits::NAIT_EXT)
                {
                  AdServer::Commons::JsonObject ext_obj(
                    bid_object.add_object(Response::OpenRtb::EXT));
                  
                  fill_native_response_(
                    &ext_obj,
                    *slot_it->native,
                    ad_slot_result,
                    true,
                    true // add native root
                    );

                  notice_enabled = true;
                }
              }

              if(!escaped_creative_body.empty())
              {
                bid_object.add_string(Response::OpenRtb::ADM, escaped_creative_body);
                notice_enabled = true;
              }
              else if(!escaped_creative_url.empty())
              {                
                if(request_params.ad_instantiate_type == AdServer::CampaignSvcs::AIT_VIDEO_URL)
                {
                  AdServer::Commons::JsonObject ext_obj(bid_object.add_object(Response::OpenRtb::EXT));
                  
                  if (need_ipw_extension)
                  {
                    ext_obj.add_escaped_string(
                      Response::OpenRtb::ADVERTISER_NAME,
                      String::SubString(ad_slot_result.selected_creatives[0].advertiser_name));

                    need_ipw_extension = false;
                  }

                  ext_obj.add_string(Response::OpenRtb::VAST_URL, escaped_creative_url);

                  if(ad_slot_result.ext_tokens.length() > 0)
                  {
                    for(CORBA::ULong token_i = 0;
                      token_i < ad_slot_result.ext_tokens.length(); ++token_i)
                    {
                      std::string escaped_name = String::StringManip::json_escape(
                        String::SubString(ad_slot_result.ext_tokens[token_i].name));

                      ext_obj.add_escaped_string(escaped_name,
                        String::SubString(ad_slot_result.ext_tokens[token_i].value));
                    }
                  }

                  notice_enabled = true;
                }
                else if(request_params.ad_instantiate_type ==
                  AdServer::CampaignSvcs::AIT_VIDEO_URL_IN_BODY)
                {
                  bid_object.add_string(Response::OpenRtb::ADM, escaped_creative_url);
                }
                else
                {
                  bid_object.add_string(Response::OpenRtb::NURL, escaped_creative_url);
                }
              }

              if (need_ipw_extension)
              {
                AdServer::Commons::JsonObject ext_obj(bid_object.add_object(Response::OpenRtb::EXT));
                ext_obj.add_escaped_string(
                  Response::OpenRtb::ADVERTISER_NAME,
                  String::SubString(ad_slot_result.selected_creatives[0].advertiser_name));
              }

              if(!request_info.notice_url.empty())
              {
                bid_object.add_escaped_string(Response::OpenRtb::NURL,
                  request_info.notice_url);
              }
              else if(notice_enabled && ad_slot_result.notice_url[0])
              {
                bid_object.add_escaped_string(Response::OpenRtb::NURL,
                  String::SubString(ad_slot_result.notice_url));
              }
              
              // FIXME: can not find 'bid[attr]' in OpenRTB 2.2/2.3 spec
              print_int_category_seq(
                bid_object,
                Response::OpenRtb::ATTR,
                ad_slot_result.external_visual_categories);

            }

            if(!slot_it->banners.empty()) // fill extensions for overlay
            {
              auto banner_by_size_it = slot_it->size_banner.find(
                ad_slot_result.tag_size.in());

              if(banner_by_size_it != slot_it->size_banner.end())
              {
                const JsonAdSlotProcessingContext::Banner& use_banner =
                  *(banner_by_size_it->second);

                bool add_ext_width_height = (
                  ad_slot_result.selected_creatives.length() == 1 &&
                  use_banner.ext_type == "20");

                if(add_ext_width_height ||
                   request_params.common_info.request_type == AdServer::CampaignSvcs::AR_OPENX)
                {
                  AdServer::Commons::JsonObject ext_obj(bid_object.add_object(Response::OpenRtb::EXT));

                  if(add_ext_width_height)
                  {
                    ext_obj.add_as_string(Response::OpenRtb::WIDTH, ad_slot_result.overlay_width);
                    ext_obj.add_as_string(Response::OpenRtb::HEIGHT, ad_slot_result.overlay_height);
                  }

                  if(request_params.common_info.request_type == AdServer::CampaignSvcs::AR_OPENX)
                  {
                    if(!use_banner.matching_ad.empty())
                    {
                      ext_obj.add_number(
                        Response::OpenX::MATCHING_AD_ID,
                        use_banner.matching_ad);
                    }

                    print_int_category_seq(
                      ext_obj,
                      Response::OpenX::AD_OX_CATS,
                      ad_slot_result.external_content_categories);
                  }
                }

                if (!(banner_by_size_it->second->width.empty() ||
                    banner_by_size_it->second->height.empty()))
                {
                  bid_object.add_number(
                    Response::OpenRtb::WIDTH,
                    banner_by_size_it->second->width);
                  bid_object.add_number(
                    Response::OpenRtb::HEIGHT,
                    banner_by_size_it->second->height);
                }
              }
            }

            if(ad_slot_result.iurl[0])
            {
              bid_object.add_escaped_string(
                Response::OpenRtb::IURL,
                String::SubString(ad_slot_result.iurl));
            }

            bid_object.add_as_string(
              Response::OpenRtb::CID,
              ad_slot_result.selected_creatives[0].campaign_group_id);
          } // if(ad_slot_result.selected_creatives.length() > 0)
        } // for(CORBA::ULong ad_slot_i = 0, ...
      }
      root_json.add_string(Response::OpenRtb::CUR, pub_currency_code);
    }
    catch(const AdServer::Commons::JsonObject::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << " Error on formatting Json response: '" << ex.what() << "'";
      logger()->log(ostr.str(), Logging::Logger::EMERGENCY, Aspect::BIDDING_FRONTEND);
    }
  }

  namespace
  {
    struct YandexIdFormatter
    {
      const String::SubString id_str;

      YandexIdFormatter(const String::SubString& id_str_val)
      : id_str(id_str_val)
      {}
    };

    std::ostream& operator<<(std::ostream& os, const YandexIdFormatter& fmt)
    {
      const char* const END = fmt.id_str.end();
      bool only_digits =
        String::AsciiStringManip::NUMBER.find_nonowned(fmt.id_str.begin(), END) == END;

      bool is_number =
        (!fmt.id_str.empty()) &&
        (fmt.id_str[0] != '0') &&
        only_digits;

      if (is_number)
      {
        os << fmt.id_str;
      }
      else
      {
        os << "\"" << fmt.id_str << "\"";
      }
      return os;
    }
  }

  void
  Frontend::fill_yandex_response_(
    std::ostream& response_ostr,
    const RequestInfo& request_info,
    const AdServer::CampaignSvcs::CampaignManager::
      RequestParams& request_params,
    const JsonProcessingContext& context,
    const AdServer::CampaignSvcs::CampaignManager::
      RequestCreativeResult& campaign_match_result)
    throw()
  {
    static const char* FUN = "Frontend::fill_yandex_response_()";

    try
    {
      std::string escaped_request_id =
        String::StringManip::json_escape(context.request_id);

      AdServer::Commons::JsonFormatter root_json(response_ostr);
      root_json.add(Response::Yandex::ID, YandexIdFormatter(escaped_request_id));
      root_json.add_number(Response::Yandex::UNITS, 2);

      {
        std::ostringstream bidid_ss;
        bidid_ss << request_params.common_info.random;
        root_json.add_string(Response::OpenRtb::BIDID, bidid_ss.str());
      }

      bool some_campaign_selected = false;

      {
        for(CORBA::ULong ad_slot_i = 0;
          ad_slot_i < campaign_match_result.ad_slots.length();
          ++ad_slot_i)
        {
          some_campaign_selected |= (
            campaign_match_result.ad_slots[ad_slot_i].selected_creatives.length() > 0);
        }
      }

      {
        AdServer::Commons::UserId user_id = CorbaAlgs::unpack_user_id(
          request_params.common_info.user_id);

        if(!user_id.is_null())
        {
          root_json.add_string(
            Response::Yandex::SETUSERDATA,
            common_module_->user_id_controller()->ssp_uuid_string(
              common_module_->user_id_controller()->ssp_uuid(
                user_id, request_info.source_id)));
        }
        else if(some_campaign_selected && request_params.common_info.external_user_id[0] == 0)
        {
          std::string tmp_ssp_user_id = common_module_->user_id_controller()->ssp_uuid_string(
            common_module_->user_id_controller()->ssp_uuid(
              common_module_->user_id_controller()->generate().uuid(),
              request_info.source_id));
          tmp_ssp_user_id[0] = '~';
          root_json.add_string(
            Response::Yandex::SETUSERDATA,
            tmp_ssp_user_id);
        }
      }

      std::string pub_currency_code;
      {
        AdServer::Commons::JsonObject bidset(root_json.add_array(Response::Yandex::BIDSET));
        AdServer::Commons::JsonObject bidsetobject(bidset.add_object());
        AdServer::Commons::JsonObject bidarray(bidsetobject.add_array(Response::Yandex::BID));

        assert(campaign_match_result.ad_slots.length() ==
          context.ad_slots.size());
        JsonAdSlotProcessingContextList::const_iterator slot_it =
          context.ad_slots.begin();

        for(CORBA::ULong ad_slot_i = 0;
            ad_slot_i < campaign_match_result.ad_slots.length();
            ++ad_slot_i, ++slot_it)
        {
          const AdServer::CampaignSvcs::CampaignManager::
            AdSlotResult& ad_slot_result = campaign_match_result.ad_slots[ad_slot_i];

          if(pub_currency_code.empty())
          {
            pub_currency_code = ad_slot_result.pub_currency_code;
            String::AsciiStringManip::to_upper(pub_currency_code);
          }

          if(ad_slot_result.selected_creatives.length() > 0)
          {
            // campaigns selected
            CampaignSvcs::RevenueDecimal sum_pub_ecpm = CampaignSvcs::RevenueDecimal::ZERO;
            for(CORBA::ULong creative_i = 0;
                creative_i < ad_slot_result.selected_creatives.length();
                ++creative_i)
            {
              sum_pub_ecpm += CorbaAlgs::unpack_decimal<CampaignSvcs::RevenueDecimal>(
                ad_slot_result.selected_creatives[creative_i].pub_ecpm);
            }

            limit_max_cpm_(
              sum_pub_ecpm, request_params.publisher_account_ids);

            // result price in RUB/1000000, ecpm is in 0.01/1000
            CampaignSvcs::RevenueDecimal openrtb_price = CampaignSvcs::RevenueDecimal::mul(
              sum_pub_ecpm,
              CampaignSvcs::RevenueDecimal(false, 10, 0),
              Generics::DMR_ROUND);

            std::string escaped_creative_url;
            std::string escaped_click_params;
            if(ad_slot_result.creative_url[0])
            {
              escaped_creative_url = String::StringManip::json_escape(
                String::SubString(ad_slot_result.creative_url));
            }

            if(ad_slot_result.click_params[0])
            {
              std::string base64_encoded_click_params;
              String::SubString click_params(ad_slot_result.click_params);
              String::StringManip::base64mod_encode(
                base64_encoded_click_params,
                click_params.data(),
                click_params.length());
              escaped_click_params = String::StringManip::json_escape(
                base64_encoded_click_params);
            }

            AdServer::Commons::JsonObject bid_object(bidarray.add_object());

            bid_object.add(Response::Yandex::ID,
              YandexIdFormatter(slot_it->id));
            bid_object.add_number(Response::Yandex::PRICE,
              openrtb_price.integer<unsigned long>());
            // Always add adid for yandex (don't check source_id)
            bid_object.add_number(Response::Yandex::ADID,
              ad_slot_result.selected_creatives[0].creative_id);

            // fill view_notices, nurl, dsp_params
            if(ad_slot_result.track_pixel_urls.length() > 0)
            {
              AdServer::Commons::JsonObject track_pixel_urls(
                bid_object.add_array(Response::Yandex::VIEW_NOTICES));

              for(CORBA::ULong i = 0;
                i < ad_slot_result.track_pixel_urls.length(); ++i)
              {
                track_pixel_urls.add_escaped_string(
                  String::SubString(ad_slot_result.track_pixel_urls[i]));
              }
            }

            if(!request_info.notice_url.empty())
            {
              bid_object.add_escaped_string(Response::Yandex::NURL,
                request_info.notice_url);
            }
            else if(ad_slot_result.notice_url[0])
            {
              bid_object.add_escaped_string(Response::Yandex::NURL,
                String::SubString(ad_slot_result.notice_url));
            }

            {
              AdServer::Commons::JsonObject banner(bid_object.add_object(Response::Yandex::BANNER));

              auto banner_by_size_it = slot_it->size_banner.find(
                ad_slot_result.tag_size.in());

              if(banner_by_size_it != slot_it->size_banner.end())
              {
                banner.add_number(Response::Yandex::WIDTH, banner_by_size_it->second->width);
                banner.add_number(Response::Yandex::HEIGHT, banner_by_size_it->second->height);
              }
            }

            {
              AdServer::Commons::JsonObject dsp_params(bid_object.add_object(Response::Yandex::DSP_PARAMS));
              dsp_params.add_string(Response::Yandex::URL_PARAM3, escaped_click_params);
              dsp_params.add_string(Response::Yandex::URL_PARAM4, escaped_click_params);

              if(ad_slot_result.yandex_track_params[0])
              {
                std::string escaped_track_params;
                String::StringManip::base64mod_encode(
                  escaped_track_params,
                  ad_slot_result.yandex_track_params,
                  ::strlen(ad_slot_result.yandex_track_params));
                dsp_params.add_escaped_string(
                  Response::Yandex::URL_PARAM17,
                  escaped_track_params);
                dsp_params.add_escaped_string(
                  Response::Yandex::URL_PARAM18,
                  escaped_track_params);
              }
            }

            // fill adm, token, properties
            for(CORBA::ULong token_i = 0; token_i < ad_slot_result.tokens.length(); ++token_i)
            {
              std::string escaped_name = String::StringManip::json_escape(
                String::SubString(ad_slot_result.tokens[token_i].name));

              bid_object.add_escaped_string(escaped_name,
                String::SubString(ad_slot_result.tokens[token_i].value));
            }
            //} // 
          } // if(ad_slot_result.selected_creatives.length() > 0)
        } // for(CORBA::ULong ad_slot_i = 0, ...
      } // close bidset oject and array
      root_json.add_string(Response::Yandex::CUR, pub_currency_code);
    }
    catch(const AdServer::Commons::JsonObject::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << " Error on formatting Json response: '" << ex.what() << "'";
      logger()->log(ostr.str(), Logging::Logger::EMERGENCY, Aspect::BIDDING_FRONTEND);
    }
  }

  void
  Frontend::update_config_()
    throw ()
  {
    static const char* FUN = "Frontend::update_config_()";

    try
    {
      AdServer::CampaignSvcs::ColocationFlagsSeq_var colocations =
        campaign_managers_.get_colocation_flags();

      ExtConfig_var new_config(new ExtConfig());

      for (CORBA::ULong i = 0; i < colocations->length(); ++i)
      {
        ExtConfig::Colocation colocation;
        colocation.flags = colocations[i].flags;
        new_config->colocations.insert(
          ExtConfig::ColocationMap::value_type(
            colocations[i].colo_id,
            colocation));
      }

      set_ext_config_(new_config);
    }
    catch (const eh::Exception& e)
    {
      logger()->sstream(Logging::Logger::CRITICAL,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-118") << FUN << ": Can't update colocation flags, "
        "caught eh::Exception: " << e.what();
    }

    try
    {
      planner_->schedule(
        Generics::Goal_var(new UpdateConfigTask(this, control_task_runner_)),
        Generics::Time::get_time_of_day() + common_config_->update_period());
    }
    catch (const eh::Exception& ex)
    {
      logger()->sstream(Logging::Logger::EMERGENCY,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-7605") <<
        FUN << ": schedule failed: " << ex.what();
    }
  }

  void
  Frontend::flush_state_()
    throw ()
  {
    static const char* FUN = "Frontend::flush_state_()";

    unsigned long reached_max_pending_tasks;

    {
      MaxPendingSyncPolicy::WriteGuard lock(reached_max_pending_tasks_lock_);
      reached_max_pending_tasks = reached_max_pending_tasks_;
      reached_max_pending_tasks_ = 0;
    }

    if(reached_max_pending_tasks > 0)
    {
      Stream::Error ostr;
      ostr << FUN << ": reached max pending tasks: " <<
        reached_max_pending_tasks;

      logger()->log(ostr.str(),
        Logging::Logger::WARNING,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-7602");
    }

    try
    {
      planner_->schedule(
        Generics::Goal_var(new FlushStateTask(this, control_task_runner_)),
        Generics::Time::get_time_of_day() + (
          config_->flush_period().present() ? *config_->flush_period() : 10));
    }
    catch (const eh::Exception& ex)
    {
      logger()->sstream(Logging::Logger::EMERGENCY,
        Aspect::BIDDING_FRONTEND,
        "ADS-IMPL-7605") <<
        FUN << ": schedule failed: " << ex.what();
    }
  }

  AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult*
  Frontend::get_empty_history_matching_()
    throw(eh::Exception)
  {
    AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult_var res =
      new AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult();
    res->fraud_request = false;
    res->times_inited = false;
    res->last_request_time = CorbaAlgs::pack_time(Generics::Time::ZERO);
    res->create_time = CorbaAlgs::pack_time(Generics::Time::ZERO);
    res->session_start = CorbaAlgs::pack_time(Generics::Time::ZERO);
    res->colo_id = -1;
    return res._retn();
  }

  void
  Frontend::protobuf_log_handler_(
    google::protobuf::LogLevel level,
    const char* filename,
    int line,
    const std::string& message)
  {
    static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };

    if (level == google::protobuf::LOGLEVEL_ERROR ||
        level == google::protobuf::LOGLEVEL_FATAL)
    {
      Stream::Error ostr;
      ostr << "[libprotobuf " << level_names[level] <<
        ' ' << filename << ':' << line << "] " << message;
      throw RequestTask::Invalid(ostr.str());
    }
  }

  void
  Frontend::fill_account_traits_() throw()
  {
    for(auto account_it = config_->Account().begin();
      account_it != config_->Account().end(); ++account_it)
    {
      RequestInfoFiller::AccountTraits_var& target_account_ptr =
        account_traits_[account_it->account_id()];

      if(!target_account_ptr.in())
      {
        target_account_ptr = new RequestInfoFiller::AccountTraits();
      }

      RequestInfoFiller::AccountTraits& target_account = *target_account_ptr;

      if(account_it->max_cpm_value().present())
      {
        CampaignSvcs::RevenueDecimal limit = CampaignSvcs::RevenueDecimal::mul(
          AdServer::Commons::extract_decimal<CampaignSvcs::RevenueDecimal>(
            account_it->max_cpm_value().get()),
          MAX_CPM_CONF_MULTIPLIER,
          Generics::DMR_FLOOR);

        target_account.max_cpm = limit;
      }
      
      if(account_it->display_billing_id().present())
      {
        target_account.display_billing_id = *(account_it->display_billing_id());
      }

      if(account_it->video_billing_id().present())
      {
        target_account.video_billing_id = *(account_it->video_billing_id());
      }

      if(account_it->google_encryption_key().present())
      {
        target_account.google_encryption_key_size = String::StringManip::hex_decode(
          *(account_it->google_encryption_key()),
          target_account.google_encryption_key);
      }

      if(account_it->google_integrity_key().present())
      {
        target_account.google_integrity_key_size = String::StringManip::hex_decode(
          *(account_it->google_integrity_key()),
          target_account.google_integrity_key);
      }
    }
  }

  inline
  void
  Frontend::limit_max_cpm_(
    AdServer::CampaignSvcs::RevenueDecimal& val,
    const AdServer::CampaignSvcs::ULongSeq& account_ids)
    const throw()
  {
    for(CORBA::ULong i = 0; i < account_ids.length(); ++i)
    {
      unsigned long account_id = account_ids[i];
      auto account_it = account_traits_.find(account_id);
      if(account_it != account_traits_.end() && account_it->second->max_cpm.present())
      {
        val = std::min(val, *(account_it->second->max_cpm));
      }
    }
  }

  inline void
  Frontend::interrupt_(
    const char* fun,
    const char* stage,
    const RequestTask* request_task)
    throw ()
  {
    Generics::Time timeout = Generics::Time::get_time_of_day() - request_task->start_processing_time();
    if (stats_.in())
    {
      stats_->add_timeout(timeout);
    }

    if (logger()->log_level() >= Logging::Logger::TRACE)
    {
      Stream::Error ostr;
      ostr << fun << ": request processing timed out(" << timeout << "):"
        << std::endl;

      request_task->print_request(ostr);

      logger()->log(
        ostr.str(),
        Logging::Logger::TRACE,
        Aspect::BIDDING_FRONTEND);
    }

    std::string ostr(fun);
    ostr += ": interrupted at ";
    ostr += stage;
    ostr += ", after";

    group_logger()->add_error(
      request_task->hostname.in()?
        request_task->hostname: "Undefined host",
      ostr,
      timeout,
      Logging::Logger::ERROR,
      Aspect::BIDDING_FRONTEND,
      "ADS-IMPL-7600");
  }

  bool
  Frontend::check_interrupt_(
    const char* fun,
    const char* stage,
    const RequestTask* request_task)
    throw()
  {
    if(request_task->interrupt())
    {
      interrupt_(fun, stage, request_task);
      return true;
    }
    return false;
  }


  

  AdServer::CampaignSvcs::AdInstantiateType
  Frontend::adapt_instantiate_type_(const std::string& inst_type_str)
    throw(Exception)
  {
    if(inst_type_str == "url")
    {
      return AdServer::CampaignSvcs::AIT_URL;
    }

    if(inst_type_str == "video url")
    {
      return AdServer::CampaignSvcs::AIT_VIDEO_URL;
    }

    if(inst_type_str == "video url in body")
    {
      return AdServer::CampaignSvcs::AIT_VIDEO_URL_IN_BODY;
    }

    if(inst_type_str == "body")
    {
      return AdServer::CampaignSvcs::AIT_BODY;
    }

    if(inst_type_str == "script with url")
    {
      return AdServer::CampaignSvcs::AIT_SCRIPT_WITH_URL;
    }

    if(inst_type_str == "iframe with url")
    {
      return AdServer::CampaignSvcs::AIT_IFRAME_WITH_URL;
    }

    if(inst_type_str == "url parameters")
    {
      return AdServer::CampaignSvcs::AIT_URL_PARAMS;
    }

    if(inst_type_str == "encoded url parameters")
    {
      return AdServer::CampaignSvcs::AIT_DATA_URL_PARAM;
    }

    if(inst_type_str == "data parameter value")
    {
      return AdServer::CampaignSvcs::AIT_DATA_PARAM_VALUE;
    }

    Stream::Error ostr;
    ostr << "unknown instantiate type '" << inst_type_str << "'";
    throw Exception(ostr);
  }

  SourceTraits::NativeAdsInstantiateType
  Frontend::adapt_native_ads_instantiate_type_(
    const std::string& inst_type_str)
    throw(Exception)
  {
    if(inst_type_str == "none")
    {
      return SourceTraits::NAIT_NONE;
    }
    else if(inst_type_str == "adm")
    {
      return SourceTraits::NAIT_ADM;
    }
    else if(inst_type_str == "adm_native")
    {
      return SourceTraits::NAIT_ADM_NATIVE;
    }
    else if(inst_type_str == "ext")
    {
      return SourceTraits::NAIT_EXT;
    }
    
    Stream::Error ostr;
    ostr << "unknown native ads instantiate type '" << inst_type_str << "'";
    throw Exception(ostr);
  }

  AdServer::CampaignSvcs::NativeAdsImpressionTrackerType
  Frontend::adapt_native_ads_impression_tracker_type_(
    const std::string& imp_type_str)
    throw(Exception)
  {
    if(imp_type_str == "imp")
    {
      return AdServer::CampaignSvcs::NAITT_IMP;
    }

    if(imp_type_str == "js")
    {
      return AdServer::CampaignSvcs::NAITT_JS;
    }

    if(imp_type_str == "resources")
    {
      return AdServer::CampaignSvcs::NAITT_RESOURCES;
    }

    Stream::Error ostr;
    ostr << "unknown native ads impression tracker type '" <<
      imp_type_str << "'";
    throw Exception(ostr);
  }


  // ADSC-10918 Native ads
  void
  Frontend::fill_native_response_(
    AdServer::Commons::JsonObject* root_json,
    const JsonAdSlotProcessingContext::Native& native_context,
    const AdServer::CampaignSvcs::CampaignManager::AdSlotResult& ad_slot_result,
    bool need_escape,
    bool add_root_native)
  {
    std::unique_ptr<AdServer::Commons::JsonObject> native_obj_holder;

    if(add_root_native)
    {
      native_obj_holder.reset(
        new AdServer::Commons::JsonObject(root_json->add_object(Response::OpenRtb::NATIVE)));
    }

    AdServer::Commons::JsonObject& native_obj =
      add_root_native ? *native_obj_holder : *root_json;

    {
      assert(ad_slot_result.selected_creatives.length());
      native_obj.add_string(
        Response::OpenRtb::NATIVE_VER,
        Response::OpenRtb::CURRENT_NATIVE_VER);
      AdServer::Commons::JsonObject link_obj(
        native_obj.add_object(Response::OpenRtb::NATIVE_LINK));
      link_obj.add_opt_escaped_string(
        Response::OpenRtb::NATIVE_LINK_URL,
        String::SubString(
          ad_slot_result.selected_creatives[0].click_url),
        need_escape);
    }

    if (native_context.video_assets.empty())
    {
      if(ad_slot_result.track_html_body[0])
      {
        native_obj.add_opt_escaped_string(
          Response::OpenRtb::NATIVE_JS_TRACKER,
          String::SubString(ad_slot_result.track_html_body),
          need_escape);
      }

      if(ad_slot_result.track_pixel_urls.length() > 0)
      {
        AdServer::Commons::JsonObject imp_trackers_obj(
          native_obj.add_array(Response::OpenRtb::NATIVE_IMP_TRACKERS));
        
        for(CORBA::ULong i = 0;
            i < ad_slot_result.track_pixel_urls.length(); ++i)
        {
          imp_trackers_obj.add_opt_escaped_string(
            String::SubString(ad_slot_result.track_pixel_urls[i]),
            need_escape);
        }
      }
    }

    AdServer::Commons::JsonObject assets_obj(
      native_obj.add_array(Response::OpenRtb::NATIVE_ASSETS));

    // Data assets
    size_t data_i = 0;
    for (auto data_it = native_context.data_assets.begin();
      data_it != native_context.data_assets.end(); ++data_it, ++data_i)
    {
      const AdServer::CampaignSvcs::CampaignManager::TokenInfo& token =
        ad_slot_result.native_data_tokens[data_i];
      if(token.value[0])
      {
        AdServer::Commons::JsonObject asset(
          assets_obj.add_object());
        asset.add_number(
          Response::OpenRtb::NATIVE_ASSET_ID, data_it->id);
        bool is_title =
          data_it->data_type == JsonAdSlotProcessingContext::Native::NDTE_TITLE;

        size_t token_len = ::strlen(token.value);
        size_t data_len = static_cast<size_t>(data_it->len);

        String::SubString res_text;
        String::StringManip::utf8_substr(
          String::SubString(token.value),
          data_len && token_len > data_len ? data_len : token_len,
          res_text);

        AdServer::Commons::JsonObject data_asset(
          asset.add_object(
            is_title ? Response::OpenRtb::NATIVE_ASSET_TITLE :
              Response::OpenRtb::NATIVE_ASSET_DATA));
        data_asset.add_opt_escaped_string(
          is_title ? Response::OpenRtb::NATIVE_ASSET_TITLE_TEXT :
            Response::OpenRtb::NATIVE_ASSET_DATA_VALUE,
          res_text,
          need_escape);
      }
    }

    // Image assets
    if(ad_slot_result.native_image_tokens.length() > 0)
    {
      size_t image_i = 0;
      for (auto image_it = native_context.image_assets.begin();
         image_it != native_context.image_assets.end(); ++image_it, ++image_i)
      {
        const AdServer::CampaignSvcs::CampaignManager::TokenImageInfo& token =
          ad_slot_result.native_image_tokens[image_i];
        if(token.value[0])
        {
          AdServer::Commons::JsonObject asset(assets_obj.add_object());
          asset.add_number(
            Response::OpenRtb::NATIVE_ASSET_ID, image_it->id);
          if (image_it->image_type ==
            JsonAdSlotProcessingContext::Native::NITE_MAIN)
          {
            asset.add_number(
              Response::OpenRtb::NATIVE_ASSET_REQUIRED, 1);
          }

          AdServer::Commons::JsonObject image_asset(
            asset.add_object(Response::OpenRtb::NATIVE_ASSET_IMG));

          image_asset.add_number(
            Response::OpenRtb::NATIVE_ASSET_REQUIRED, 0);

          image_asset.add_opt_escaped_string(
            Response::OpenRtb::NATIVE_ASSET_IMG_URL,
            String::SubString(token.value),
            need_escape);

          image_asset.add_number(
            Response::OpenRtb::NATIVE_ASSET_IMG_W,
            token.width);

          image_asset.add_number(
            Response::OpenRtb::NATIVE_ASSET_IMG_H,
            token.height);
        }
      }
    }

    // Video asset
    if(ad_slot_result.creative_body[0])
    {
      if (!native_context.video_assets.empty())
      {
        const JsonAdSlotProcessingContext::Native::Video& video =
          native_context.video_assets[0];
        AdServer::Commons::JsonObject asset(assets_obj.add_object());
        asset.add_number(
          Response::OpenRtb::NATIVE_ASSET_ID, video.id);
        asset.add_number(
          Response::OpenRtb::NATIVE_ASSET_REQUIRED, 1);

        AdServer::Commons::JsonObject video_asset(
          asset.add_object(Response::OpenRtb::NATIVE_ASSET_VIDEO));

        video_asset.add_opt_escaped_string(
          Response::OpenRtb::NATIVE_ASSET_VIDEO_TAG,
          String::SubString(ad_slot_result.creative_body),
          need_escape);
      }
    }
  }
}
}
