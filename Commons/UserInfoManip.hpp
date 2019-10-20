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

#ifndef COMMONS_USER_INFO_MANIP_HPP_
#define COMMONS_USER_INFO_MANIP_HPP_

#include <Generics/Uuid.hpp>
#include <Generics/CRC.hpp>
#include <Generics/Hash.hpp>

namespace AdServer
{
  namespace Commons
  {
    typedef Generics::Uuid UserId;
    typedef Generics::Uuid RequestId;

    extern const UserId PROBE_USER_ID;

    inline
    unsigned long
    uuid_distribution_hash(const Generics::Uuid& uuid) throw ()
    {
      return Generics::CRC::quick(0, &*uuid.begin(), uuid.size());
    }

    inline
    unsigned long
    external_id_distribution_hash(const String::SubString& external_id)
      throw ()
    {
      unsigned long ext_hash;

      {
        Generics::Murmur64Hash hash(ext_hash);
        hash.add(external_id.data(), external_id.size());
      }

      return ext_hash;
    }

    struct UserIdDistributionHashAdapter: public UserId
    {
      UserIdDistributionHashAdapter(const UserId& user_id)
        : UserId(user_id)
      {
        hash_ = uuid_distribution_hash(user_id);
      }

      unsigned long hash() const
      {
        return hash_;
      }

    private:
      unsigned long hash_;
    };
  }
}

#endif /*COMMONS_USER_INFO_MANIP_HPP*/
