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

#include <Commons/CorbaConfig.hpp>
#include <Commons/ErrorHandler.hpp>
#include <Commons/ConfigUtils.hpp>

#include "UserInfoManagerControllerMain.hpp"

namespace
{
  const char ASPECT[] = "UserInfoManagerController";
  const char USER_INFO_MANAGER_CONTROLLER_OBJ_KEY[] = "UserInfoManagerController";
  const char USER_INFO_CLUSTER_OBJ_KEY[] = "UserInfoClusterControl";
  const char USER_INFO_CLUSTER_STATS_OBJ_KEY[] = "UserInfoClusterStats";
  const char PROCESS_CONTROL_OBJ_KEY[] = "ProcessControl";
}

UserInfoManagerControllerApp_::UserInfoManagerControllerApp_()
  throw (eh::Exception)
  : AdServer::Commons::ProcessControlVarsLoggerImpl(
      "UserInfoManagerControllerApp_", ASPECT)
{
}

void
UserInfoManagerControllerApp_::shutdown(CORBA::Boolean wait_for_completion)
  throw(CORBA::SystemException)
{
  ShutdownGuard guard(shutdown_lock_);

  if(user_info_manager_controller_impl_.in() != 0)
  {
    user_info_manager_controller_impl_->deactivate_object();
    user_info_manager_controller_impl_->wait_object();
  }

  CORBACommons::ProcessControlImpl::shutdown(wait_for_completion);
}

CORBACommons::IProcessControl::ALIVE_STATUS
UserInfoManagerControllerApp_::is_alive() throw(CORBA::SystemException)
{
  return CORBACommons::ProcessControlImpl::is_alive();
}

