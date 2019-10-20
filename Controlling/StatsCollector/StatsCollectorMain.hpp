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

#include<eh/Exception.hpp>
#include<CORBACommons/ProcessControl.hpp>
#include<Commons/ProcessControlVarsImpl.hpp>
#include<CORBACommons/CorbaAdapters.hpp>
#include<SNMPAgent/SNMPAgentX.hpp>
#include<xsd/Controlling/StatsCollectorConfig.hpp>
#include"StatsCollectorImpl.hpp"

namespace AdServer
{
  namespace Controlling
  {
    class StatsCollectorApp :
      public AdServer::Commons::ProcessControlVarsLoggerImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      StatsCollectorApp() throw(eh::Exception);

      virtual void shutdown(CORBA::Boolean wait_for_completion)
        throw(CORBA::SystemException);

      virtual CORBACommons::IProcessControl::ALIVE_STATUS
      is_alive() throw(CORBA::SystemException);

      int run(int argc, char* argv[]) throw();

      virtual bool
      is_ready_() throw ();

      virtual char*
      comment() throw (CORBACommons::OutOfMemory);

    protected:
      virtual ~StatsCollectorApp() throw(){};

    private:
      typedef std::unique_ptr<xsd::AdServer::Configuration::
        StatsCollectorConfigType> ConfigPtr;

      void load_config_(const char* name) throw(Exception);

      void init_logger_(const char* name) throw(Exception);

      void init_corba_() throw(Exception);

    private:
      CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
      CORBACommons::CorbaConfig corba_config_;

      ConfigPtr configuration_;
      StatCollectorImpl_var collector_impl_;

      typedef Sync::PosixMutex ShutdownMutex;
      typedef Sync::PosixGuard ShutdownGuard;

      ShutdownMutex shutdown_lock_;
    };
    typedef ReferenceCounting::SmartPtr<StatsCollectorApp>
      StatsCollectorApp_var;
  }
}

namespace AdServer
{
  namespace Controlling
  {
    inline
    bool StatsCollectorApp::is_ready_() throw()
    {
      if(collector_impl_.in())
      {
        return collector_impl_->ready();
      }
      else
      {
        return false;
      }
    }

    inline
    char* StatsCollectorApp::comment() throw (CORBACommons::OutOfMemory)
    {
      if(collector_impl_.in())
      {
        return collector_impl_->comment();
      }
      else
      {
        return 0;
      }
    }

    inline
    CORBACommons::IProcessControl::ALIVE_STATUS
    StatsCollectorApp::is_alive() throw(CORBA::SystemException)
    {
      ShutdownGuard guard(shutdown_lock_);
      return CORBACommons::ProcessControlImpl::is_alive();
    }

  }
}
