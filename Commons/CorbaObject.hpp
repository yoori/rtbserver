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

#ifndef COMMONS_CORBAOBJECT_HPP
#define COMMONS_CORBAOBJECT_HPP

#include <eh/Exception.hpp>
#include <Sync/SyncPolicy.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

#include <CORBACommons/CorbaAdapters.hpp>

namespace AdServer
{
namespace Commons
{
  template<typename ObjectType>
  class CorbaObject
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(NullPointer, Exception);

    // not thread safe
    typedef TAO_Objref_Var_T<ObjectType> ObjectType_var;

  public:
    CorbaObject() throw();

    CorbaObject(
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      const CORBACommons::CorbaObjectRef& object_ref)
      throw();

    ObjectType* operator->() throw(NullPointer, Exception);

    const CORBACommons::CorbaObjectRef&
    ref() const
      throw(NullPointer);

    ObjectType_var
    get() const throw();

  private:
    typedef Sync::Policy::PosixThread SyncPolicy;

    class Holder: public ReferenceCounting::AtomicImpl
    {
    public:
      Holder(
        const CORBACommons::CorbaClientAdapter* corba_client_adapter_val,
        const CORBACommons::CorbaObjectRef& object_ref_val)
        throw();

      const CORBACommons::CorbaClientAdapter_var corba_client_adapter;
      const CORBACommons::CorbaObjectRef object_ref;
      mutable SyncPolicy::Mutex lock;
      ObjectType_var object;

    protected:
      virtual ~Holder() throw() {};
    };

    typedef ReferenceCounting::SmartPtr<Holder> Holder_var;

  private:
    Holder_var holder_;
  };
}
}

namespace AdServer
{
namespace Commons
{
  template<typename ObjectType>
  CorbaObject<ObjectType>::Holder::Holder(
    const CORBACommons::CorbaClientAdapter* corba_client_adapter_val,
    const CORBACommons::CorbaObjectRef& object_ref_val)
    throw()
    : corba_client_adapter(ReferenceCounting::add_ref(corba_client_adapter_val)),
      object_ref(object_ref_val)
  {}

  template<typename ObjectType>
  CorbaObject<ObjectType>::CorbaObject()
    throw()
  {}

  template<typename ObjectType>
  CorbaObject<ObjectType>::CorbaObject(
    const CORBACommons::CorbaClientAdapter* corba_client_adapter,
    const CORBACommons::CorbaObjectRef& object_ref)
    throw()
    : holder_(new Holder(corba_client_adapter, object_ref))
  {}

  template<typename ObjectType>
  ObjectType*
  CorbaObject<ObjectType>::operator->() throw(NullPointer, Exception)
  {
    Holder* holder = holder_;

    if(!holder)
    {
      throw NullPointer("");
    }

    SyncPolicy::WriteGuard lock(holder->lock);

    if(!holder->object.in())
    {
      try
      {
        holder->object =
          holder->corba_client_adapter->template resolve_object<ObjectType>(
            holder->object_ref);
      }
      catch(const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << "Can't resolve reference '" << holder->object_ref <<
          "', caught eh::Exception: " << ex.what();
        throw Exception(ostr);
      }
      catch(const CORBA::SystemException& ex)
      {
        Stream::Error ostr;
        ostr << "Can't resolve reference '" << holder->object_ref <<
          "', caught CORBA::SystemException: " << ex;
        throw Exception(ostr);
      }
    }

    return holder->object;
  }

  template<typename ObjectType>
  typename CorbaObject<ObjectType>::ObjectType_var
  CorbaObject<ObjectType>::get() const throw()
  {
    Holder* holder = holder_;

    if(!holder)
    {
      return ObjectType_var();
    }

    SyncPolicy::ReadGuard lock(holder->lock);
    return holder->object;
  }

  template<typename ObjectType>
  const CORBACommons::CorbaObjectRef&
  CorbaObject<ObjectType>::ref() const
    throw(NullPointer)
  {
    const Holder* holder = holder_;

    if(!holder)
    {
      throw NullPointer("");
    }

    SyncPolicy::ReadGuard lock(holder->lock);
    return holder->object_ref;
  }
}
}

#endif /*COMMONS_CORBAOBJECT_HPP*/
