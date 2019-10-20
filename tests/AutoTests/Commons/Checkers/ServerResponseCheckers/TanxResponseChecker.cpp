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


#include "TanxResponseChecker.hpp"

namespace AutoTest
{
  namespace ProtoBuf
  {
    // Expected
    
    BaseTraits<TanxResponseChecker>::Expected::Expected()
    { }

    BaseTraits<TanxResponseChecker>::Expected::Expected(
      TanxResponseChecker& checker,
      const Expected& other) :
      creative_id_(
        Utils().add_checker(
        checker, "creative_id",
        other.creative_id_, &Ad::creative_id)),
      destination_url_(
        Utils().add_seq_checker(
          checker, "destination_url",
          other.destination_url_, &Ad::destination_url)),
      ad_bid_count_idx_(
        Utils().add_checker(
          checker, "ad_bid_count_idx",
          other.ad_bid_count_idx_, &Ad::ad_bid_count_idx)),
      category_(
        Utils().add_seq_checker(
          checker, "category",
          other.category_, &Ad::category)),
      creative_type_(
        Utils().add_seq_checker(
          checker, "creative_type",
          other.creative_type_, &Ad::creative_type)),
      max_cpm_price_(
        Utils().add_checker(
          checker, "max_cpm_price",
          other.max_cpm_price_, &Ad::max_cpm_price))
    { }

    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::creative_id(
      const std::string& val)
    {
      creative_id_ = val;
      return *this;
    }
    
    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::destination_url(
      const std::string& val)
    {
      destination_url_->push_back(val);
      destination_url_.is_set(true);
      return *this;
    }
    
    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::ad_bid_count_idx(
      unsigned long val)
    {
      ad_bid_count_idx_ = val;
      return *this;
    }

    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::category(
      unsigned long val)
    {
      category_->push_back(val);
      category_.is_set(true);
      return *this;
    }
    
    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::creative_type(
      unsigned long val)
    {
      creative_type_->push_back(val);
      creative_type_.is_set(true);
      return *this;
    }

    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::max_cpm_price(
      AutoTest::Money val)
    {
      max_cpm_price_ = val;
      return *this;
    }

    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::category_checked()
    {
      category_.is_set(true);
      return *this;
    }
    
    BaseTraits<TanxResponseChecker>::Expected&
    BaseTraits<TanxResponseChecker>::Expected::creative_type_checked()
    {
      creative_type_.is_set(true);
      return *this;
    }
  }
    
  // TanxResponseChecker
  
  TanxResponseChecker::TanxResponseChecker(
    const AdClient& client,
    const Expected& expected,
    size_t creative_num) :
    Base(client, expected, creative_num)
  {
    response_.ParseFromString(client.req_response_data());
  }

  TanxResponseChecker::~TanxResponseChecker() throw()
  { }

  const TanxResponseChecker::Ad&
  TanxResponseChecker::ad() const
  {
    return response_.ads(creative_num_);
  }

  TanxResponseChecker::size_type
  TanxResponseChecker::ad_size() const
  {
    return response_.ads_size();
  }

}

