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

#ifndef REQUESTINFOSVCS_REQUESTINFOMANAGER_BILLINGPROCESSOR_HPP
#define REQUESTINFOSVCS_REQUESTINFOMANAGER_BILLINGPROCESSOR_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>

#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/MemBuf.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <ProfilingCommons/ProfileMap/ProfileMapFactory.hpp>
#include <ProfilingCommons/PlainStorageAdapters.hpp>
#include <RequestInfoSvcs/RequestInfoCommons/RequestProfile.hpp>

#include "RequestActionProcessor.hpp"

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class BillingProcessor:
      public virtual ReferenceCounting::AtomicImpl,
      public virtual Generics::ActiveObject,
      public virtual RequestActionProcessor,
      protected virtual Generics::CompositeActiveObject
    {
    public:
      DECLARE_EXCEPTION(Exception, RequestActionProcessor::Exception);

      struct Request: public ReferenceCounting::AtomicCopyImpl
      {
      public:
        enum Mode
        {
          RM_NORMAL,
          RM_RESEND,
          RM_FORCED
        };

      public:
        Request(
          const Generics::Time& time_val,
          unsigned long account_id_val,
          unsigned long advertiser_id_val,
          unsigned long campaign_id_val,
          unsigned long ccg_id_val,
          const RevenueDecimal& ctr_val,
          Mode mode_val,
          const RevenueDecimal& account_amount_val,
          const RevenueDecimal& amount_val,
          const RevenueDecimal& imps_val,
          const RevenueDecimal& clicks_val)
          throw();

        Request(
          const Generics::Time& rounded_time_val,
          unsigned long account_id_val,
          unsigned long advertiser_id_val,
          unsigned long campaign_id_val,
          unsigned long ccg_id_val,
          const RevenueDecimal& ctr_val,
          Mode mode_val,
          const Generics::Time& first_request_time_val,
          const RevenueDecimal& account_amount_val,
          const RevenueDecimal& amount_val,
          const RevenueDecimal& imps_val,
          const RevenueDecimal& clicks_val)
          throw();

        // return true of first_request_time changed
        bool
        add(const Request& request)
          throw(RevenueDecimal::Overflow);

        void
        print(
          std::ostream& out,
          const char* prefix = "")
          const throw();

      public:
        // aggregate key
        const Generics::Time rounded_time;
        const unsigned long account_id;
        const unsigned long advertiser_id;
        const unsigned long campaign_id;
        const unsigned long ccg_id;
        const RevenueDecimal ctr;
        // resend marker means that some server already refused request
        Mode mode;

        // aggregate fields
        Generics::Time first_request_time;
        RevenueDecimal account_amount;
        RevenueDecimal amount;
        RevenueDecimal imps;
        RevenueDecimal clicks;

      protected:
        virtual ~Request() throw() = default;
      };

      typedef ReferenceCounting::SmartPtr<Request> Request_var;

      typedef std::vector<Request_var> RequestArray;

      struct RequestSender: public virtual ReferenceCounting::AtomicImpl
      {
        DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
        DECLARE_EXCEPTION(ServerUnreachable, Exception);

        // method should remove success processed requests from array
        virtual void
        send_requests(
          RequestArray& requests,
          unsigned long service_index)
          throw(ServerUnreachable, Exception) = 0;

        virtual unsigned long
        server_count() const throw() = 0;
      };

      typedef ReferenceCounting::SmartPtr<RequestSender> RequestSender_var;

      class BillingServerRequestSender: public RequestSender
      {
      public:
        BillingServerRequestSender(
          const CORBACommons::CorbaObjectRefList& billing_server_refs)
          throw();

        virtual void
        send_requests(
          RequestArray& requests,
          unsigned long service_index)
          throw(ServerUnreachable, Exception);

        virtual unsigned long
        server_count() const throw();

      protected:
        struct BillingServerArrayHolder;

      protected:
        virtual
        ~BillingServerRequestSender() throw();

      protected:
        // pimpl
        std::unique_ptr<BillingServerArrayHolder> billing_servers_holder_;
      };

    public:
      BillingProcessor(
        Logging::Logger* logger,
        Generics::ActiveObjectCallback* callback,
        const String::SubString& storage_root,
        RequestSender* request_sender,
        unsigned long thread_count,
        const Generics::Time& dump_period,
        const Generics::Time& send_delayed_period)
        throw(Exception);

      // RequestActionProcessor interface
      virtual void
      process_request(
        const RequestInfo&,
        const ProcessingState&)
        throw(RequestActionProcessor::Exception)
      {};

      virtual void
      process_impression(
        const RequestInfo&,
        const ImpressionInfo&,
        const ProcessingState&)
        throw(RequestActionProcessor::Exception);

      virtual void
      process_click(
        const RequestInfo&,
        const ProcessingState&)
        throw(RequestActionProcessor::Exception);

      virtual void
      process_action(const RequestInfo&)
        throw(RequestActionProcessor::Exception)
      {};

      virtual void
      wait_object() throw();

    protected:
      struct RequestPool;

      typedef ReferenceCounting::SmartPtr<RequestPool> RequestPool_var;  

      class Sender;

    protected:
      virtual
      ~BillingProcessor() throw();

      Generics::Time
      dump_() throw();

      Generics::Time
      send_delayed_() throw();

      virtual RequestSender_var
      init_request_sender_(
        const CORBACommons::CorbaObjectRefList& billing_server_refs)
        throw();

      static void
      mark_resend_(RequestArray& requests)
        throw();

    protected:
      Logging::Logger_var logger_;
      const std::string storage_root_;
      const Generics::Time dump_period_;
      const Generics::Time send_delayed_period_;

      Generics::Planner_var scheduler_;
      Generics::TaskRunner_var task_runner_;
      RequestSender_var request_sender_;
      RequestPool_var request_pool_;
    };

    typedef ReferenceCounting::SmartPtr<BillingProcessor> BillingProcessor_var;
  }
}

#endif /*REQUESTINFOSVCS_REQUESTINFOMANAGER_BILLINGPROCESSOR_HPP*/
