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

#include "RequestInfoManagerStats.hpp"

namespace AdServer {
namespace RequestInfoSvcs {

  const Generics::StringHashAdapter StatNames::LOAD_ABS_TIME(
    "loadAbsTime");
  const Generics::StringHashAdapter StatNames::PROCESS_ABS_TIME(
    "processAbsTime");
  const Generics::StringHashAdapter StatNames::PROCESS_COUNT(
    "processCount");
  const Generics::StringHashAdapter StatNames::FILE_COUNT(
    "fileCount");
  const Generics::StringHashAdapter StatNames::LAST_PROCESSED_TIMESTAMP(
    "lastProcessedFileTimestamp");
  const char* StatNames::LOG_CORBA_NAMES[StatNames::LOG_COUNT] =
  {
    "advertiserAction.",
    "click.",
    "impression.",
    "passbackImpression.",
    "request.",
    "tagRequest.",
  };
  
  const LogStatValues::Key StatNames::WEB_INDEX_REQUESTS("webIndexRequestCounter");

} // namespace RequestInfoSvcs
} // namespace AdServer
