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

#ifndef USERINFOSVCS_USEROPERATIONGENERATOR_NULLUSEROPERATIONPROCESSOR_HPP_
#define USERINFOSVCS_USEROPERATIONGENERATOR_NULLUSEROPERATIONPROCESSOR_HPP_

#include <UserInfoSvcs/UserInfoManager/UserOperationProcessor.hpp>

namespace AdServer
{
namespace UserInfoSvcs
{
  class NullUserOperationProcessor :
    public UserOperationProcessor,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    virtual
    bool
    remove_user_profile(const UserId&)
      throw(ChunkNotFound, Exception)
    {
      return false;
    }

    virtual
    void
    fraud_user(
      const UserId&,
      const Generics::Time&)
      throw(NotReady, ChunkNotFound, Exception)
    {}

    virtual
    void
    match(
      const RequestMatchParams&,
      long,
      long,
      ColoUserId&,
      const ChannelMatchPack&,
      ChannelMatchMap&,
      UserAppearance&,
      ProfileProperties&,
      AdServer::ProfilingCommons::OperationPriority,
      UserInfoManagerLogger::HistoryOptimizationInfo*,
      UniqueChannelsResult* = 0)
      throw(NotReady, ChunkNotFound, Exception)
    {}

    virtual
    void
    merge(
      const RequestMatchParams&,
      const Generics::MemBuf&,
      Generics::MemBuf&,
      const Generics::MemBuf&,
      const Generics::MemBuf&,
      UserAppearance&,
      long,
      long,
      AdServer::ProfilingCommons::OperationPriority,
      UserInfoManagerLogger::HistoryOptimizationInfo* = 0)
      throw(NotReady, ChunkNotFound, Exception)
    {}

    virtual
    void
    exchange_merge(
      const UserId&,
      const Generics::MemBuf&,
      const Generics::MemBuf&,
      UserInfoManagerLogger::HistoryOptimizationInfo*)
      throw(NotReady, ChunkNotFound, Exception)
    {}

    virtual
    void
    update_freq_caps(
      const UserId&,
      const Generics::Time&,
      const Commons::RequestId&,
      const UserFreqCapProfile::FreqCapIdList&,
      const UserFreqCapProfile::FreqCapIdList&,
      const UserFreqCapProfile::FreqCapIdList&,
      const UserFreqCapProfile::SeqOrderList&,
      const UserFreqCapProfile::CampaignIds&,
      const UserFreqCapProfile::CampaignIds&,
      AdServer::ProfilingCommons::OperationPriority)
      throw(ChunkNotFound, Exception)
    {}

    virtual
    void
    confirm_freq_caps(
      const UserId&,
      const Generics::Time&,
      const Commons::RequestId&,
      const std::set<unsigned long>&)
      throw(ChunkNotFound, Exception)
    {}

    virtual
    void
    update_wd_impressions(
      const UserId&,
      const Generics::Time&,
      const AdServer::Commons::FreqCap&,
      const AdServer::Commons::FreqCap&,
      const AdServer::Commons::FreqCap&)
      throw(ChunkNotFound, Exception)
    {}

    virtual void
    remove_audience_channels(
      const UserId&,
      const AudienceChannelSet&)
      throw(ChunkNotFound, Exception)
    {}

    virtual void
    add_audience_channels(
      const UserId&,
      const AudienceChannelSet&)
      throw(NotReady, ChunkNotFound, Exception)
    {}

    virtual void
    consider_publishers_optin(
      const UserId&,
      const std::set<unsigned long>&,
      const Generics::Time&,
      AdServer::ProfilingCommons::OperationPriority)
      throw(ChunkNotFound, Exception)
    {}

  protected:
    virtual
    ~NullUserOperationProcessor() throw ()
    {}
  };
}
}

#endif /* USERINFOSVCS_USEROPERATIONGENERATOR_NULLUSEROPERATIONPROCESSOR_HPP_ */
