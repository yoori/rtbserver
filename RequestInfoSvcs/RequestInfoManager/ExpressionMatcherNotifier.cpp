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

#include <RequestInfoSvcs/RequestInfoCommons/ConsiderMessages.hpp>
#include "ExpressionMatcherNotifier.hpp"

namespace Aspect
{
  const char EXPRESSION_MATCHER_NOTIFIER[] = "ExpressionMatcherNotifier";
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  ExpressionMatcherNotifier::ExpressionMatcherNotifier(
    Logging::Logger* logger,
    bool notify_impressions,
    bool notify_revenue,
    unsigned long distrib_count,
    const LogProcessing::LogFlushTraits& action_saver_opts,
    const LogProcessing::LogFlushTraits& click_saver_opts,
    const LogProcessing::LogFlushTraits& impression_saver_opts,
    const LogProcessing::LogFlushTraits& request_saver_opts)
    throw()
    : logger_(ReferenceCounting::add_ref(logger)),
      notify_impressions_(notify_impressions),
      notify_revenue_(notify_revenue)
  {
    action_saver_ = new ProfilingCommons::MessageSaver(
      logger,
      action_saver_opts.out_dir.c_str(),
      "ConsiderAction",
      distrib_count,
      action_saver_opts.period);
    add_child_object(action_saver_);

    click_saver_ = new ProfilingCommons::MessageSaver(
      logger,
      click_saver_opts.out_dir.c_str(),
      "ConsiderClick",
      distrib_count,
      click_saver_opts.period);
    add_child_object(click_saver_);

    impression_saver_ = new ProfilingCommons::MessageSaver(
      logger,
      impression_saver_opts.out_dir.c_str(),
      "ConsiderImpression",
      distrib_count,
      impression_saver_opts.period);
    add_child_object(impression_saver_);

    request_saver_ = new ProfilingCommons::MessageSaver(
      logger,
      request_saver_opts.out_dir.c_str(),
      "ConsiderRequest",
      distrib_count,
      request_saver_opts.period);
    add_child_object(request_saver_);
  }

  void
  ExpressionMatcherNotifier::process_impression(
    const RequestInfo& ri,
    const ImpressionInfo&,
    const ProcessingState& processing_state)
    throw(Exception)
  {
    static const char* FUN = "ExpressionMatcherNotifier::process_impression()";

    if(!need_process_(ri, processing_state) || !notify_impressions_)
    {
      return;
    }

    try
    {
      ConsiderImpressionWriter writer;
      writer.version() = 0;
      writer.user_id() = ri.user_id.to_string();
      writer.request_id() = ri.request_id.to_string();
      writer.time() = ri.time.tv_sec;
      writer.revenue() = ri.adv_revenue.sys_impression().str();

      std::copy (
        ri.channels.begin(), ri.channels.end(),
        std::back_inserter(writer.channels()));

      Generics::SmartMemBuf_var op_mem_buf(new Generics::SmartMemBuf(writer.size()));
      writer.save(op_mem_buf->membuf().data(), op_mem_buf->membuf().size());

      impression_saver_->write(
        AdServer::Commons::uuid_distribution_hash(ri.user_id),
        op_mem_buf->membuf());
    }
    catch (eh::Exception& ex)
    {
      logger_->stream(Logging::Logger::EMERGENCY,
        Aspect::EXPRESSION_MATCHER_NOTIFIER,
        "ADS-IMPL-3023") << FUN <<
        ": Exception caught: " << ex.what();
    }
  }

  void
  ExpressionMatcherNotifier::process_click(
    const RequestInfo& ri,
    const ProcessingState& processing_state)
    throw(Exception)
  {
    static const char* FUN = "ExpressionMatcherNotifier::process_click()";

    if(!need_process_(ri, processing_state) || !notify_revenue_)
    {
      return;
    }

    try
    {
      ConsiderClickWriter writer;
      writer.version() = 0;
      writer.user_id() = ri.user_id.to_string();
      writer.request_id() = ri.request_id.to_string();
      writer.time() = ri.time.tv_sec;
      writer.revenue() = ri.adv_revenue.sys_click().str();

      Generics::SmartMemBuf_var op_mem_buf(new Generics::SmartMemBuf(writer.size()));
      writer.save(op_mem_buf->membuf().data(), op_mem_buf->membuf().size());

      click_saver_->write(
        AdServer::Commons::uuid_distribution_hash(ri.user_id),
        op_mem_buf->membuf());
    }
    catch (eh::Exception& ex)
    {
      logger_->stream(Logging::Logger::EMERGENCY,
        Aspect::EXPRESSION_MATCHER_NOTIFIER,
        "ADS-IMPL-3023") << FUN <<
        ": Exception caught: " << ex.what();
    }
  }

  void
  ExpressionMatcherNotifier::process_action(
    const RequestInfo& ri) throw(Exception)
  {
    static const char* FUN = "ExpressionMatcherNotifier::process_action()";

    if(!need_process_(ri, ProcessingState(RequestInfo::RS_NORMAL)) || !notify_revenue_)
    {
      return;
    }

    try
    {
      ConsiderActionWriter writer;
      writer.version() = 0;
      writer.user_id() = ri.user_id.to_string();
      writer.time() = ri.time.tv_sec;
      writer.revenue() = ri.adv_revenue.sys_action().str();

      Generics::SmartMemBuf_var op_mem_buf(new Generics::SmartMemBuf(writer.size()));
      writer.save(op_mem_buf->membuf().data(), op_mem_buf->membuf().size());

      action_saver_->write(
        AdServer::Commons::uuid_distribution_hash(ri.user_id),
        op_mem_buf->membuf());
    }
    catch (eh::Exception& ex)
    {
      logger_->stream(Logging::Logger::EMERGENCY,
        Aspect::EXPRESSION_MATCHER_NOTIFIER,
        "ADS-IMPL-3023") << FUN <<
        ": Exception caught: " << ex.what();
    }
  }

  bool
  ExpressionMatcherNotifier::need_process_(
    const RequestInfo& ri,
    const ProcessingState& processing_state) throw()
  {
    // currently we increment user cost by test requests (ri.test_request)
    return !ri.user_id.is_null() &&
      processing_state.state == RequestInfo::RS_NORMAL;
  }
}
}

