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


#ifndef _AUTOTESTS_COMMONS_STATS_DIFFSTATS_HPP
#define _AUTOTESTS_COMMONS_STATS_DIFFSTATS_HPP

#include <tests/AutoTests/Commons/Stats/ORMStats.hpp>

namespace AutoTest
{
  namespace ORM
  {
    /**
     * @class DiffStats
     * @brief Base class for statistic with diffs
     */
    template<typename T, int Count>
    class DiffStats: public BasicStats<T, Count>
    {

      typedef T Self;
      typedef BasicStats<T, Count> Base;
      
    public:

      class Diffs;
      /**
       * @brief Constructor.
       *
       * @param Statistic description.
       */
      DiffStats(
        const char* description);
            
      /**
       * @brief Print diff.
       * @param output stream.
       * @param diff.
       */

      static void print_diff(
        std::ostream& out, 
        const stats_diff_type& diff);

      static void print_diff(
        std::ostream& out,
        const Diffs& values);

      /**
       * @brief Print diff with statistic values.
       * @param output stream.
       * @param diff.
       * @param real (current) statistic
       */
      void print_diff(
        std::ostream& out, 
        const stats_diff_type& diff,
        const DiffStats<T, Count>& real) const;
      
      void print_diff(
        std::ostream& out, 
        const Diffs& diff,
        const DiffStats<T, Count>& real) const;
    };
  }
}

#include "DiffStats.tpp"

#endif  //_AUTOTESTS_COMMONS_STATS_DIFFSTATS_HPP
