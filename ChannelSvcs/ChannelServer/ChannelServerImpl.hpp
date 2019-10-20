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

#ifndef AD_SERVER_CHANNEL_SERVER_IMPL_HPP_
#define AD_SERVER_CHANNEL_SERVER_IMPL_HPP_

#include <set>
#include <vector>
#include <map>
#include <malloc.h>

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Logger/Logger.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/Time.hpp>
#include <Language/SegmentorCommons/SegmentorInterface.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ServantImpl.hpp>
#include <CORBACommons/ProcessControl.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <ChannelSvcs/ChannelServer/ChannelServer_s.hpp>
#include <ChannelSvcs/ChannelServer/ChannelServerVariant.hpp>
#include <ChannelSvcs/ChannelCommons/ChannelUtils.hpp>
#include <ChannelSvcs/ChannelCommons/ChannelUpdateStatLogger.hpp>
#include <ChannelSvcs/ChannelServer/ChannelUpdateBase.hpp>

#include <xsd/AdServerCommons/AdServerCommons.hpp>
#include <xsd/ChannelSvcs/ChannelServerConfig.hpp>

#include "ChannelContainer.hpp"
#include "UpdateContainer.hpp"
#include "DictionaryMatcher.hpp"

namespace AdServer
{
namespace ChannelSvcs
{

  typedef ::AdServer::ChannelSvcs::ChannelUpdateBase_v33 ChannelCurrent;

