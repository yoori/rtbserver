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

#ifndef CONTROL_ADMIN_APPLICATION_HPP_
#define CONTROL_ADMIN_APPLICATION_HPP_

#include <getopt.h>
#include<limits.h>
#include<vector>
#include<string>
#include<iostream>
#include<sstream>
#include<eh/Exception.hpp>
#include<CORBACommons/CorbaAdapters.hpp>
#include<CORBACommons/Stats.hpp>
#include<Controlling/StatsCollector/StatsCollector.hpp>
#include<UtilCommons/Table.hpp>

namespace AdServer
{
  namespace Controlling
  {
    class ControlAdmin
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      ControlAdmin() throw ()
        : adapter_(new CORBACommons::CorbaClientAdapter), quiet_(false)
      {
      }
      virtual ~ControlAdmin()throw(){};
      int run(int argc, char* argv[]) throw();
      void help(int topic) throw();
      int get(int argc, char* argv[]) throw();
      int put(int argc, char* argv[]) throw();

    private:
      enum
      {
        GET_TOPIC = 0,
        PUT_TOPIC,
        MAX_TOPIC
      };
      enum
      {
        HELP_COM = 0,
        GET_COM,
        PUT_COM,
        MAX_COM
      };

      enum Types
      {
        LONG = 0,
        ULONG,
        DOUBLE,
        STRING
      };

      void parse_options_(int argc, char* argv[]) throw();

      void resolve_stats_control_ref_() throw(Exception);

      void print_header_() throw();
      void print_keys_() throw(Exception);

      static Types detect_value_(const std::string& str) throw();

    private:
      typedef std::vector<std::string> StringVector;
      typedef std::unique_ptr<Table> TablePtr;

      StringVector keys_;
      StringVector values_;
      std::string reference_;
      CORBACommons::CorbaClientAdapter_var adapter_;
      StatsCollector_var service_;
      bool quiet_;
      std::string command_;

    private:
      static const char* topics[MAX_TOPIC+1];
      static const char* command[MAX_COM];
    };
  }
}

#endif
