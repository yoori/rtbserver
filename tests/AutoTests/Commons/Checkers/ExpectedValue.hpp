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

#ifndef __AUTOTESTS_COMMONS_CHECKER_EXPECTEDVALUE_HPP
#define __AUTOTESTS_COMMONS_CHECKER_EXPECTEDVALUE_HPP

#include <istream>

namespace AutoTest
{
  /**
   * @class ExpValue
   * @brief Expected value storage.
   *
   * Some value, that holds 'not set' state.
   * Useful in checkers. Allows it to check only set values.
   */
  template <typename T>
  class ExpValue
  {
  public:
    
    /**
     * @brief Default constructor.
     */
    ExpValue();

    /**
     * @brief Copy constructor.
     */
    explicit ExpValue(const T& value);
    
    /**
     * @brief Access to original value.
     */
    const T& operator*() const;

    /**
     * @brief Access to original value.
     */
    const T* operator->() const;
    
    /**
     * @brief Access to original value.
     */
    T* operator->();
    
    /**
     * @brief Assignment operator.
     */
    ExpValue& operator=(
      const T& value);
   
    /**
     * @brief Check value set.
     */
    bool is_set() const;
    
    /**
     * @brief Mark value set.
     */
    void is_set(
      bool is_set);
    
  private:
    T value_;
    bool is_set_;
  };
  
  /**
   * @brief Assignment from stream.
   * @param stream
   * @param value
   */
  template <typename T>
  std::istream&
  operator >>(std::istream& istr, ExpValue<T>& value);
  
} //namespace AutoTest

#include "ExpectedValue.tpp"

#endif  // __AUTOTESTS_COMMONS_CHECKER_EXPECTEDVALUE_HPP
