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

#ifndef USERINFOCOMMONS_FREQCAPPROFILE_HPP
#define USERINFOCOMMONS_FREQCAPPROFILE_HPP

#include <Generics/MemBuf.hpp>
#include <Commons/UserInfoManip.hpp>
#include "Allocator.hpp"
#include "FreqCapConfig.hpp"

#include <UserInfoSvcs/UserInfoCommons/UserFreqCapProfileDescription.hpp>

namespace AdServer
{
namespace UserInfoSvcs
{
  static const unsigned long
  CURRENT_FREQ_CAP_PROFILE_VERSION = 351;

  class UserFreqCapProfile
  {
  public:
    struct SeqOrder
    {
      SeqOrder() 
      {}

      SeqOrder(
        unsigned long ccg_id_val,
        unsigned long set_id_val,
        unsigned long imps_val)
        :
        ccg_id(ccg_id_val),
        set_id(set_id_val),
        imps(imps_val)
      {}

      unsigned long ccg_id;
      unsigned long set_id;
      unsigned long imps;
    };

    struct CampaignFreq
    {
      unsigned long campaign_id;
      unsigned long imps;
    };

    typedef std::list<SeqOrder> SeqOrderList;
    typedef std::list<unsigned long> FreqCapIdList;
    typedef std::list<CampaignFreq> CampaignFreqs;
    typedef std::list<unsigned long> CampaignIds;

  public:
    DECLARE_EXCEPTION(Invalid, eh::DescriptiveException);

    UserFreqCapProfile(ConstSmartMemBufPtr plain_profile)
      throw(Invalid);

    bool
    full(FreqCapIdList& fcs,
      FreqCapIdList* virtual_fcs,
      SeqOrderList& seq_orders,
      CampaignFreqs& campaign_freqs,
      const Generics::Time& now,
      const FreqCapConfig& fc_config)
      throw(eh::Exception);

    void
    consider(const Commons::RequestId& request_id,
      const Generics::Time& now,
      const FreqCapIdList& fcs, // sorted
      const FreqCapIdList& uc_fcs, // sorted
      const FreqCapIdList& virtual_fcs, // sorted
      const SeqOrderList& seq_orders,
      const CampaignIds& campaign_ids,
      const CampaignIds& uc_campaign_ids,
      const FreqCapConfig& fc_config) throw();

    bool
    confirm_request(
      const Commons::RequestId& request_id,
      const Generics::Time& now,
      const FreqCapConfig& fc_config) throw();

    void
    merge(
      ConstSmartMemBufPtr merge_profile,
      const Generics::Time& now,
      const FreqCapConfig& fc_config)
      throw(eh::Exception);

    bool
    consider_publishers_optin(
      const std::set<unsigned long>& publisher_account_ids,
      const Generics::Time& timestamp) throw();
    
      void
    get_optin_publishers(
      std::list<unsigned long>& optin_publishers,
      const Generics::Time& time)
      throw(eh::Exception);
    
    void
    print(
      std::ostream& out,
      const FreqCapConfig* fc_config) const
      throw(eh::Exception);

    ConstSmartMemBuf_var
    transfer_membuf() throw()
    {
      return Generics::transfer_membuf(plain_profile_);
    };

  private:
    SmartMemBuf_var plain_profile_;
  };
}
}

#endif /*USERINFOCOMMONS_FREQCAPPROFILE_HPP*/
