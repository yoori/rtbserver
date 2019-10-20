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

#ifndef PROFILINGCOMMONS_ADAPTPROFILEMAP_HPP
#define PROFILINGCOMMONS_ADAPTPROFILEMAP_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include "ProfileMap.hpp"
#include "DelegateProfileMap.hpp"

namespace AdServer
{
namespace ProfilingCommons
{
  /* wrapper for change MemBuf content stored in ProfileMap at profile read */
  template<typename KeyType, typename ProfileAdapterType>
  class AdaptProfileMap:
    public virtual DelegateProfileMap<KeyType>,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, typename ProfileMap<KeyType>::Exception);

    AdaptProfileMap(
      ProfileMap<KeyType>* profile_map,
      const ProfileAdapterType& profile_adapter = ProfileAdapterType())
      throw();

    virtual
    Generics::ConstSmartMemBuf_var
    get_profile(
      const KeyType& key,
      Generics::Time* last_access_time)
      throw(Exception);

  private:
    ReferenceCounting::SmartPtr<ProfileMap<KeyType> > profile_map_;
    ProfileAdapterType profile_adapter_;
  };
}
}

namespace AdServer
{
namespace ProfilingCommons
{
  /** AdaptProfileMap */
  template<typename KeyType, typename ProfileAdapterType>
  AdaptProfileMap<KeyType, ProfileAdapterType>::
  AdaptProfileMap(
    ProfileMap<KeyType>* profile_map,
    const ProfileAdapterType& profile_adapter)
    throw()
    : DelegateProfileMap<KeyType>(profile_map),
      profile_adapter_(profile_adapter)
  {}

  template<typename KeyType, typename ProfileAdapterType>
  Generics::ConstSmartMemBuf_var
  AdaptProfileMap<KeyType, ProfileAdapterType>::
  get_profile(
    const KeyType& key,
    Generics::Time* last_access_time)
    throw(Exception)
  {
    static const char* FUN = "AdaptProfileMap<>::get_profile()";

    Generics::ConstSmartMemBuf_var res =
      this->no_add_ref_delegate_map_()->get_profile(key, last_access_time);

    if(res.in())
    {
      try
      {
        return profile_adapter_(res.in());
      }
      catch(const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": caught eh::Exception: " << ex.what();
        throw Exception(ostr);
      }
    }

    return Generics::ConstSmartMemBuf_var();
  }
}
}

#endif /*PROFILINGCOMMONS_ADAPTPROFILEMAP_HPP*/
