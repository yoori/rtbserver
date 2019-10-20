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


#include <eh/Exception.hpp>

#include <Commons/ProcessControlVarsImpl.hpp>
#include <CORBACommons/StatsImpl.hpp>

#include <Commons/CorbaConfig.hpp>
#include <Commons/ErrorHandler.hpp>
#include <Commons/ConfigUtils.hpp>

#include "LogGeneralizerMain.hpp"
#include "ErrorCode.hpp"

namespace
{
  const char ASPECT[] = "LogGeneralizer";
  const char LOG_GENERALIZER_OBJ_KEY[] = "LogGeneralizer";
  const char PROCESS_CONTROL_OBJ_KEY[] = "ProcessControl";
  const char PROCESS_STATS_CONTROL_OBJ_KEY[] = "ProcessStatsControl";
}

LogGeneralizerApp_::LogGeneralizerApp_() throw (eh::Exception)
:
  AdServer::Commons::ProcessControlVarsLoggerImpl(
    "LogGeneralizerApp_", ASPECT)
{
}

void
LogGeneralizerApp_::shutdown(CORBA::Boolean wait_for_completion)
  throw(CORBA::SystemException)
{
  ShutdownGuard guard(shutdown_lock_);

  if (log_generalizer_impl_.in())
  {
    log_generalizer_impl_->deactivate_object();
    log_generalizer_impl_->wait_object();
  }

  CORBACommons::ProcessControlImpl::shutdown(wait_for_completion);
}

CORBACommons::IProcessControl::ALIVE_STATUS
LogGeneralizerApp_::is_alive() throw(CORBA::SystemException)
{
  return CORBACommons::ProcessControlImpl::is_alive();
}

