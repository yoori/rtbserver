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

#ifndef COMMONS_EXTERNALUSERIDUTILS_HPP_
#define COMMONS_EXTERNALUSERIDUTILS_HPP_

#include <string>
#include <vector>
#include <String/SubString.hpp>

namespace AdServer
{
namespace Commons
{
  /*
  struct ExternalUserId
  {
    ExternalUserId()
    {}

    ExternalUserId(
      const String::SubString& source_id_val,
      const String::SubString& id_val)
      : source_id(source_id_val.str()),
        id(id_val.str())
    {}

    std::string source_id;
    std::string id;
  };
  */

  typedef std::vector<std::string> ExternalUserIdArray;

  void
  dns_encode_external_user_ids(
    std::string& res,
    const ExternalUserIdArray& user_ids)
    throw();

  void
  dns_decode_external_user_ids(
    ExternalUserIdArray& user_ids,
    const String::SubString& hostname)
    throw();
}
}

#endif /*COMMONS_EXTERNALUSERIDUTILS_HPP_*/
