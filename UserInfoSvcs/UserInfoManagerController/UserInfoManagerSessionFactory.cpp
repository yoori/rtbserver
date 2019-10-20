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

#include <string>

//#include <Commons/UserInfoManip.hpp>

//#include <UserInfoSvcs/UserInfoManager/UserInfoManager.hpp>
//#include <UserInfoSvcs/UserInfoManagerController/UserInfoManagerController.hpp>

#include "UserInfoManagerSessionFactory.hpp"
#include "UserInfoManagerSessionImpl.h"

/**
 * UserInfomanagerSessionFactoryImpl
 * implementation of corba valuetype factory
 */
class UserInfoManagerSessionFactoryImpl :
  public virtual CORBA::ValueFactoryBase
{
public:
  virtual CORBA::ValueBase* create_for_unmarshal();
};

/**
 * UserInfoManagerSessionFactoryImpl
 */
CORBA::ValueBase*
UserInfoManagerSessionFactoryImpl::create_for_unmarshal()
{
  return new AdServer::UserInfoSvcs::UserInfoManagerSessionImpl();
}

/**
 * UserInfoManagerSessionFactory
 */
namespace AdServer{
namespace UserInfoSvcs{

  void
  UserInfoManagerSessionFactory::init(
    const CORBACommons::CorbaClientAdapter& corba_client_adapter)
    throw(eh::Exception)
  {
    static const char* FUN = "UserInfoManagerSessionFactory::init()";

    /* register UserInfomanagerSessionFactoryImpl
       for creating Session valuetype
     */
    try
    {
      CORBA::ValueFactoryBase_var factory =
        new UserInfoManagerSessionFactoryImpl();

      corba_client_adapter.register_value_factory(
        UserInfoManagerSessionImpl::_tao_obv_static_repository_id(),
        factory);
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": Can't init UserInfoManagerSessionFactory: " <<
        ex.what();
      throw Exception(ostr);
    }
    catch(const CORBA::SystemException& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": Can't init UserInfoManagerSessionFactory. "
        "Caught CORBA::SystemException: " << e;
      throw Exception(ostr);
    }
  }

}
}

