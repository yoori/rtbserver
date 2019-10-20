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

#ifndef CAMPAIGNSERVER_BILLSTATDBSOURCE_HPP_
#define CAMPAIGNSERVER_BILLSTATDBSOURCE_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/PtrHolder.hpp>
#include <Logger/Logger.hpp>
#include <HTTP/UrlAddress.hpp>

#include <Commons/Postgres/ConnectionPool.hpp>

#include "BillStatSource.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
  class BillStatDBSource:
    public BillStatSource,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, BillStatSource::Exception);

    BillStatDBSource(
      Logging::Logger* logger,
      Commons::Postgres::Environment* pg_env)
      throw(Exception);

    virtual Stat_var
    update(Stat* stat,
      const Generics::Time& now)
      throw(Exception);

  protected:
    virtual ~BillStatDBSource() throw() {}

  private:
    Stat_var
    query_db_stats_(const Generics::Time& now)
      throw(Exception);

  private:
    Logging::Logger_var logger_;
    Commons::Postgres::ConnectionPool_var pg_pool_;
  };

  typedef ReferenceCounting::QualPtr<BillStatDBSource>
    BillStatDBSource_var;
}
}

#endif /*CAMPAIGNSERVER_BILLSTATDBSOURCE_HPP_*/
