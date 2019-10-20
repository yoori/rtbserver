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

#ifndef _CORBA_CAMPAIGN_TYPES_HPP_
#define _CORBA_CAMPAIGN_TYPES_HPP_

#include "CampaignTypes.hpp"
#include <CampaignSvcs/CampaignCommons/CampaignCommons.hpp>
#include "CampaignSvcsVersionAdapter.hpp"

namespace AdServer
{
  namespace CampaignSvcs
  {
    template<typename SequenceType, typename ValueType>
    void
    fill_interval_sequence(
      SequenceType& target_seq,
      const Commons::IntervalSet<ValueType>& source)
      throw();

    template<typename SequenceType, typename ValueType>
    void
    convert_interval_sequence(
      Commons::IntervalSet<ValueType>& target,
      const SequenceType& source_seq)
      throw();

    void
    pack_delivery_limits(
      CampaignDeliveryLimitsInfo& delivery_info,
      const CampaignDeliveryLimits& delivery_limits)
      throw();

    void
    unpack_delivery_limits(
      CampaignDeliveryLimits& delivery_limits,
      const CampaignDeliveryLimitsInfo& delivery_info)
      throw();

    void
    pack_option_value_map(
      OptionValueSeq& token_seq,
      const OptionValueMap& tokens)
      throw();

    void
    unpack_option_value_map(
      OptionValueMap& tokens,
      const OptionValueSeq& token_seq)
      throw();
  }
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    template<typename SequenceType, typename ValueType>
    inline
    void
    fill_interval_sequence(
      SequenceType& target_seq,
      const Commons::IntervalSet<ValueType>& source)
      throw()
    {
      target_seq.length(source.size());
      CORBA::ULong i = 0;
      for(typename Commons::IntervalSet<ValueType>::const_iterator sit =
            source.begin(); sit != source.end(); ++sit, ++i)
      {
        target_seq[i].min = sit->min;
        target_seq[i].max = sit->max;
      }
    }

    template<typename SequenceType, typename ValueType>
    inline
    void
    convert_interval_sequence(
      Commons::IntervalSet<ValueType>& target,
      const SequenceType& source_seq)
      throw()
    {
      for(CORBA::ULong i = 0; i < source_seq.length(); ++i)
      {
        target.insert(Commons::Interval<ValueType>(
          source_seq[i].min, source_seq[i].max));
      }
    }

    inline
    void
    pack_delivery_limits(
      CampaignDeliveryLimitsInfo& delivery_info,
      const CampaignDeliveryLimits& delivery_limits)
      throw()
    {
      delivery_info.date_start = CorbaAlgs::pack_time(delivery_limits.date_start);
      delivery_info.date_end = CorbaAlgs::pack_time(delivery_limits.date_end);
      delivery_info.budget = CorbaAlgs::pack_optional_decimal(
        delivery_limits.budget);
      delivery_info.daily_budget = CorbaAlgs::pack_optional_decimal(
        delivery_limits.daily_budget);
      delivery_info.delivery_pacing = delivery_limits.delivery_pacing;
    }

    inline
    void
    unpack_delivery_limits(
      CampaignDeliveryLimits& delivery_limits,
      const CampaignDeliveryLimitsInfo& delivery_info)
      throw()
    {
      delivery_limits.date_start = CorbaAlgs::unpack_time(delivery_info.date_start);
      delivery_limits.date_end = CorbaAlgs::unpack_time(delivery_info.date_end);
      delivery_limits.budget = CorbaAlgs::unpack_optional_decimal<RevenueDecimal>(
        delivery_info.budget);
      delivery_limits.daily_budget = CorbaAlgs::unpack_optional_decimal<RevenueDecimal>(
        delivery_info.daily_budget);
      delivery_limits.delivery_pacing = delivery_info.delivery_pacing;
    }

    inline
    void
    pack_option_value_map(
      OptionValueSeq& token_seq,
      const OptionValueMap& tokens)
      throw()
    {
      token_seq.length(tokens.size());
      CORBA::ULong i = 0;
      for(OptionValueMap::const_iterator token_it = tokens.begin();
          token_it != tokens.end(); ++token_it, ++i)
      {
        token_seq[i].option_id = token_it->first;
        token_seq[i].value << token_it->second;
      }
    }

    inline
    void
    unpack_option_value_map(
      OptionValueMap& tokens,
      const OptionValueSeq& token_seq)
      throw()
    {
      for(CORBA::ULong i = 0; i < token_seq.length(); ++i)
      {
        tokens[token_seq[i].option_id] = token_seq[i].value.in();
      }
    }
  }
}

#endif /*_CORBA_CAMPAIGN_TYPES_HPP_*/