void
UserInfoManagerControllerApp_::main(int& argc, char** argv)
  throw()
{
  const char FUN[] = "UserInfoManagerControllerApp_::main()";

  try
  {
    const char* usage = "usage: UserInfoManagerController <config_file>";

    if (argc < 2)
    {
      Stream::Error ostr;
      ostr << "config file is not specified\n" << usage;
      throw InvalidArgument(ostr);
    }

    Config::ErrorHandler error_handler;

    try
    {
      /* using xsd namespace */
      using namespace xsd::AdServer::Configuration;

      std::string file_name(argv[1]);

      std::unique_ptr<AdConfigurationType>
        ad_configuration = AdConfiguration(file_name.c_str(), error_handler);

      if(error_handler.has_errors())
      {
        std::string error_string;
        throw Exception(error_handler.text(error_string));
      }

      configuration_ =
        ConfigPtr(new UserInfoManagerControllerConfigType(
          ad_configuration->UserInfoManagerControllerConfig()));

    }
    catch(const xml_schema::parsing& e)
    {
      Stream::Error ostr;

      ostr << "Can't parse config file '"
        << argv[1] << "'."
        << ": ";

      if(error_handler.has_errors())
      {
        std::string error_string;
        ostr << error_handler.text(error_string);
      }

      throw Exception(ostr);
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << "Can't parse config file '"
           << argv[1] << "'."
           << ": "
           << e.what();
      throw Exception(ostr);
    }
    catch(...)
    {
      Stream::Error ostr;
      ostr << "Unknown Exception at parsing of config.";
      throw Exception(ostr);
    }

    // Initializing logger
    try
    {
      logger_ = Config::LoggerConfigReader::create(
        config().Logger(), argv[0]);
    }
    catch (const Config::LoggerConfigReader::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << "got LoggerConfigReader::Exception: " << e.what();
      throw Exception(ostr);
    }

    // fill corba_config
    try
    {
      Config::CorbaConfigReader::read_config(
        config().CorbaConfig(),
        corba_config_);
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << "Can't read Corba Config. : "
           << e.what();
      throw Exception(ostr);
    }

    try
    {
      // init CORBA Server
      corba_server_adapter_ =
        new CORBACommons::CorbaServerAdapter(corba_config_);

      shutdowner_ = corba_server_adapter_->shutdowner();
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << "Can't init CorbaServerAdapter. : "
           << e.what();
      throw Exception(ostr);
    }

    // Creating user info manager servant
    user_info_manager_controller_impl_ =
      new AdServer::UserInfoSvcs::UserInfoManagerControllerImpl(
        callback(),
        logger(),
        config());

    register_vars_controller();

    AdServer::UserInfoSvcs::UserInfoClusterImpl_var cluster_control =
      new AdServer::UserInfoSvcs::UserInfoClusterImpl(
        user_info_manager_controller_impl_.in());

    AdServer::UserInfoSvcs::UserInfoClusterStatsImpl_var cluster_stats =
      new AdServer::UserInfoSvcs::UserInfoClusterStatsImpl(
        user_info_manager_controller_impl_.in());

    corba_server_adapter_->add_binding(
      USER_INFO_MANAGER_CONTROLLER_OBJ_KEY,
      user_info_manager_controller_impl_.in());

    corba_server_adapter_->add_binding(
      USER_INFO_CLUSTER_OBJ_KEY,
      cluster_control.in());

    corba_server_adapter_->add_binding(
      USER_INFO_CLUSTER_STATS_OBJ_KEY,
      cluster_stats.in());

    user_info_manager_controller_impl_->activate_object();

    corba_server_adapter_->add_binding(
      PROCESS_CONTROL_OBJ_KEY, this);

    logger()->sstream(Logging::Logger::NOTICE, ASPECT) << "service started.";
    // Running orb loop
    corba_server_adapter_->run();

    wait();
    logger()->sstream(Logging::Logger::NOTICE, ASPECT) << "service stopped.";
  }
  catch (const Exception& e)
  {
    try
    {
      logger()->sstream(Logging::Logger::CRITICAL,
                        ASPECT,
                        "ADS-IMPL-73")
        << "UserInfoManagerApp_::main(): "
        << "Got UserInfoManagerControllerApp_::Exception. : \n"
        << e.what();
    }
    catch (...)
    {
      logger()->log(String::SubString(
                      "UserInfoManagerControllerApp_::main(): "
                      "Got UserInfoManagerControllerApp_::Exception."),
                    Logging::Logger::EMERGENCY,
                    ASPECT,
                    "ADS-IMPL-73");
    }
  }
  catch (const CORBA::SystemException& e)
  {
    try
    {
      logger()->sstream(Logging::Logger::EMERGENCY,
                        ASPECT,
                        "ADS-IMPL-73")
        << "UserInfoManagerControllerApp_::main(): "
        << "Got CORBA::SystemException. : \n"
        << e;
    }
    catch (...)
    {
      logger()->log(String::SubString(
                      "UserInfoManagerControllerApp_::main(): "
                      "Got CORBA::SystemException. : \n"),
                    Logging::Logger::EMERGENCY,
                    ASPECT,
                    "ADS-IMPL-73");
    }
  }
  catch (const eh::Exception& e)
  {
    try
    {
      logger()->sstream(Logging::Logger::EMERGENCY,
                        ASPECT,
                        "ADS-IMPL-73")
        << "UserInfoManagerControllerApp_::main(): "
        << "Got eh::Exception. : \n"
        << e.what();
    }
    catch (...)
    {
      logger()->log(String::SubString(
                      "UserInfoManagerControllerApp_::main(): "
                      "Got eh::Exception."),
                    Logging::Logger::EMERGENCY,
                    ASPECT,
                    "ADS-IMPL-73");
    }
  }
  catch (...)
  {
    logger()->log(String::SubString(
                    "UserInfoManagerControllerApp_::main(): "
                    "Got Unknown exception."),
                  Logging::Logger::EMERGENCY,
                  ASPECT,
                  "ADS-IMPL-73");
  }
}

int
main(int argc, char** argv)
{
  UserInfoManagerControllerApp_* app = 0;

  try
  {
    app = &UserInfoManagerControllerApp::instance();
  }
  catch (...)
  {
    std::cerr << "main(): Critical: Got exception while "
      "creating application object.\n";
    return -1;
  }

  if (app == 0)
  {
    std::cerr << "main(): Critical: got NULL application object.\n";
    return -1;
  }

  app->main(argc, argv);
}

