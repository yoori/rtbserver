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

#ifndef CHANNEL_SVCS_CHANNEL_LOAD_SESSION_FACTORY_HPP_
#define CHANNEL_SVCS_CHANNEL_LOAD_SESSION_FACTORY_HPP_


#include <ReferenceCounting/ReferenceCounting.hpp>
#include <eh/Exception.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/TaskRunner.hpp>
#include <Logger/Logger.hpp>

#include <Generics/CompositeActiveObject.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ServantImpl.hpp>

/**
 * ChannelLoadSessionFactoryImpl
 * implementation of corba valuetype factory
 */
class ChannelLoadSessionFactoryImpl :
  public virtual Generics::CompositeActiveObject,
  public virtual CORBACommons::ReferenceCounting::CorbaRefCountImpl<
    CORBA::ValueFactoryBase>
{
public:
  ChannelLoadSessionFactoryImpl(
    unsigned long count_threads,
    Generics::ActiveObjectCallback* callback)
    throw();

  ~ChannelLoadSessionFactoryImpl() throw();

  virtual CORBA::ValueBase* create_for_unmarshal();

  virtual void report_error(
    Generics::ActiveObjectCallback::Severity severity,
    const char* description,
    const char* error_code = 0) throw();
private:
  Generics::ActiveObjectCallback_var callback_;
  Generics::TaskRunner_var task_runner_;
  static const char* ASPECT;
};

typedef ReferenceCounting::SmartPtr<ChannelLoadSessionFactoryImpl> 
  ChannelLoadSessionFactoryImpl_var;

#endif /*CHANNEL_SVCS_CHANNEL_LOAD_SESSION_FACTORY_HPP_*/
