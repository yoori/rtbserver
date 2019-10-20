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


#ifndef _AUTOTESTS_COMMONS_REQUEST_CONVERSATIONREQUEST_HPP
#define _AUTOTESTS_COMMONS_REQUEST_CONVERSATIONREQUEST_HPP

#include "BaseRequest.hpp"

namespace AutoTest
{
  /**
   * @class ConversationRequest
   * @brief Presentation of Conversation (action) AdServer request.
   *
   * This request is sent when end user
   * complete an action (purchasing smth. for example). 
   */
  class ConversationRequest : public BaseRequest
  {

    /// Base url for action request
    static const char*          BASE_URL;

    typedef RequestParam<ConversationRequest> ConversationParam;

  public:

    typedef RequestParamSetter<ConversationRequest> Member;   //!< ConversationRequest member

    /**
     * @brief Constructor.
     *
     * Create ConversationRequest and set default values for params.
     * @param set_defs this flag tells ActionRequest
     * whether or not to set default values for parameters.
     */
    explicit ConversationRequest(bool set_defs = true);

    ConversationRequest(const ConversationRequest& other);

    //request params

    /**
     * @brief Represent 'convid' param.
     *
     * Conversion ID, required
     */
    ConversationParam convid;

    /**
     * @brief Represents 'value' param.
     *
     * Conversion value in float format (dot separated,
     *  for example, 5.99) in currency of advertiser account. 
     */
    ConversationParam value;

    /**
     * @brief Represents 'orderid' param.
     *
     * URL encoded order ID. Maximum decoded length
     * is 100 characters, anything longer is truncated.
     * Only stored on confirmed conversion requests.
     */
    ConversationParam orderid;

    /**
     * @brief Represents 'test' param.
     *
     * Test request flag - (0,1).
     */
    ConversationParam test;

    /**
     * @brief Represents 'debug.time' param
     *
     * Debug parameter used to emulate specific
     * request time (normally in future)
     */
    RequestParam <ConversationRequest, TimeParam> debug_time;


    /**
     * @brief Represents 'Referer' HTTP header.
     *
     * URL of a page which have trigered ad request. 
     */
    HeaderParam<ConversationRequest> referer;
  };
}//namespace AutoTest


#endif  // _AUTOTESTS_COMMONS_REQUEST_CONVERSATIONREQUEST_HPP
