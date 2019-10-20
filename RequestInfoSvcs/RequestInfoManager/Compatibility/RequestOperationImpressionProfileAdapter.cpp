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

#include <Generics/Time.hpp>
#include <Commons/UserInfoManip.hpp>
#include <Commons/Algs.hpp>

#include <RequestInfoSvcs/RequestInfoCommons/RequestOperationProfile.hpp>
#include <RequestInfoSvcs/RequestInfoManager/Compatibility/RequestOperationImpressionProfile_v0.hpp>

#include "RequestOperationImpressionProfileAdapter.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
namespace
{
  namespace RequestInfoSvcs_v1
  {
    void convert_to_v331(Generics::MemBuf& membuf)
    {
      AdServer::RequestInfoSvcs::RequestOperationImpressionWriter
        profile_writer;

      AdServer::RequestInfoSvcs_v0::RequestOperationImpressionReader
        old_profile_reader(membuf.data(), membuf.size());

      profile_writer.version() = 331;
      profile_writer.time() = old_profile_reader.time();
      profile_writer.request_id() = old_profile_reader.request_id();
      profile_writer.verify_impression() = old_profile_reader.verify_impression();
      profile_writer.pub_revenue() = old_profile_reader.pub_revenue();
      profile_writer.pub_sys_revenue() = old_profile_reader.pub_sys_revenue();
      profile_writer.pub_revenue_type() = old_profile_reader.pub_revenue_type();
      profile_writer.user_id() = AdServer::Commons::UserId().to_string();

      Generics::MemBuf mb(profile_writer.size());
      profile_writer.save(mb.data(), mb.size());
      membuf.assign(mb.data(), mb.size());
    }
  }
}
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  void
  RequestOperationImpressionProfileAdapter::operator()(
    Generics::MemBuf& membuf) throw(Exception)
  {
    static const char* FUN = "RequestOperationImpressionProfileAdapter::operator()";

    unsigned long version_head_size =
      RequestOperationVersionReader::FIXED_SIZE;

    if(membuf.size() < version_head_size)
    {
      throw Exception("Corrupt header");
    }

    try
    {
      RequestOperationVersionReader version_reader(
        membuf.data(), membuf.size());

      if(version_reader.version() != AdServer::
         RequestInfoSvcs::CURRENT_REQUESTOPERATIONIMPRESSION_PROFILE_VERSION)
      {
        unsigned long current_version = version_reader.version();

        if(current_version == 0) // previous version
        {
          RequestInfoSvcs_v1::convert_to_v331(membuf);
          current_version = 331;
        }

        if(current_version != AdServer::RequestInfoSvcs::
           CURRENT_REQUESTOPERATIONIMPRESSION_PROFILE_VERSION)
        {
          Stream::Error ostr;
          ostr << FUN << ": incorrect version after adaptation = " <<
            current_version;
          throw Exception(ostr);
        }
      }
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": caught eh::Exception: " << ex.what();
      throw Exception(ostr);
    }
  }
}
}
