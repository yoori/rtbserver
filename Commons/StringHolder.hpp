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

#ifndef COMMONS_STRINGHOLDER_HPP
#define COMMONS_STRINGHOLDER_HPP

#include <string>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Hash.hpp>
#include <Generics/Hash.hpp>

namespace AdServer
{
namespace Commons
{
  /* const std::string holder for decrease
   *   number of std::string copy construction
   */
  class StringHolder: public ReferenceCounting::AtomicImpl
  {
  public:
    template <typename... T>
    explicit
    StringHolder(T... data) throw (eh::Exception)
      : str_(std::forward<T>(data)...)
    {
    }

    StringHolder(const String::SubString& str)
      : str_(str.data(), str.length())
    {}
    
    const std::string*
    operator ->() const throw ()
    {
      return &str_;
    }

    const std::string&
    str() const throw ()
    {
      return str_;
    }

    bool
    operator ==(const StringHolder& arg) const throw ()
    {
      return this == &arg || str_ == arg.str_;
    }

  private:
    virtual
    ~StringHolder() throw () = default;

  private:
    std::string str_;
  };
  typedef ReferenceCounting::SmartPtr<
    StringHolder, ReferenceCounting::PolicyAssert>
    StringHolder_var;

  class StringHolderHashAdapter
  {
  public:
    StringHolderHashAdapter() throw ()
      : hash_(0)
    {
    }

    explicit
    StringHolderHashAdapter(StringHolder* str) throw ()
      : value_(ReferenceCounting::add_ref(str))
    {
      Generics::Murmur64Hash hash(hash_);
      hash_add(hash, value_->str());
    }
    
    StringHolder_var
    value() const throw ()
    {
      return value_;
    }

    bool
    operator ==(const StringHolderHashAdapter& right) const throw ()
    {
      return value_->str() == right.value_->str();
    }

    size_t
    hash() const throw ()
    {
      return hash_;
    }

  private:
    StringHolder_var value_;
    size_t hash_;
  };

  /**
   * Wrapper over StringHolder
   */
  class ImmutableString
  {
  public:
    typedef ReferenceCounting::ConstPtr<StringHolder,
      ReferenceCounting::PolicyAssert> CStringHolder_var;

    template <typename... T>
    explicit
    ImmutableString(T... data) throw (eh::Exception)
      : string_holder_(new StringHolder(std::forward<T>(data)...))
    {
    }

    template <typename T>
    ImmutableString&
    operator =(T&& data) throw (eh::Exception)
    {
      string_holder_ = new StringHolder(std::forward<T>(data));
      return *this;
    }

    const std::string&
    str() const throw ()
    {
      return string_holder_->str();
    }

    bool
    empty() const throw ()
    {
      return string_holder_->str().empty();
    }

    bool
    operator <(const ImmutableString& arg) const throw ()
    {
      return str() < arg.str();
    }

    bool
    operator ==(const ImmutableString& arg) const throw ()
    {
      return str() == arg.str();
    }

    bool
    operator !=(const ImmutableString& arg) const throw ()
    {
      return str() != arg.str();
    }

  private:
    CStringHolder_var string_holder_;
  };
}
}

inline
std::ostream&
operator <<(std::ostream& os, const AdServer::Commons::ImmutableString& arg)
  throw (eh::Exception)
{
  os << arg.str();
  return os;
}

#endif /*COMMONS_STRINGHOLDER_HPP*/
