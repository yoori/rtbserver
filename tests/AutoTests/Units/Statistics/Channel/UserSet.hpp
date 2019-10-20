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

#ifndef _UNITTEST__USERSET_HPP
#define _UNITTEST__USERSET_HPP

#include <tests/AutoTests/Commons/Common.hpp>

namespace AutoTest
{
  namespace Statistics
  {
    /**
     * @class UserSet
     * @brief User collection, using in statistics test
     */
    class UserSet
    {

      DECLARE_EXCEPTION(InvalidIndex, Exception);

      typedef AutoTest::AdClient AdClient;

      typedef std::vector<AdClient> Users;

    public:

      /**
       * @brief Initialize collection.
       *
       * Creates collection of persistent users.
       * @param test, used collection.
       * @param collection size.
       */
      void initialize(
        BaseUnit* test,
        unsigned long size);

      /**
       * @brief Send requests  from each 
       *  user, who have index in range [start_index, stop_index].
       *
       * Sends GET requests to the server
       * @param request sending request.
       * @param fisrt user index.
       * @param last user index.
       * @param detail_info description for this request.
       */
      void process_request(
        const BaseRequest& request,
        unsigned long start_index = 0,
        unsigned long stop_index = 0)
          throw(eh::Exception);
      

      /**
       * @brief Send request to the server from one
       *  of the collection's user.
       *
       * Sends GET requests to the server
       * @param user index.
       * @param request sending request.
       */
      void process_request_i(
        unsigned long index,
        const BaseRequest& request)
          throw(eh::Exception);

      /**
       * @brief Access operator.
       *
       * Get access to user by index.
       * @param key param index.
       * @return user.
       */
      AdClient& operator[](unsigned long index);

      /**
       * @brief Get collection size
       *
       * @return collection size
       */
      size_t size() const;
      
    private:
      Users users_;
      void check_index(unsigned long index);
    };

    inline AdClient& UserSet::operator[](unsigned long index)
    {
      check_index(index);
      return users_[index];
    }

    inline size_t UserSet::size() const
    {
      return users_.size();
    }
  }
}


#endif  // _UNITTEST__USERSET_HPP
