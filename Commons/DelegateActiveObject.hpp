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

#ifndef COMMONS_DELEGATEACTIVEOBJECT_HPP_
#define COMMONS_DELEGATEACTIVEOBJECT_HPP_

#include <Generics/ActiveObject.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

namespace AdServer
{
  namespace Commons
  {
    class DelegateActiveObject :
      public Generics::ActiveObjectCommonImpl
    {
    public:
      template<typename Delegate>
      DelegateActiveObject(
        const Delegate& delegate,
        Generics::ActiveObjectCallback* callback,
        unsigned threads_number = 1,
        unsigned stack_size = 0)
        throw ()
        : Generics::ActiveObjectCommonImpl(
            SingleJob_var(new DelegateJob<Delegate>(delegate, callback)),
            threads_number,
            stack_size)
      {}

    private:
      template<typename Delegate>
      class DelegateJob : public SingleJob
      {
      public:
        DelegateJob(
          const Delegate& delegate,
          Generics::ActiveObjectCallback* callback)
          throw()
          : SingleJob(callback),
            delegate_(delegate)
        {}

        virtual void
        work() throw()
        {
          while (!this->is_terminating())
          {
            delegate_();
          }
        }

        virtual void
        terminate() throw()
        {}

      private:
        Delegate delegate_;

      protected:
        virtual
        ~DelegateJob() throw ()
        {}
      };

      class SelfDelegateJob : public SingleJob
      {
      public:
        SelfDelegateJob(
          DelegateActiveObject& delegate_active_object,
          Generics::ActiveObjectCallback* callback)
          throw()
          : SingleJob(callback),
            delegate_active_object_(delegate_active_object)
        {}

        virtual void
        work() throw()
        {
          delegate_active_object_.work_();
        }

        virtual void
        terminate() throw()
        {
          delegate_active_object_.terminate_();
        }

      private:
        DelegateActiveObject& delegate_active_object_;

      protected:
        virtual
        ~SelfDelegateJob() throw ()
        {}
      };

    protected:
      DelegateActiveObject(
        Generics::ActiveObjectCallback* callback,
        unsigned threads_number = 1,
        unsigned stack_size = 0)
        throw ()
        : Generics::ActiveObjectCommonImpl(
            SingleJob_var(new SelfDelegateJob(*this, callback)),
            threads_number,
            stack_size)
      {}

      virtual
      ~DelegateActiveObject() throw ()
      {}

      virtual void
      work_() throw ()
      {}

      virtual void
      terminate_() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<DelegateActiveObject>
      DelegateActiveObject_var;

    template<typename Delegate>
    DelegateActiveObject_var
    make_delegate_active_object(
      const Delegate& delegate,
      Generics::ActiveObjectCallback* callback,
      unsigned threads_number = 1)
      throw ()
    {
      return DelegateActiveObject_var(
        new DelegateActiveObject(
          delegate,
          callback,
          threads_number));
    }
  }
}

#endif /* COMMONS_DELEGATEACTIVEOBJECT_HPP_ */
