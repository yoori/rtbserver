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

#ifndef USEROPERATIONPROFILESADAPTER_HPP
#define USEROPERATIONPROFILESADAPTER_HPP

#include <eh/Exception.hpp>
#include <Generics/MemBuf.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    const unsigned long PREF_GET_OPERATION_PROFILE_VERSION = 1; 
    const unsigned long FRAUD_OPERATION_PROFILE_VERSION = 1;
    const unsigned long MATCH_OPERATION_PROFILE_VERSION = 340;
    const unsigned long MERGE_OPERATION_PROFILE_VERSION = 350;
    const unsigned long FC_UPDATE_OPERATION_PROFILE_VERSION = 351;
    const unsigned long FC_CONFIRM_OPERATION_PROFILE_VERSION = 340;
    const unsigned long PREF_UPDATE_OPERATION_PROFILE_VERSION = 1;
    const unsigned long PROP_UPDATE_OPERATION_PROFILE_VERSION = 1;
    const unsigned long ADD_AUDIENCE_CHANNELS_OPERATION_PROFILE_VERSION = 1;
    const unsigned long REMOVE_AUDIENCE_CHANNELS_OPERATION_PROFILE_VERSION = 1;

    struct UserFreqCapUpdateOperationProfilesAdapter
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      Generics::SmartMemBuf_var
      operator()(Generics::SmartMemBuf* smart_mem_buf) throw(Exception);
    };

    struct UserFreqCapConfirmOperationProfilesAdapter
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      Generics::SmartMemBuf_var
      operator()(Generics::SmartMemBuf* smart_mem_buf) throw(Exception);
    };

    struct UserMatchOperationProfilesAdapter
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      template<typename InputIterator, typename OutputContainerType>
      void fill_channel_trigger_seq(
        InputIterator first,
        InputIterator last,
        OutputContainerType& out);
      
      Generics::SmartMemBuf_var
      operator()(Generics::SmartMemBuf* smart_mem_buf) throw(Exception);
    };

    struct UserMergeOperationProfilesAdapter
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      Generics::SmartMemBuf_var
      operator()(Generics::SmartMemBuf* smart_mem_buf) throw(Exception);
    };

  }
}

#endif /* USEROPERATIONPROFILESADAPTER_HPP */
