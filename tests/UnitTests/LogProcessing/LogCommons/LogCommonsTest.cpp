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

#include "../../TestHelpers.hpp"

#include <LogCommons/LogCommons.hpp>

using namespace AdServer::LogProcessing;

TEST(restore_log_file_name)
{
  LogFileNameInfo info("Request");
  info.timestamp = Generics::Time(String::SubString("20090525.111111"), "%Y%m%d.%H%M%S");
  info.distrib_index = 5555;
  info.random = 1234567890;

  ASSERT_EQUALS (
    restore_log_file_name(info, "/some_path"), 
    "/some_path/Request.20090525.111111.000000.1234567890.1.5555");

  info.processed_lines_count = 311;
  ASSERT_EQUALS (
    restore_log_file_name(info, "/some_path"), 
    "/some_path/Request.20090525.111111.000000.1234567890.1.5555.311");

  ASSERT_EQUALS (
    restore_log_file_name(info, ""), 
    "Request.20090525.111111.000000.1234567890.1.5555.311");
}

RUN_TESTS
