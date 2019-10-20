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

#include <signal.h>
#include "UnitManager.hpp"

class SignalHandler
{
  DECLARE_EXCEPTION(SignalException, eh::DescriptiveException);

public:
  /**
   * Constructor.
   * @param application
   */
  SignalHandler();
  
  /**
   * Destructor.
   */
  ~SignalHandler();
  
  /**
   * Sets exit signal to true.
   * @param _ignored Not used, but required by prototype to match required handler.
   */
  static void exitSignalHandler(int _ignored);

  /**
   * Set up the signal handlers for UnitManager.
   */
  void
  setupSignalHandlers();

};

SignalHandler::SignalHandler()
{ }

SignalHandler::~SignalHandler()
{ }

void SignalHandler::exitSignalHandler(int /*_ignored*/)
{
  AutoTest::Shutdown::instance().set();
}

void
SignalHandler::setupSignalHandlers()
{
  if (signal((int) SIGINT, SignalHandler::exitSignalHandler) == SIG_ERR)
  {
    throw SignalException("Error setting up signal handlers!");
  }
}
  

int
main(int argc, const char* argv[])
{
  int result = 2;
  
  try
  {
    SignalHandler signalHandler;
    signalHandler.setupSignalHandlers();

    UnitManager manager;
    manager.run(argc, argv);

    result = manager.succeed() ? 0 : 1;
  }
  catch(const UnitManager::InvalidArgument& e)
  {
    std::cerr << e.what() << std::endl;
  }
  catch(const UnitManager::Exception& e)
  {
    std::cerr << "main: UnitManager::Exception exception caught. "
      ":" << std::endl << e.what() << std::endl;
  }
  catch(const eh::Exception& e)
  {
    std::cerr << "main: eh::Exception exception caught. "
      ":" << std::endl << e.what() << std::endl;    
  }

  return result;
}
