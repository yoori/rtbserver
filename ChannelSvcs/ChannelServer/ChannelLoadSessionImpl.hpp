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

#ifndef AD_CHANNEL_SVCS_CHANNEL_LOAD_SESSION_IMPL_HPP_
#define AD_CHANNEL_SVCS_CHANNEL_LOAD_SESSION_IMPL_HPP_

#include <Generics/TaskRunner.hpp>
#include <ChannelSvcs/ChannelServer/ChannelUpdateBase.hpp>

namespace AdServer
{
namespace ChannelSvcs
{

  typedef ::AdServer::ChannelSvcs::ChannelUpdateBase_v33 ChannelCurrent;

  /**
   * ChannelLoadSessionImpl
   * implementation of ChannelLoadSession valuetype 
   */
  class ChannelLoadSessionImpl:
    public virtual OBV_AdServer::ChannelSvcs::ChannelLoadSession
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    ChannelLoadSessionImpl(
      Generics::ActiveObjectCallback* callback,
      Generics::TaskRunner* runner)
      throw();

    ChannelLoadSessionImpl(ChannelLoadSessionImpl& init)
      throw(eh::Exception, Exception);

    ChannelLoadSessionImpl(
      const ChannelSvcs::GroupLoadDescriptionSeq& servers,
      CORBA::ULong source)
      throw(Exception);

    virtual ~ChannelLoadSessionImpl() throw();
      
    virtual CORBA::ValueBase* _copy_value();

    //
    // IDL:AdServer/ChannelSvcs/ChannelCurrent/check:1.0
    //
    virtual void check(
      const ::AdServer::ChannelSvcs::ChannelCurrent::CheckQuery& query,
      ::AdServer::ChannelSvcs::ChannelCurrent::CheckData_out data) 
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_triggers:1.0
    //
    virtual void update_triggers(
        const ::AdServer::ChannelSvcs::ChannelIdSeq& ids,
        ::AdServer::ChannelSvcs::ChannelCurrent::UpdateData_out result)
        throw(AdServer::ChannelSvcs::ImplementationException,
              AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_all_ccg:1.0
    //
    virtual void update_all_ccg(
      const AdServer::ChannelSvcs::ChannelCurrent::CCGQuery& query_in,
      AdServer::ChannelSvcs::ChannelCurrent::PosCCGResult_out result)
      throw(AdServer::ChannelSvcs::ImplementationException,
            AdServer::ChannelSvcs::NotConfigured);

    //
    // IDL:AdServer/ChannelSvcs/ChannelProxy/get_count_chunks:1.0
    //
    ::CORBA::ULong get_count_chunks()
       throw(AdServer::ChannelSvcs::ImplementationException);

  private:
    size_t get_item_() throw();
    void bad_(size_t index) throw();

  private:
    typedef Sync::PosixRWLock Mutex_;
    typedef Sync::PosixRGuard RGuard_;
    typedef Sync::PosixWGuard WGuard_;

    Generics::TaskRunner_var task_runner_;
    Generics::ActiveObjectCallback_var callback_;
    Mutex_ init_lock_;
    size_t index_;
    static const char* ASPECT;
  };

  template<class STREAM>
  STREAM&
  describe_description(
    STREAM& ostr,
    const ChannelSvcs::GroupLoadDescriptionSeq& servers)
  {
    ostr << "servers:";
    for(size_t group_num = 0; group_num < servers.length(); group_num++)
    {
      for(size_t i = 0; i < servers[group_num].length(); i++)
      {
        ostr << ' ' << group_num << '.' << i;
      }
    }
    return ostr;
  }

} /* ChannelSvcs */
}
#endif //AD_CHANNEL_SVCS_CHANNEL_LOAD_SESSION_IMPL_HPP_
