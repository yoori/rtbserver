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

#ifndef GENERICS_HOLDING_HASH_ADAPTER_HPP
#define GENERICS_HOLDING_HASH_ADAPTER_HPP

/* TODO: this file should be merged with Generics/HashTableAdapters.hpp */

#include <String/SubString.hpp>

#include <Generics/Hash.hpp>

namespace Generics
{
  class SubStringNoAlloc : public String::SubString
  {
  public:
    template <class S>
    SubStringNoAlloc(const S& text)
      : String::SubString(text) {}

    SubStringNoAlloc(const char* c, size_t l)
      : String::SubString(c, l) {}
  };

  class HoldingHashAdapter
  {
  public:
    typedef String::SubString text_type;

  public:
    HoldingHashAdapter()
      throw ();

    HoldingHashAdapter(const String::SubString& text)
      throw ();

    HoldingHashAdapter(const SubStringNoAlloc& text)
      throw ();

    template <typename Traits, typename Alloc>
    HoldingHashAdapter(const std::basic_string<char, Traits, Alloc>& text)
      throw ();

    HoldingHashAdapter(size_t hash, const String::SubString& text)
      throw ();

    bool
    operator ==(const HoldingHashAdapter& src) const
      throw ();

    bool
    operator ==(const String::SubString& src) const
      throw ();

    bool
    operator ==(const std::string& src) const
      throw ();

    bool
    operator <(const HoldingHashAdapter& src) const
      throw ();

    size_t
    hash() const
      throw ();

    operator const String::SubString&() const
      throw ();

    operator String::SubString() const
      throw ();

    /* String::SubString accessors */
    const String::SubString&
    text() const throw ();

    String::SubString::ConstPointer
    data() const
      throw ()
    {
      return text_.data();
    }

    String::SubString::SizeType
    length() const
      throw ()
    {
      return text_.length();
    }

    String::SubString::SizeType
    size() const
      throw ()
    {
      return text_.size();
    }

    /* std::string accessors */
    std::string str() const throw ();
    operator const std::string&() const throw ();

  protected:
    void
    calc_hash_()
      throw ();

  protected:
    std::string holder_;
    String::SubString text_;
    size_t hash_;
  };

//
// HoldingHashAdapter class
//
  inline
  HoldingHashAdapter::HoldingHashAdapter()
    throw ()
    : hash_(0)
  {
  }

  inline
  HoldingHashAdapter::HoldingHashAdapter(const String::SubString& text)
    throw ()
    : holder_(text.str()), text_(holder_)
  {
    calc_hash_();
  }

  inline
  HoldingHashAdapter::HoldingHashAdapter(const SubStringNoAlloc& text)
    throw ()
    : text_(text)
  {
    calc_hash_();
  }

  template <typename Traits, typename Alloc>
  HoldingHashAdapter::HoldingHashAdapter(
    const std::basic_string<char, Traits, Alloc>& text) throw ()
    : holder_(text), text_(holder_)
  {
    calc_hash_();
  }

  inline
  HoldingHashAdapter::HoldingHashAdapter(size_t hash,
    const String::SubString& text)
    throw ()
    : holder_(text.str()), text_(holder_), hash_(hash)
  {
  }

  inline
  bool
  HoldingHashAdapter::operator ==(const HoldingHashAdapter& src) const
    throw ()
  {
    return text_ == src.text_;
  }

  inline
  bool
  HoldingHashAdapter::operator ==(const String::SubString& src) const
    throw ()
  {
    return text_ == src;
  }

  inline
  bool
  HoldingHashAdapter::operator ==(const std::string& src) const
    throw ()
  {
    return text_ == src;
  }

  inline
  bool
  HoldingHashAdapter::operator <(const HoldingHashAdapter& src) const
    throw ()
  {
    return text_ < src.text_;
  }

  inline
  size_t
  HoldingHashAdapter::hash() const throw ()
  {
    return hash_;
  }

  inline
  HoldingHashAdapter::operator const String::SubString&() const throw ()
  {
    return text_;
  }

  inline
  HoldingHashAdapter::operator String::SubString() const throw ()
  {
    return text_;
  }

  inline
  const String::SubString&
  HoldingHashAdapter::text() const throw ()
  {
    return text_;
  }

  inline
  std::string
  HoldingHashAdapter::str() const
    throw ()
  {
    return text_.str();
  }

  inline
  HoldingHashAdapter::operator const std::string&()
    const throw ()
  {
    return holder_;
  }

  inline
  void
  HoldingHashAdapter::calc_hash_() throw ()
  {
    Murmur64Hash hash(hash_);
    hash_add(hash, text_);
  }
}

#endif
