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


#ifndef _AUTOTESTS_COMMONS_OPENRTBRESPONSECHECKER_IPP
#define _AUTOTESTS_COMMONS_OPENRTBRESPONSECHECKER_IPP

namespace AutoTest
{

  // OpenRTBResponse
  
  inline
  const OpenRTBResponse::Bids&
  OpenRTBResponse::bids() const
  {
    return bids_;
  }

  inline
  const std::string&
  OpenRTBResponse::id() const
  {
    return id_;
  }

  inline
  const std::string&
  OpenRTBResponse::currency() const
  {
    return currency_;
  }

  inline
  JsonParseStatus
  OpenRTBResponse::status() const
  {
    return status_;
  }

  // OpenRTBResponseChecker::Expected
  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::id(
    const std::string& val)
  {
    id_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::impid(
    const std::string& val)
  {
    impid_ = val;
    return *this;
  }
  
  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::price(
    Money val)
  {
    price_ = val;
    return *this;
  }
  
  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::adid(
    unsigned long val)
  {
    adid_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::crid(const std::string& val)
  {
    crid_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::adomain(
    const std::string& val)
  {
    adomain_->push_back(val);
    adomain_.is_set(true);
    return *this;
  }
  
  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::adm(
    const std::string& val)
  {
    adm_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::nurl(
    const std::string& val)
  {
    nurl_ = val;
    return *this;
  }    
  
  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::cid(
    unsigned long val)
  {
    cid_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::attr(
    unsigned long val)
  {
    attr_->push_back(val);
    attr_.is_set(true);
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::ad_ox_cats(
    unsigned long val)
  {
    ad_ox_cats_->push_back(val);
    ad_ox_cats_.is_set(true);
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::cat(
    const std::string& val)
  {
    cat_->push_back(val);
    cat_.is_set(true);
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::fmt(
    unsigned long val)
  {
    fmt_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::matching_ad_id(
    const std::string& val)
  {
    matching_ad_id_ = val;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::fmt_exist(
    bool value)
  {
    fmt_exist_ = value;
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::cat_exist(
    bool value)
  {
    
    cat_exist_ = value;
    return *this;    
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::cat_checked()
  {
    cat_.is_set(true);
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::attr_exist(
    bool value)
  {
    
    attr_exist_ = value;
    return *this;    
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::attr_checked()
  {
    attr_.is_set(true);
    return *this;
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::ad_ox_cats_exist(
    bool value)
  {
    
    ad_ox_cats_exist_ = value;
    return *this;    
  }

  inline
  OpenRTBResponseChecker::Expected&
  OpenRTBResponseChecker::Expected::ad_ox_cats_checked()
  {
    ad_ox_cats_.is_set(true);
    return *this;
  }
}
#endif  // _AUTOTESTS_COMMONS_OPENRTBRESPONSECHECKER_IPP
