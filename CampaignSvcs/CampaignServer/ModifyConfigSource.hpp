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

#ifndef CAMPAIGNSERVER_MODIFYCONFIGSOURCE_HPP
#define CAMPAIGNSERVER_MODIFYCONFIGSOURCE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>

#include <Commons/Postgres/ConnectionPool.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  struct ModifyConfig: public virtual ReferenceCounting::AtomicImpl
  {
  public:
    struct CountryDef
    {
      unsigned long cpc_random_imps;
      unsigned long cpa_random_imps;
    };

    typedef std::map<std::string, CountryDef>
      CountryMap;

    CountryMap countries;

  protected:
    virtual
    ~ModifyConfig() throw()
    {}
  };

  typedef ReferenceCounting::QualPtr<ModifyConfig>
    ModifyConfig_var;

  typedef ReferenceCounting::ConstPtr<ModifyConfig>
    CModifyConfig_var;

  class ModifyConfigSource: public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    virtual ModifyConfig_var
    update() throw() = 0;

  protected:
    virtual ~ModifyConfigSource() throw()
    {}
  };

  typedef ReferenceCounting::QualPtr<ModifyConfigSource>
    ModifyConfigSource_var;

  typedef ReferenceCounting::FixedPtr<ModifyConfigSource>
    FModifyConfigSource_var;

  class ModifyConfigDBSource: public ModifyConfigSource
  {
  public:
    ModifyConfigDBSource(
      Logging::Logger* logger,
      Commons::Postgres::ConnectionPool* pg_pool)
      throw();

    ModifyConfig_var
    update() throw();

  protected:
    virtual
    ~ModifyConfigDBSource() throw()
    {}

    void
    query_countries_(
      Commons::Postgres::Connection* conn,
      ModifyConfig& config)
      throw(Exception);

  private:
    Logging::Logger_var logger_;
    AdServer::Commons::Postgres::ConnectionPool_var pg_pool_;
  };
}
}

#endif /*CAMPAIGNSERVER_MODIFYCONFIGSOURCE_HPP*/
