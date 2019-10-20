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

/// @file DomainParser.hpp
#ifndef _DOMAINPARSER_HPP_
#define _DOMAINPARSER_HPP_

#include <set>
#include <map>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <String/SubString.hpp>
#include <xsd/CampaignSvcs/DomainConfig.hpp>

namespace AdServer
{
  namespace CampaignSvcs
  {
    class DomainParser: public virtual ReferenceCounting::AtomicImpl
    {
    public:
      typedef xsd::AdServer::Configuration::DomainConfigurationType
        DomainConfig;

      /**
       * @param domain_config Load list of Top Level Domains from XML file
       */
      DomainParser(const DomainConfig& domain_config) throw ();

      /**
       * @param domain Must be in lower case
       * @param specific_domain The return value
       */
      void
      specific_domain(
        const String::SubString& domain,
        std::string& specific_domain)
        const
        throw ();

    protected:
      virtual
      ~DomainParser() throw ();
    private:
      typedef std::set<std::string> ThirdsDomainSet;
      typedef std::map<std::string, ThirdsDomainSet> SecondsDomainMap;
      typedef std::map<std::string, SecondsDomainMap> FirstDomainMap;

      /// Prefix tree - Radix Trie used for data lookup.
      FirstDomainMap domains_trie_;
    };

    typedef ReferenceCounting::SmartPtr<DomainParser> DomainParser_var;
  }
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    inline
    DomainParser::~DomainParser() throw ()
    {
    }
  }
}

#endif // _DOMAINPARSER_HPP_
