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

#ifndef PROFILEMAPUTILS_HPP
#define PROFILEMAPUTILS_HPP

#include <ProfilingCommons/ProfileMap/ExpireProfileMap.hpp>

template<typename MapType>
ReferenceCounting::SmartPtr<
  AdServer::ProfilingCommons::ProfileMap<typename MapType::KeyTypeT> >
init_map(
  const char* root,
  const char* sub_folder,
  const Generics::Time& extend_time,
  bool remove = true,
  ReferenceCounting::SmartPtr<MapType>* sub_map = 0)
  throw(eh::Exception)
{
  if(remove)
  {
    ::system((std::string("rm -r ") + root +
      sub_folder + " 2>/dev/null; mkdir -p " + root +
      sub_folder + "/").c_str());
  }

  ReferenceCounting::SmartPtr<MapType> map(
    new MapType((std::string(root) + sub_folder + "/").c_str(),
    "Chunk_",
    extend_time));

  if(sub_map)
  {
    *sub_map = map;
  }

  return map;
}

#endif /*PROFILEMAPUTILS_HPP*/
