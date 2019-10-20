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

#ifndef AD_SERVER_LOG_PROCESSING_LOG_DB_SAVER_IMPL_HPP
#define AD_SERVER_LOG_PROCESSING_LOG_DB_SAVER_IMPL_HPP


#include <Logger/Logger.hpp>
#include <Commons/Postgres/Environment.hpp>
#include <Commons/Postgres/Connection.hpp>

#include "LogTypeExtTraits.hpp"
#include "LogTypeCsvTraits.hpp"
#include "DbSaverDeclMacro.hpp"
#include "DbConnectionFactory.hpp"
#include "LogGeneralizerStatDef.hpp"

namespace AdServer {
namespace LogProcessing {

DECLARE_NESTED_LOG_DB_SAVER_EXT_2(CreativeStat, CustomCreativeStat,
  LogGeneralizerStatMapBundle_var);

DECLARE_NESTED_LOG_DB_SAVER_EXT_2_USING_PREFIX(
  CreativeStatTraits,
  LogGeneralizerStatMapBundle_var,
  DeferredCreativeStat
);

DECLARE_NESTED_LOG_PG_CSV_SAVER(CreativeStat, CustomCreativeStat,
  LogGeneralizerStatMapBundle_var);

DECLARE_NESTED_LOG_PG_CSV_SAVER_USING_PREFIX(
  CreativeStatTraits,
  CreativeStatCsvTraits,
  LogGeneralizerStatMapBundle_var,
  DeferredCreativeStat
);

DECLARE_NESTED_LOG_DB_SAVER_EXT_2(CmpStat, CustomCmpStat,
  LogGeneralizerStatMapBundle_var);

DECLARE_NESTED_LOG_DB_SAVER_EXT_2_USING_PREFIX(
  CmpStatTraits,
  LogGeneralizerStatMapBundle_var,
  DeferredCmpStat
);

DECLARE_NESTED_LOG_PG_CSV_SAVER(CmpStat, CustomCmpStat,
  LogGeneralizerStatMapBundle_var);

DECLARE_NESTED_LOG_PG_CSV_SAVER_USING_PREFIX(
  CmpStatTraits,
  CmpStatCsvTraits,
  LogGeneralizerStatMapBundle_var,
  DeferredCmpStat
);

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_LOG_DB_SAVER_IMPL_HPP */

