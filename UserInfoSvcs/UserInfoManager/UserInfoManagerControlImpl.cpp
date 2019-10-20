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

#include <Commons/CorbaAlgs.hpp>

#include "UserInfoManagerControlImpl.hpp"

namespace AdServer{
namespace UserInfoSvcs{

  /**
   * UserInfoManagerControlImpl
   */
  
  UserInfoManagerControlImpl::UserInfoManagerControlImpl(
    UserInfoManagerImpl* user_info_manager_impl)
    throw(Exception)
    : user_info_manager_impl_(ReferenceCounting::add_ref(user_info_manager_impl)),
      admitted_(false)
  {
  }

  UserInfoManagerControlImpl::~UserInfoManagerControlImpl() throw()
  {
  }

  AdServer::UserInfoSvcs::UserInfoManagerStatus
  UserInfoManagerControlImpl::status() throw()
  {
    ReadGuard_ lock(lock_);
    
    if(admitted_)
    {
      return AdServer::UserInfoSvcs::S_ADMITTED;
    }

    return AdServer::UserInfoSvcs::S_READY;
  }
  
  void 
  UserInfoManagerControlImpl::get_source_info(
    AdServer::UserInfoSvcs::ChunksConfig_out user_info_source)
    throw(AdServer::UserInfoSvcs::UserInfoManagerControl::ImplementationException)
  {
    static const char* FUN = "UserInfoManagerControlImpl::get_source_info()";

    try
    {
      user_info_source = new AdServer::UserInfoSvcs::ChunksConfig();

      unsigned long common_chunks_number;
      
      UserInfoManagerImpl::ChunkIdList chunk_ids;
      user_info_manager_impl_->get_controllable_chunks(
        chunk_ids, common_chunks_number);

      user_info_source->common_chunks_number = common_chunks_number;
      
      CorbaAlgs::fill_sequence(chunk_ids.begin(),
        chunk_ids.end(), user_info_source->chunk_ids);
    }
    catch(const UserInfoManagerImpl::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": "
        "Can't get chunk ids. Caught UserInfoManagerImpl::Exception: " <<
        ex.what();
      CORBACommons::throw_desc<
        UserInfoSvcs::UserInfoManagerControl::ImplementationException>(
          ostr.str());
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": "
        "Can't get chunk ids. Caught eh::Exception: " << ex.what();
      CORBACommons::throw_desc<
        UserInfoSvcs::UserInfoManagerControl::ImplementationException>(
          ostr.str());
    }
  }

  void UserInfoManagerControlImpl::admit() throw()
  {
    WriteGuard_ lock(lock_);
    admitted_ = true;
  }
  
} /*UserInfoSvcs*/
} /*AdServer*/

