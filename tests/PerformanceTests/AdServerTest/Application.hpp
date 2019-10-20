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

#ifndef __APPLICATION_HPP
#define __APPLICATION_HPP

#include <signal.h>
#include <ace/Event_Handler.h>
#include "QuerySender.hpp"
#include <Logger/StreamLogger.hpp>
#include <Logger/DistributorLogger.hpp>

class Application;

class SignalHandler : public ACE_Event_Handler // FIXME Get rid of ACE
{
public:
  SignalHandler(Application* app);

  int handle_signal (int signum, siginfo_t *, ucontext_t * );

private:
  Application* app_;
};

class Application :
  public Generics::ActiveObjectCallback,
  public ReferenceCounting::AtomicImpl
{

public:
  Application(unsigned long log_level,
              const char* cfg_file_path);

  ~Application() throw();


  void init() throw(eh::Exception);

  void run();

  void shutdown();

  // from ActiveObjectCallback
  virtual void report_error(
    Generics::ActiveObjectCallback::Severity severity,
    const String::SubString& description, const char* error_code = 0) throw();

private:

  void _stop();
  void _log_execution_time();

  unsigned long log_level_;
  Configuration config_;
  Logging::Logger_var logger_;
  SignalHandler sig_handler_;
  Generics::TaskRunner_var task_runner_;
  Generics::Planner_var scheduler_;
  HTTP::PoolPolicy_var http_pool_policy_;
  HTTP::HttpActiveInterface_var http_pool_;
  QuerySender* sender_;
};


#endif  // __APPLICATION_HPP
