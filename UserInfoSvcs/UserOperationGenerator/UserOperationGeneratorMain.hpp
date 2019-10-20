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

#ifndef USERINFOSVCS_USEROPERATIONGENERATOR_USEROPERATIONGENERATORMAIN_HPP_
#define USERINFOSVCS_USEROPERATIONGENERATOR_USEROPERATIONGENERATORMAIN_HPP_

#include <memory>

#include <eh/Exception.hpp>

#include <Commons/ProcessControlVarsImpl.hpp>

#include "UserOperationGeneratorImpl.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  class UserOperationGeneratorApp
    : public AdServer::Commons::ProcessControlVarsLoggerImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(InvalidArgument, Exception);

  public:
    UserOperationGeneratorApp() throw (eh::Exception);

    void
    main(
      int argc,
      char** argv)
      throw ();

  protected:
    virtual
    void
    shutdown(CORBA::Boolean wait_for_completion)
      throw(CORBA::SystemException);

  protected:


  protected:
    virtual
    ~UserOperationGeneratorApp() throw ()
    {};

  private:
    CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
    CORBACommons::CorbaConfig corba_config_;
    UserOperationGeneratorImpl::ConfigPtr config_;
    Logging::Logger_var logger_;

    UserOperationGeneratorImpl_var user_operation_generator_impl_;

    Sync::PosixMutex shutdown_lock_;
  };

  typedef ReferenceCounting::SmartPtr<UserOperationGeneratorApp>
    UserOperationGeneratorApp_var;

  typedef Generics::Singleton<
    UserOperationGeneratorApp, UserOperationGeneratorApp_var>
      UserOperationGeneratorSingleton;
}
}

#endif /* USERINFOSVCS_USEROPERATIONGENERATOR_USEROPERATIONGENERATORMAIN_HPP_ */
