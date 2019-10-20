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

/// @file DomainParser.cpp

#include "DomainParser.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
  DomainParser::DomainParser(const DomainConfig& config) throw ()
  {
    for(DomainConfig::Domain_sequence::const_iterator d_it =
          config.Domain().begin();
        d_it != config.Domain().end(); ++d_it)
    {
      if (!d_it->SubDomain().size())
      {
        domains_trie_.insert(
          FirstDomainMap::value_type(d_it->name(), SecondsDomainMap()));
        continue;
      }

      for (xsd::AdServer::Configuration::DomainType::
          SubDomain_sequence::const_iterator sd_it =
            d_it->SubDomain().begin();
          sd_it != d_it->SubDomain().end(); ++sd_it)
      {
        std::string::size_type pos = sd_it->name().rfind('.');
        FirstDomainMap::mapped_type& second_domains =
          domains_trie_[d_it->name()];
        if (pos != std::string::npos && pos != 0) // third.second.first domain
        {
          SecondsDomainMap::mapped_type& third_domains =
            second_domains[sd_it->name().substr(pos + 1)];
          third_domains.insert(sd_it->name().substr(0, pos));
        }
        else
        {
          second_domains.insert(
            FirstDomainMap::mapped_type::value_type(sd_it->name(),
              FirstDomainMap::mapped_type::mapped_type()));
        }
      }
    }
  }

  void
  DomainParser::specific_domain(
    const String::SubString& domain,
    std::string& result)
    const
    throw ()
  {
    String::SubString::SizeType pos = domain.rfind('.');
    String::SubString::SizeType last_pos = String::SubString::NPOS;
    for (;;)
    {
      if (pos == String::SubString::NPOS || pos == 0)
      { // Took first level domain. first or .first
        domain.assign_to(result);
        return;
      }
      FirstDomainMap::const_iterator cit = domains_trie_.end();
      if (pos < domain.size() - 1) // "domain." case, avoid overflow
      {
        // get ".com" by com
        cit = domains_trie_.find(
          std::string(domain.data() + pos + 1, domain.size() - pos - 1));
      }
      // Take second level domain
      last_pos = pos - 1;
      pos = domain.rfind('.', last_pos);
      // not exists first domain .somefirst ||
      // .second.first || second.first || not exists subdomains some.first
      if (cit == domains_trie_.end() ||
        pos == String::SubString::NPOS || pos == 0 || !cit->second.size())
      {
        domain.substr(++pos).assign_to(result);
        return;
      }

       // get ".ru.com" by ru
      SecondsDomainMap::const_iterator cit_two =
        cit->second.find(std::string(domain.data() + pos + 1, last_pos - pos));
      if (cit_two == cit->second.end())  // not exists subdomains .some.first
      {
        break;
      }
      last_pos = pos - 1;
      pos = domain.rfind('.', last_pos);
      // some.second.first || .some.second.first || !exist some.second.first
      if (pos == String::SubString::NPOS || pos == 0 || !cit_two->second.size())
      {
        break;
      }
      ThirdsDomainSet::const_iterator cit_third =
        cit_two->second.find(
          std::string(domain.data() + pos + 1, last_pos - pos));
      if (cit_third != cit_two->second.end())
      {
        last_pos = pos - 1;
        pos = domain.rfind('.', last_pos);
      }
      break;
    }
    /// Unified write results
    // somepiece<.(pos)>domain_n<.(last_pos)>DOMAIN_n-1. ... .DOMAIN_1
    // Check that somepiece is www.
    if (pos == String::SubString::NPOS && last_pos == 2 &&
      !strncmp("www", domain.data(), 3))
    {
      // check exception domain="www.ro" !?
      domain.substr(4).assign_to(result);
    }
    else
    {
      domain.substr(++pos).assign_to(result);
    }
  }
}
}
