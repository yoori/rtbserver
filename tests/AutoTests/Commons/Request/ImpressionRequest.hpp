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

#ifndef __AUTOTESTS_COMMONS_REQUEST_IMPRESSIONREQUEST_HPP
#define __AUTOTESTS_COMMONS_REQUEST_IMPRESSIONREQUEST_HPP

#include "BaseRequest.hpp"
#include <HTTP/UrlAddress.hpp>

namespace AutoTest
{
  /**
   * @class ImpressionRequest
   * @brief Presentation of impression AdServer request.
   *
   * This request is sent when end user
   * make impression on advertising 
   */
  class ImpressionRequest : public BaseRequest
  {
    /// Base url for click request
    static const char*          BASE_URL;
    typedef RequestParam<ImpressionRequest> ImpressionParam;

  public:

    /**
     * @brief Constructor.
     *
     * Create ImpressionRequest and set default values for params.
     * @param set_defs this flag tells ImpressionRequest
     * whether or not to set default values for parameters.
     */
    ImpressionRequest();

    /**
     * @brief Copy constructor.
     *
     * @param other request
     */
    ImpressionRequest(const ImpressionRequest& other);

    /**
     * @brief Constructor.
     *
     * @param URL for decoding
     * @throw HTTP::URLAddress::InvalidURL if url unvalid
     */
    explicit ImpressionRequest(const char* url)
      throw (HTTP::URLAddress::InvalidURL, eh::Exception);

    //request params

    /**
     * @brief Represents 'uid' param.
     *Country code of action request
     * UID - user id. 
     */
    ImpressionParam uid;

    /**
     * @brief Represents 'requestid' param.
     *
     * Impression request id. 
     */
    ImpressionParam requestid;

    /**
     * @brief Represents 'ccid' param.
     *
     * Campaign creative id. 
     */
    ImpressionParam ccid;

    /**
     * @brief Represents 'debug-time
     *
     * Determines time of  click request.
     * Parameter is for debugging/testing purposes only.
     */
    RequestParam <ImpressionRequest, TimeParam> debug_time;
  };

}//namespace AutoTest

#endif  // __AUTOTESTS_COMMONS_REQUEST_IMPRESSIONREQUEST_HPP
