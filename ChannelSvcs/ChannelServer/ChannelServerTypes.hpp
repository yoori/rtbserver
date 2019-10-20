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

#ifndef AD_SERVER_CHANNELSERVER_TYPES
#define AD_SERVER_CHANNELSERVER_TYPES

#include <eh/Exception.hpp>

namespace AdServer
{
namespace ChannelSvcs
{
  struct ChannelServerStats
  {
    enum StatParams
    {
      KW_COUNT = 0,
      KW_ID_COUNT,
      URL_COUNT,
      URL_ID_COUNT,
      UID_COUNT,
      UID_ID_COUNT,
      NS_KW_COUNT,
      NS_URL_COUNT,
      MATCHINGS_COUNT,
      EXCEPTIONS_COUNT,
      PARAMS_MAX
    };
    static const char* param_name[PARAMS_MAX];
    size_t params[PARAMS_MAX];
    std::string configuration;
    Generics::Time configuration_date;
  };

  namespace ChannelServerException
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(NotReady, Exception);
    DECLARE_EXCEPTION(TemporyUnavailable, eh::DescriptiveException);
  }

}
}

#endif //AD_SERVER_CHANNELSERVER_TYPES

