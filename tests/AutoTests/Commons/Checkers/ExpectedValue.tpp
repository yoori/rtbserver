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

#ifndef __AUTOTESTS_COMMONS_CHECKER_EXPECTEDVALUE_IPP
#define __AUTOTESTS_COMMONS_CHECKER_EXPECTEDVALUE_IPP

namespace AutoTest
{
  // ExpValue
  template <typename T>
  ExpValue<T>::ExpValue() :
    is_set_(false)
  { }

  template <typename T>
  ExpValue<T>::ExpValue(
    const T& value) :
    value_(value),
    is_set_(true)
  { }

  template <typename T>
  const T&
  ExpValue<T>::operator*() const
  {
    return value_;
  }

  template <typename T>
  const T*
  ExpValue<T>::operator->() const
  {
    return &value_;
  }

  template <typename T>
  T*
  ExpValue<T>::operator->()
  {
    return &value_;
  }

  template <typename T>
  ExpValue<T>&
  ExpValue<T>::operator=(const T& value)
  {
    value_ = value;
    is_set_ = true;
    
    return *this;
  }

  template <typename T>
  bool
  ExpValue<T>::is_set() const
  {
    return is_set_;
  }

  template <typename T>
  void
  ExpValue<T>::is_set(
    bool is_set)
  {
    is_set_ = is_set;
  }

  template <typename T>
  std::istream&
  operator>> (
    std::istream &in,
    ExpValue<T>& value)
  {
    T v;
    in >> v;
    value = v;
    return in;
  }
}
#endif  // __AUTOTESTS_COMMONS_CHECKER_EXPECTEDVALUE_IPP
