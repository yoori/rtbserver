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


#ifndef _AD_SERVER_TEST_CHANNEL_UPDATE_IMPL_HPP_
#define _AD_SERVER_TEST_CHANNEL_UPDATE_IMPL_HPP_

#include <eh/Exception.hpp>
#include <CORBACommons/ServantImpl.hpp>
#include <CORBACommons/ProcessControlImpl.hpp>
#include <ChannelSvcs/ChannelServer/ChannelUpdateBase.hpp>
#include <ChannelSvcs/ChannelServer/ChannelUpdateBase_s.hpp>

namespace AdServer
{
namespace UnitTests
{
  typedef ::AdServer::ChannelSvcs::ChannelUpdateBase_v33 ChannelCurrent;

  class UpdateImitator:
    public virtual CORBACommons::ReferenceCounting::ServantImpl
      <POA_AdServer::ChannelSvcs::ChannelUpdate_v33>
  {
  public:
    enum Algorithm
    {
      ALG_RANDOM,
      ALG_CONST,
      ALG_GROW,
      ERROR
    };


    UpdateImitator(
      Algorithm alg,
      size_t count_channels,
      size_t count_triggers,
      size_t count_keywords,
      char trigger_type,
      size_t trigger_length,
      size_t start_point,
      size_t grow_step,
      bool dump_memory_stat)
      throw();

    //
    // IDL:AdServer/ChannelSvcs/ChannelCurrent/check:1.0
    //
    virtual void check(
      const ChannelCurrent::CheckQuery& query,
      ChannelCurrent::CheckData_out data)
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_triggers:1.0
    //
    virtual void update_triggers(
        const ChannelSvcs::ChannelIdSeq& ids,
        ChannelCurrent::UpdateData_out result)
        throw(AdServer::ChannelSvcs::ImplementationException,
              AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelProxy/get_count_chunks:1.0
    //
    virtual ::CORBA::ULong get_count_chunks()
      throw(AdServer::ChannelSvcs::ImplementationException);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_all_ccg:1.0
    //
    virtual void update_all_ccg(
      const ChannelCurrent::CCGQuery& query,
      ChannelCurrent::PosCCGResult_out result)
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    const std::string control(const char* param_name, const char* parma_value);


    static Algorithm parse_alg_name(const std::string& name) throw();

  protected:
    typedef Sync::PosixRWLock Mutex_;
    typedef Sync::PosixRGuard ReadGuard_;
    typedef Sync::PosixWGuard WriteGuard_;

    virtual ~UpdateImitator() throw();

    void generate_triggers_() throw();
  private:
    typedef std::map<unsigned int, std::string> TriggerMap;
    int alg_;
    size_t count_channels_;
    size_t count_triggers_;
    size_t count_keywords_;
    size_t trigger_length_;
    size_t start_point_;
    size_t grow_step_;
    char trigger_type_;
    size_t count_channels_i_;
    TriggerMap triggers_;
    bool dump_memory_stat_;

    mutable Mutex_ lock_triggers_;
  };

  typedef ReferenceCounting::SmartPtr<UpdateImitator> UpdateImitator_var;

  class ControlImpl: public CORBACommons::ProcessControlImpl
  {
  public:
    ControlImpl(
      CORBACommons::OrbShutdowner* shutdowner,
      UpdateImitator* server) throw()
      : CORBACommons::ProcessControlImpl(shutdowner),
        server_(ReferenceCounting::add_ref(server)) {};

    virtual
    char*
    control(const char* param_name, const char* param_value)
      throw (CORBACommons::OutOfMemory, CORBACommons::ImplementationError);

  protected:
    virtual ~ControlImpl() throw() {};

  private:
    UpdateImitator_var server_;
  };

  typedef ReferenceCounting::SmartPtr<ControlImpl> ControlImpl_var;

}
}
#endif //_AD_SERVER_TEST_CHANNEL_UPDATE_IMPL_HPP_

