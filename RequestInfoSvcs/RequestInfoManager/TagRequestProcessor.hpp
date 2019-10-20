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

#ifndef TAGREQUESTPROCESSOR_HPP
#define TAGREQUESTPROCESSOR_HPP

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/Time.hpp>
#include <Commons/UserInfoManip.hpp>
#include <Commons/Containers.hpp>
#include <Commons/StringHolder.hpp>

#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>
#include <LogCommons/LogCommons.hpp>

/**
 * TagRequestProcessor
 */
namespace AdServer
{
namespace RequestInfoSvcs
{
  struct TagRequestInfo
  {
    Generics::Time time;
    unsigned long colo_id;
    unsigned long tag_id;
    unsigned long size_id;
    std::string ext_tag_id;
    char user_status;
    std::string referer;
    AdServer::LogProcessing::StringList urls;
    AdServer::Commons::Optional<unsigned long> referer_hash;

    Generics::Time isp_time;
    std::string country;
    unsigned long site_id;
    Commons::UserId user_id;
    AdServer::Commons::Optional<unsigned long> page_load_id;
    bool ad_shown;
    bool profile_referer;
    AdServer::CampaignSvcs::RevenueDecimal floor_cost;

    AdServer::Commons::RequestId request_id; // passback request id
    Commons::StringHolder_var user_agent;

    void print(std::ostream& out, const char* offset) const throw();
  };

  /**
   * TagRequestProcessor
   */
  class TagRequestProcessor:
    public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    virtual void process_tag_request(const TagRequestInfo&) throw(Exception) = 0;

  protected:
    virtual ~TagRequestProcessor() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<TagRequestProcessor>
    TagRequestProcessor_var;
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  inline
  void TagRequestInfo::
  print(std::ostream& out, const char* offset) const throw()
  {
    out << offset << "user_id = " << user_id.to_string() << std::endl <<
      offset << "time = " << time.get_gm_time() << std::endl <<
      offset << "isp_time = " << isp_time.get_gm_time() << std::endl <<
      offset << "site_id = " << site_id << std::endl <<
      offset << "colo_id = " << colo_id << std::endl <<
      offset << "tag_id = " << tag_id << std::endl <<
      offset << "ext_tag_id = " << ext_tag_id << std::endl <<
      offset << "user_status = '" << user_status << "'" << std::endl <<
      offset << "referer = '" << referer << "'" << std::endl <<
      offset << "page_load_id = ";
    if(page_load_id.present())
    {
      out << *page_load_id;
    }
    else
    {
      out << "null";
    }
    out  << std::endl <<
      offset << "country = " << country << std::endl <<
      offset << "ad_shown = " << ad_shown << std::endl <<
      offset << "profile_referer = " << profile_referer << std::endl;
  }
}
}

#endif /*TAGREQUESTPROCESSOR_HPP*/