  class UpdateData;
  /**
   * Implementation of common part ChannelServer
   */
  class ChannelServerCustomImpl:
    public virtual Generics::CompositeActiveObject,
    public virtual CORBACommons::ReferenceCounting::ServantImpl
      <POA_AdServer::ChannelSvcs::ChannelServer>
  {

  public:

    //
    // IDL:AdServer/ChannelSvcs/ChannelCurrent/check:1.0
    //
    virtual void check(
      const ::AdServer::ChannelSvcs::ChannelCurrent::CheckQuery& query,
      ::AdServer::ChannelSvcs::ChannelCurrent::CheckData_out data)
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServer/match:1.0
    //
    virtual void match(
        const ::AdServer::ChannelSvcs::ChannelServerBase::MatchQuery& query,
        ::AdServer::ChannelSvcs::ChannelServer::MatchResult_out result)
        throw(AdServer::ChannelSvcs::ImplementationException,
              AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServer/match:1.0
    //
    virtual void get_ccg_traits(
        const ::AdServer::ChannelSvcs::ChannelIdSeq& query,
        ::AdServer::ChannelSvcs::ChannelServer::TraitsResult_out result)
        throw(AdServer::ChannelSvcs::ImplementationException,
              AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/set_sources:1.0
    //
    virtual void set_sources(const ::AdServer::ChannelSvcs::
        ChannelServerControl::DBSourceInfo& db_info,
        const ::AdServer::ChannelSvcs::ChunkKeySeq& sources)
        throw(AdServer::ChannelSvcs::ImplementationException);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/set_proxy_sources:1.0
    //
    virtual void set_proxy_sources(
        const ::AdServer::ChannelSvcs::ChannelServerControl::ProxySourceInfo&
          poxy_info,
        const ::AdServer::ChannelSvcs::ChunkKeySeq& sources)
        throw(AdServer::ChannelSvcs::ImplementationException);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/check_configuration:1.0
    //
    ::CORBA::ULong check_configuration() throw();

    //
    // IDL:AdServer/ChannelSvcs/ChannelProxy/get_count_chunks:1.0
    //
    ::CORBA::ULong get_count_chunks()
      throw(AdServer::ChannelSvcs::ImplementationException);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_triggers:1.0
    //
    void update_triggers(
      const ::AdServer::ChannelSvcs::ChannelIdSeq& ids,
      ::AdServer::ChannelSvcs::ChannelCurrent::UpdateData_out result)
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    virtual void update_all_ccg(
      const AdServer::ChannelSvcs::ChannelCurrent::CCGQuery& query,
      AdServer::ChannelSvcs::ChannelCurrent::PosCCGResult_out result)
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    typedef xsd::AdServer::Configuration::ChannelServerConfigType
      ChannelServerConfig;

    ChannelServerCustomImpl(
      Logging::Logger* logger,
      const ChannelServerConfig* server_config) throw(Exception);

    virtual void
    deactivate_object() throw (Exception, eh::Exception);

  protected:
    virtual ~ChannelServerCustomImpl() throw();
  public:

    bool ready() throw();

    char* comment() throw(CORBACommons::OutOfMemory);

    void get_stats(ChannelServerStats& stats) throw();

    void change_db_state(bool new_state) throw(eh::Exception);

    bool get_db_state() throw(Commons::DbStateChanger::NotSupported);

    static const char* TRAFFICKING_PROBLEM;

  protected:

    typedef Generics::TaskGoal TaskBase;
    typedef ReferenceCounting::SmartPtr<TaskBase> Task_var;

    class UpdateTask: public TaskBase
    {
    public:

      UpdateTask(
          ChannelServerCustomImpl* server_impl,
          UpdateData* data,
          Generics::TaskRunner* task_runner) throw();

      virtual void execute() throw();

    private:
      virtual ~UpdateTask() throw();

      ChannelServerCustomImpl* server_impl_;
      UpdateData_var update_data_;
    };

    void schedule_update_task_(unsigned long period, UpdateData* update_data)
      throw ();

    void update_task(UpdateData* update_data)
      throw();

    UpdateContainer* get_update_container() throw();

    class CCGKeywordPacker:
      public std::unary_function<
      CCGKeyword, ChannelServerBase::CCGKeyword>
    {
    public:
      CCGKeywordPacker(
        size_t count[2],
        ChannelServerBase::CCGKeyword* ccg_out,
        CORBA::ULong* neg_out = 0)
        throw(eh::Exception);

      void operator() (const argument_type& id) throw();

      template<class RESULT>
      static RESULT convert(const CCGKeyword& in) throw();

    private:
      size_t* count_;
      ChannelServerBase::CCGKeyword* out_;
      CORBA::ULong* neg_out_;
    };

    template<class MAPIN, class CORBASEQ>
    class DeletedPacker:
      public std::unary_function<
        typename MAPIN::value_type,
        typename CORBASEQ::value_type>
    {
    public:
      DeletedPacker(const MAPIN& deleted, CORBASEQ& out)
        throw(eh::Exception);
      typename DeletedPacker::result_type operator()(
        const typename DeletedPacker::argument_type& in)
        throw(eh::Exception);
    private:
      CORBASEQ& out_;
    };

  private:

    Logging::Logger* logger () throw ();

    static size_t add_channels_(
      unsigned int channel_id,
      const TriggerMatchItem::value_type& in,
      AdServer::ChannelSvcs::ChannelServerBase::ChannelAtom* out)
      throw();

    void fill_result_(
      const TriggerMatchRes& result,
      AdServer::ChannelSvcs::ChannelServer::MatchResult& res,
      bool fill_content)
      throw();

    void unpack_ref_list_(
      const ::AdServer::ChannelSvcs::
        ChannelServerControl::CorbaObjectRefDefSeq& corba_refs,
      ChannelServerVariantBase::ServerPoolConfig& pool_config,
      const char* name)
    throw(eh::Exception, CORBACommons::CorbaObjectRef::Exception);

    void init_logger_(const ChannelServerConfig* config) throw();

    void log_update_() throw();

    void init_update_(
      UpdateData* update_data,
      ChannelServerVariantBase* variant_server)
      throw(
        ChannelServerException::TemporyUnavailable,
        ChannelServerException::NotReady,
        ChannelServerException::Exception,
        eh::Exception);

    bool change_source_of_data_() throw();

    ChannelServerVariantBase_var get_source_of_data_() throw();

    void trace_ccg_info_changing_(const UpdateData* update_data)
      throw();

    static void log_parsed_input_(
      const MatchUrls& url_words,
      const MatchWords match_words[CT_MAX],
      const StringVector& exact_words,
      std::ostream& ostr)
      throw(eh::Exception);

    static void log_result_(
      const TriggerMatchRes& res,
      const Generics::Time& time,
      std::ostream& ostr)
      throw(eh::Exception);

  protected:
    Logging::ActiveObjectCallbackImpl_var callback_;
    Logging::Logger_var statistic_logger_;
    std::unique_ptr<DictionaryMatcher> dict_matcher_;
    std::unique_ptr<UpdateContainer> update_cont_;

    typedef Sync::PosixRWLock Mutex_;
    typedef Sync::PosixRGuard ReadGuard_;
    typedef Sync::PosixWGuard WriteGuard_;

  private:

    mutable Mutex_ lock_set_sources_;
    CORBACommons::CorbaClientAdapter_var c_adapter_;
    ChannelUpdateStatLogger_var update_stat_logger_;
    std::string version_;
    unsigned long colo_;
    volatile sig_atomic_t ready_;
    volatile sig_atomic_t source_id_;
    ProgressCounterPtr progress_; /* progress of current operation */
    volatile sig_atomic_t load_keywords_;/* count of loaded keywords*/
    volatile sig_atomic_t state_; /* state of update */
    Generics::Planner_var scheduler_;
    Generics::TaskRunner_var task_runner_;
    SegmMap segmentors_;
    Language::Segmentor::SegmentorInterface_var china_segmentor_;
    Language::Segmentor::SegmentorInterface_var normal_segmentor_;
    const Language::Segmentor::SegmentorInterface* segmentor_;

    /* common params for ChannelServer's */
    const unsigned SERVICE_INDEX_; // The number to query campaign server
    unsigned long update_period_;//update period of data
    unsigned long reschedule_period_;// reschedule period
    ChannelServerVariantBase_var variant_server_;//backend of data server
    ChannelServerVariantBase_var new_variant_server_;//new backend of data server
    unsigned long count_chunks_;//count chunks in cluster
    unsigned long count_container_chunks_;//count chunks in container
    unsigned long merge_limit_;//max size of merge data in bytes
    unsigned long ccg_limit_;//maximum count keyword in one call
    std::unique_ptr<ChannelContainer> container_;
    //container of triggers and matched channels
    std::set<unsigned short> ports_;//allowed ports in urls

    /* actual channels */
    volatile _Atomic_word queries_counter_;//counter of queries to server
    Generics::Time configuration_date_;

    static const char* ASPECT;
    static const char* MATCHASPECT;
  };

  typedef ReferenceCounting::SmartPtr<ChannelServerCustomImpl>
      ChannelServerCustomImpl_var;

} /* ChannelSvcs */
} /* AdServer */

namespace AdServer
{
  namespace ChannelSvcs
  {
    inline
    Logging::Logger*
    ChannelServerCustomImpl::logger ()
      throw ()
    {
      return callback_->logger();
    }

    /* ChannelServerCustomImpl */
    inline
    bool ChannelServerCustomImpl::ready() throw()
    {
      return ready_;
    }

    inline
    ChannelServerVariantBase_var ChannelServerCustomImpl::get_source_of_data_()
    throw()
    {
      ReadGuard_ lock(lock_set_sources_);
      return variant_server_;
    }

    inline
    UpdateContainer* ChannelServerCustomImpl::get_update_container()
      throw()
    {
      return update_cont_.get();
    }

    inline
    ChannelServerCustomImpl::UpdateTask::UpdateTask(
      ChannelServerCustomImpl* server_impl,
      UpdateData* data,
      Generics::TaskRunner* task_runner) throw()
      : TaskBase(task_runner), server_impl_(server_impl)
    {
      if(data)
      {
        update_data_ = ReferenceCounting::add_ref(data);
      }
      else
      {
        update_data_ = new UpdateData(
          server_impl_->get_update_container());
      }
    }

    inline
    ChannelServerCustomImpl::UpdateTask::~UpdateTask() throw()
    {
    }

  } /* ChannelSvcs */
} /* AdServer */


#endif /*AD_SERVER_CHANNEL_SERVER_IMPL_HPP_*/
