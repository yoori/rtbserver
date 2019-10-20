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


//
// UnitStat inlines
//
inline
bool
UnitStat::db_active() const throw()
{
  return !(flags & UM_NO_DB);
}

//
// BaseUnit inlines
//
inline
Params
BaseUnit::get_global_params()
  throw()
{
  return params_.get_global_params();
}

inline
const GlobalConfig& BaseUnit::get_config() const
  throw()
{
  return params_.get_config();
}

inline
Locals
BaseUnit::get_local_params()
  throw()
{
  return params_.get_local_params();
}

inline
std::string
BaseUnit::fetch_string(const char* obj_name)
{
  ::xsd::tests::AutoTests::ValueType v =
    get_object_by_name(obj_name).Value();
  if (v.base64())
  {
    std::string ret;
    String::StringManip::base64mod_decode(
      ret, v, false);
    return ret;
  }
  return v;
}

inline
std::string
BaseUnit::fetch_string(const std::string& obj_name)
{
  return fetch_string(obj_name.c_str());
}

inline
unsigned long
BaseUnit::fetch_int(const std::string& obj_name)
    throw (Exception)
{
  return fetch_int(obj_name.c_str());
}

inline
long double
BaseUnit::fetch_float(const std::string& obj_name)
  throw (Exception)
{
  return fetch_float(obj_name.c_str());
}

inline
void
BaseUnit::fetch(
  long double& v,
  const std::string& obj_name)
  throw (Exception)
{
  v = fetch_float(obj_name);
}

inline
void
BaseUnit::fetch(
  unsigned long& v,
  const std::string& obj_name)
  throw (Exception)
{
  v = fetch_int(obj_name);
}

inline
void
BaseUnit::fetch(
  std::string& v,
  const std::string& obj_name)
  throw (Exception)
{
  v = fetch_string(obj_name);  
}
