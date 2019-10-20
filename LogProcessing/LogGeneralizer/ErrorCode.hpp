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

#ifndef AD_SERVER_LOG_PROCESSING_ERROR_CODE_HPP
#define AD_SERVER_LOG_PROCESSING_ERROR_CODE_HPP


#include <LogCommons/LogCommons.hpp>

namespace AdServer {
namespace LogProcessing {

  struct SaveEx
  {
    DECLARE_EXCEPTION(PgException, LogSaver::Exception);
    DECLARE_EXCEPTION(CsvException, LogSaver::Exception);
  };

extern const char *LOG_GEN_DB_ERR_CODE_0;
extern const char *LOG_GEN_DB_ERR_CODE_1;
extern const char *LOG_GEN_DB_ERR_CODE_2;
extern const char *LOG_GEN_IMPL_ERR_CODE_0;
extern const char *LOG_GEN_IMPL_ERR_CODE_1;
extern const char *LOG_GEN_IMPL_ERR_CODE_2;
extern const char *LOG_GEN_IMPL_ERR_CODE_3;
extern const char *LOG_GEN_IMPL_ERR_CODE_4;
extern const char *LOG_GEN_IMPL_ERR_CODE_5;
extern const char *LOG_GEN_IMPL_ERR_CODE_6;
extern const char *LOG_GEN_IMPL_ERR_CODE_7;
extern const char *LOG_GEN_IMPL_ERR_CODE_8;
extern const char *LOG_GEN_IMPL_ERR_CODE_9;
extern const char *LOG_GEN_IMPL_ERR_CODE_10;
extern const char *LOG_GEN_IMPL_ERR_CODE_11;

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_ERROR_CODE_HPP */

