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

#ifndef _COMMONS_CONTAINERS_HPP_
#define _COMMONS_CONTAINERS_HPP_

namespace AdServer
{
  namespace Commons
  {
    template<typename ObjectType>
    class Optional
    {
    public:
      Optional(): defined_(false), val_() {}

      Optional(const Optional<ObjectType>& val)
        : defined_(val.defined_),
          val_(val.val_)
      {}

      Optional(Optional<ObjectType>&& val)
        : defined_(val.defined_),
          val_(std::move(val.val_))
      {}

      template<typename...Args>
      explicit Optional(bool defined, Args... data)
        : defined_(defined),
          val_(std::forward<Args>(data)...)
      {}

      explicit Optional(const ObjectType& val)
        : defined_(true),
          val_(val)
      {}

      Optional(const ObjectType* val)
        : defined_(val),
          val_(val ? *val : ObjectType())
      {}

      const ObjectType*
      operator->() const throw ()
      {
        return &val_;
      }

      ObjectType*
      operator->() throw ()
      {
        return &val_;
      }

      const ObjectType&
      operator*() const throw ()
      {
        return val_;
      }

      ObjectType&
      operator*() throw ()
      {
        return val_;
      }

      bool
      present() const throw ()
      {
        return defined_;
      }

      void set(const ObjectType& val)
      {
        val_ = val;
        defined_ = true;
      }

      ObjectType&
      fill()
      {
        defined_ = true;
        return val_;
      }

      Optional&
      operator=(const ObjectType& val)
      {
        set(val);
        return *this;
      }

      Optional&
      operator=(const Optional<ObjectType>& val)
      {
        if(val.present())
        {
          set(*val);
        }
        else
        {
          clear();
        }

        return *this;
      }

      template<typename RightType>
      Optional&
      operator=(const Optional<RightType>& val)
      {
        if(val.present())
        {
          set(*val);
        }
        else
        {
          clear();
        }

        return *this;
      }

      /*
      template<typename RightType>
      Optional&
      operator=(const RightType& val)
      {
        set(val);
        return *this;
      }
      */

      template<typename RightType>
      bool
      operator==(const Optional<RightType>& right) const
      {
        return &right == this || (present() == right.present() &&
          (!present() || **this == *right));
      }

      template <typename RightType>
      bool
      operator !=(const Optional<RightType>& right) const
      {
        return !(*this == right);
      }

      void clear()
      {
        defined_ = false;
        val_ = ObjectType();
      }

    protected:
      template <typename CompatibleType>
      Optional(const CompatibleType& val, bool defined)
        : defined_(defined), val_(defined ? ObjectType(val) : ObjectType())
      {}

      void
      present_(bool new_state) throw ()
      {
        defined_ = new_state;
      }

    private:
      bool defined_;
      ObjectType val_;
    };

    template <typename ObjectType>
    class ValueStateHolder
    {
    public:
      typedef ObjectType ValueType;
      enum State
      {
        S_NOT_INITED,
        S_GOOD,
        S_FAIL
      };

      ValueStateHolder()
      : val_(), state_(S_NOT_INITED)
      {}

      template<typename...Args>
      explicit ValueStateHolder(State state, Args... data): val_(std::forward<Args>(data)...), state_(state) {}
      explicit ValueStateHolder(const ObjectType& val): val_(val), state_(S_GOOD) {}
      ValueStateHolder(const ObjectType* val): val_(val ? *val : ObjectType()), state_(val ? S_GOOD : S_NOT_INITED) {}

      const ObjectType*
      operator->() const throw ()
      {
        return &val_;
      }

      ObjectType*
      operator->() throw ()
      {
        return &val_;
      }

      const ObjectType&
      operator*() const throw ()
      {
        return val_;
      }

      ObjectType&
      operator*() throw ()
      {
        return val_;
      }

      bool
      present() const throw ()
      {
        return state_ == S_GOOD;
      }

      void set(const ObjectType& val)
      {
        val_ = val;
        state_ = S_GOOD;
      }

      ValueStateHolder&
      operator=(const ObjectType& val)
      {
        set(val);
        return *this;
      }

      ValueStateHolder&
      operator=(const ValueStateHolder<ObjectType>& val)
      {
        if(val.present())
        {
          set(*val);
        }
        else
        {
          clear();
        }

        return *this;
      }

      template<typename RightType>
      ValueStateHolder&
      operator=(const ValueStateHolder<RightType>& val)
      {
        if(val.present())
        {
          set(*val);
        }
        else
        {
          clear();
        }

        return *this;
      }

      template<typename RightType>
      ValueStateHolder&
      operator=(const RightType& val)
      {
        set(val);
        return *this;
      }

      template<typename RightType>
      bool
      operator==(const ValueStateHolder<RightType>& right) const
      {
        return &right == this || (present() == right.present() &&
          (!present() || **this == *right));
      }

      template <typename RightType>
      bool
      operator !=(const ValueStateHolder<RightType>& right) const
      {
        return !(*this == right);
      }

      void clear()
      {
        state_ = S_NOT_INITED;
        val_ = ObjectType();
      }

      void set_state(bool s) throw()
      {
        state_ = s;
      }

      bool state() const throw()
      {
        return state_;
      }

      bool fail() const throw()
      {
        return state_ == S_FAIL;
      }

      bool good() const throw()
      {
        return state_ == S_GOOD;
      }

      bool empty() const throw()
      {
        return state_ == S_NOT_INITED;
      }

    protected:
      template <typename CompatibleType>
      ValueStateHolder(const CompatibleType& val, State state)
        : val_(state == S_GOOD ? ObjectType(val) : ObjectType()), state_(state)
      {}

    protected:
      ValueType val_;
      bool state_;
    };


  }
}

#endif /*_COMMONS_CONTAINERS_HPP_*/
