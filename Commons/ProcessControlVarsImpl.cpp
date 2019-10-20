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

// $Id: ProcessControlVarsImpl.cpp 125956 2013-07-18 15:59:18Z andrey_gusev $

#include "ProcessControlVarsImpl.hpp"

namespace AdServer {
namespace Commons {

const char LogLevelProcessor::VAR_NAME[] = "LOG_LEVEL";
const char DbStateProcessor::VAR_NAME[] = "DB";

char*
ProcessControlVarsImpl::control(const char *var_name, const char *var_value)
  throw(Exception)
{
  static const char* FUN = "ProcessControlVarsImpl::control()";

  VarProcessorMapT::const_iterator it = var_proc_map_.find(var_name);
  if (it == var_proc_map_.end())
  {
    Stream::Error err;
    err << "No processor found for variable: '" << var_name << "'";
    throw Exception(err);
  }

  std::string result;

  try
  {
    if (!it->second->set_value(var_value, result))
    {
      result = "Failed to set variable '";
      result += var_name;
      result +="' to '";
      result += var_value;
      result += "'";
    }
  }
  catch(const eh::Exception& ex)
  {
    Stream::Error ostr;
    ostr << FUN << ": caught eh::Exception: " << ex.what();
    throw Exception(ostr);
  }

  return CORBA::string_dup(result.c_str());
}

} // namespace Commons
} // namespace AdServer
