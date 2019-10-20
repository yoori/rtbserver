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

#include <stdlib.h>
#include <iostream>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Time.hpp>
#include <Generics/TaskRunner.hpp>

class TaskCheck;

class GetTimeOfDayTest:
  public Generics::ActiveObjectCallback,
  public ReferenceCounting::AtomicImpl
{
public:
  GetTimeOfDayTest(long timeout, long count_threads) throw();
  int run() throw();
  void iteration(TaskCheck* task) throw();

  virtual void report_error(
    Severity severity,
    const String::SubString& description,
    const char* error_code = 0) throw ();
private:
  Generics::TaskRunner_var task_runner_;
  Generics::Time timeout_;
  long count_threads_;
  volatile sig_atomic_t iterations_;
};
typedef ReferenceCounting::SmartPtr<GetTimeOfDayTest> GetTimeOfDayTest_var;

class TaskCheck:
  public Generics::Task,
  public ReferenceCounting::AtomicImpl
{
public:
  TaskCheck(GetTimeOfDayTest* test) throw();
  void execute() throw (eh::Exception);
private:
  GetTimeOfDayTest* test_;
};
typedef ReferenceCounting::SmartPtr<TaskCheck> TaskCheck_var;

TaskCheck::TaskCheck(GetTimeOfDayTest* test) throw()
  : test_(test)
{
}

void TaskCheck::execute() throw (eh::Exception)
{
  Generics::Time::get_time_of_day();
  test_->iteration(this);
}

GetTimeOfDayTest::GetTimeOfDayTest(
  long timeout,
  long count_threads) throw()
  : task_runner_(new Generics::TaskRunner(this, count_threads)),
    timeout_(timeout),
    count_threads_(count_threads),
    iterations_(0)
{
}

int GetTimeOfDayTest::run() throw()
{
  task_runner_->activate_object();
  TaskCheck_var task = new TaskCheck(this);
  for(long i = 0; i < count_threads_; i++)
  {
    try
    {
      task_runner_->enqueue_task(task);
    }
    catch(const eh::Exception& e)
    {
      std::cerr << "Exception: " << e.what() << std::endl;
      return 1;
    }
  }
  sleep(timeout_.tv_sec);
  task_runner_->deactivate_object();
  std::cout << "Wait active threads" << std::endl;
  task_runner_->wait_object();
  task_runner_ = nullptr;
  std::cout << "Success " << iterations_  << " iterations" << std::endl;
  return 0;
}

void GetTimeOfDayTest::iteration(TaskCheck* task) throw()
{
  try
  {
    ++iterations_;
    task_runner_->enqueue_task(task);
  }
  catch(const Generics::TaskRunner::NotActive&)
  {//task_runner was deactivated ignore it
  }
  catch(const eh::Exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    exit(1);
  }
}

void GetTimeOfDayTest::report_error(
  Severity,
  const String::SubString& description,
  const char* error_code)
  throw ()
{
  std::cerr << "Error " << error_code << " :" << description.str();
}

int main(int argc, char* argv[])
{
  long timeout = 30;
  long count_threads = 1;
  if(argc > 1)
  {
    count_threads = atol(argv[1]);
    if(argc > 2)
    {
      timeout = atol(argv[2]);
    }
  }
  else
  {
    std::cerr << "Usage: GetTimeOfDayTest threads [time]" << std::endl;
    return 0;
  }
  GetTimeOfDayTest_var test = new GetTimeOfDayTest(timeout, count_threads);
  return test->run();
}