void
LogGeneralizerApp_::main(int &argc, char **argv)
  throw()
{
  try
  {
    const char *usage = "usage: LogGeneralizer <config_file>";

    if (argc < 2)
    {
      Stream::Error es;
      es << "config file is not specified\n" << usage;
      throw InvalidArgument(es);
    }

    Config::ErrorHandler error_handler;

    try
    {
      /* using xsd namespace */
      using namespace xsd::AdServer::Configuration;

      std::string file_name(argv[1]);

      std::unique_ptr<AdConfigurationType>
        ad_configuration = AdConfiguration(file_name.c_str(), error_handler);

      if (error_handler.has_errors())
      {
        std::string error_string;
        throw Exception(error_handler.text(error_string));
      }

      configuration_ =
        ConfigPtr(
          new LogGeneralizerConfigType(
            ad_configuration->LogGeneralizerConfig()
          )
        );
    }
    catch (const xml_schema::parsing &ex)
    {
      Stream::Error es;

      es << "Can't parse config file '"
         << argv[1] << "'. : ";

      if (error_handler.has_errors())
      {
        std::string error_string;
        es << error_handler.text(error_string);
      }

      throw Exception(es);
    }
    catch (const eh::Exception &ex)
    {
      Stream::Error es;
      es << "Can't parse config file '"
         << argv[1] << "'. : " << ex.what();
      throw Exception(es);
    }

    // Initializing logger
    try
    {
      logger(Config::LoggerConfigReader::create(config().Logger(), argv[0]));
    }
    catch (const Config::LoggerConfigReader::Exception &ex)
    {
      Stream::Error es;
      es << "got LoggerConfigReader::Exception: " << ex.what();
      throw Exception(es);
    }

    // fill corba_config
    try
    {
      Config::CorbaConfigReader::read_config(config().CorbaConfig(),
        corba_config_);
    }
    catch (const eh::Exception &ex)
    {
      Stream::Error es;
      es << "Can't read Corba Config. : " << ex.what();
      throw Exception(es);
    }

    try
    {
      // init CORBA Server
      corba_server_adapter_ =
        new CORBACommons::CorbaServerAdapter(corba_config_);

      shutdowner_ = corba_server_adapter_->shutdowner();
    }
    catch (const eh::Exception &ex)
    {
      Stream::Error es;
      es << "Can't init CorbaServerAdapter. : " << ex.what();
      throw Exception(es);
    }

    proc_stat_impl_ = new AdServer::LogProcessing::ProcStatImpl;

    proc_stat_ctrl_ =
      new CORBACommons::ProcessStatsImpl(proc_stat_impl_);

    log_generalizer_impl_ =
      new AdServer::LogProcessing::LogGeneralizerImpl(callback(), logger(),
        config(), proc_stat_impl_);

    {
      using namespace AdServer::Commons;
      register_vars_controller();
      add_var_processor(
        DbStateProcessor::VAR_NAME,
        new DbStateProcessor(
          new_simple_db_state_changer(log_generalizer_impl_)
        )
      );
    }

    if (config().SNMPConfig().present())
    {
      try
      {
        unsigned snmp_index = config().SNMPConfig().get().index().present() ?
                                config().SNMPConfig().get().index().get() :
                                  getpid();

        proc_stat_impl_->set("index", static_cast<unsigned long>(snmp_index));

        snmp_stat_impl_ =
          new SNMPAgentX::SNMPStatsImpl(
            proc_stat_impl_, snmp_index,
            Logging::Logger_var(new Logging::LoggerDefaultHolder(
              logger(), 0, "ADS-IMPL-1012")),
            "",
            "LogGeneralizer-MIB:logGeneralizer",
            config().SNMPConfig().get().mib_dirs().c_str()
  //          , "/home/alexandr_kashkirov/tmp/agentX"
          );
      }
      catch (const eh::Exception &ex)
      {
        logger()->sstream(
          Logging::Logger::ERROR,
          ASPECT,
          AdServer::LogProcessing::LOG_GEN_IMPL_ERR_CODE_10
        ) << "LogGeneralizerApp_::main(): "
          "Got eh::Exception during initialization of SNMP stats. "
          ": " << ex.what();
      }
    }

    corba_server_adapter_->add_binding(PROCESS_CONTROL_OBJ_KEY, this);

    corba_server_adapter_->add_binding(PROCESS_STATS_CONTROL_OBJ_KEY,
      proc_stat_ctrl_.in());

    corba_server_adapter_->add_binding(LOG_GENERALIZER_OBJ_KEY,
      log_generalizer_impl_.in());

    log_generalizer_impl_->activate_object();

    logger()->sstream(Logging::Logger::NOTICE, ASPECT) << "service started.";
    // Running orb loop
    corba_server_adapter_->run();

    wait();
    logger()->sstream(Logging::Logger::NOTICE, ASPECT) << "service stopped.";

  }
  catch (const Exception &ex)
  {
    logger()->sstream(
      Logging::Logger::CRITICAL,
      ASPECT,
      AdServer::LogProcessing::LOG_GEN_IMPL_ERR_CODE_0
    ) << "LogGeneralizerApp_::main(): Got LogGeneralizerApp_::Exception: "
      << ex.what();
  }
  catch (const CORBA::SystemException &ex)
  {
    logger()->sstream(
      Logging::Logger::EMERGENCY,
      ASPECT,
      AdServer::LogProcessing::LOG_GEN_IMPL_ERR_CODE_0
    ) << "LogGeneralizerApp_::main(): Got CORBA::SystemException: "
      << ex;
  }
  catch (const eh::Exception &ex)
  {
    logger()->sstream(
      Logging::Logger::EMERGENCY,
      ASPECT,
      AdServer::LogProcessing::LOG_GEN_IMPL_ERR_CODE_0
    ) << "LogGeneralizerApp_::main(): Got eh::Exception: "
      << ex.what();
  }

  /* references to servants in ORB must be destroyed before
     destroying var pointers to them */
  try
  {
    corba_server_adapter_.reset();
    shutdowner_.reset();
  }
  catch (const CORBA::Exception &ex)
  {
    logger()->sstream(
      Logging::Logger::EMERGENCY,
      ASPECT,
      AdServer::LogProcessing::LOG_GEN_IMPL_ERR_CODE_1
    ) << "LogGeneralizerApp_::main(): Got CORBA::Exception while trying to "
         "destroy ORB: " << ex;
  }
}

int
main(int argc, char **argv) throw ()
{
  LogGeneralizerApp_ *app = 0;

  try
  {
    app = &LogGeneralizerApp::instance();
  }
  catch (...)
  {
    std::cerr
      << "main(): Critical: Got exception while creating application object.\n";
    return -1;
  }

  if (app == 0)
  {
    std::cerr << "main(): Critical: got NULL application object.\n";
    return -1;
  }

  app->main(argc, argv);
}

