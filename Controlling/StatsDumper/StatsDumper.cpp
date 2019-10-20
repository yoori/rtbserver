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

/// @file Controlling/StatsDumper/StatsDumper.cpp
#include <Generics/Network.hpp>
#include "StatsDumper.hpp"

namespace AdServer
{
  namespace Controlling
  {
    StatsDumper::StatsDumper(
      const CORBACommons::CorbaObjectRef& ref,
      const char* host_name)
      throw (Exception)
      : ref_(CORBACommons::CorbaClientAdapter_var(
          new CORBACommons::CorbaClientAdapter()).in(), ref)
    {
      static const char* FUN = "StatsDumper::StatsDumper";

      try
      {
        if (!host_name)
        {
          char name[2048];
          if (::gethostname(name, sizeof(name)) != 0)
          {
            eh::throw_errno_exception<Exception>(errno,
              FUN, "gethostname failed on host name '", name, "'");
          }
          host_name_ = name;
        }
        else
        {
          host_name_ = host_name;
        }
      }
      catch (const eh::Exception& e)
      {
        Stream::Error ostr;
        ostr << FUN
          << ": Controlling::StatsCollector::can't init: " << e.what();
        throw Exception(ostr);
      }
      catch (const CORBA::SystemException& e)
      {
        Stream::Error ostr;
        ostr << FUN << ": caught exception on _narrow: " << e;
        throw Exception(ostr);
      }
    }

    void
    StatsDumper::dump_statistics(const Generics::Values& stat_snapshot)
      throw (Exception)
    {
      static const char* FUN = "StatsDumper::dump_statistic";

      try
      {
        StatsValueSeq_var in(
          CORBACommons::ValuesConverter::get_stats(stat_snapshot));
        ref_->add_stats(host_name_.c_str(), in);
      }
      catch (const CORBACommons::ProcessStatsControl::ImplementationException&
        e)
      {
        Stream::Error ostr;
        ostr << FUN << ": caught "
          "CORBACommons::ProcessStatsControl::ImplementationException: "
          << e.description;
        throw Exception(ostr, "ADS-ICON-4");
      }
      catch(const CORBA::SystemException& e)
      {
        Stream::Error ostr;
        ostr  << FUN << ": caught CORBA::SystemException: " << e;
        throw Exception(ostr, "ADS-ICON-4");
      }
      catch(const eh::Exception& e)
      {
        Stream::Error ostr;
        ostr  << FUN << ": caught eh::Exception: " << e.what();
        throw Exception(ostr);
      }
    }

  }
}
