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


#include "KeywordHitStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer
{
namespace LogProcessing
{
  template <> const char*
  KeywordHitStatTraits::B::base_name_ = "KeywordHitStat";

  template <> const char*
  KeywordHitStatTraits::B::signature_ = "KeywordHitStat";

  template <> const char*
  KeywordHitStatTraits::B::current_version_ = "3.5";

  std::istream&
  operator>>(std::istream& is, KeywordHitStatKey& key)
  {
    is >> key.sdate_;
    key.calc_hash_();
    return is;
  }

  std::ostream&
  operator<<(std::ostream& os, const KeywordHitStatKey& key)
    throw(eh::Exception)
  {
    os << key.sdate_;
    return os;
  }

  std::istream&
  operator>>(std::istream& is, KeywordHitStatInnerKey& key)
  {
    Aux_::StringIoWrapper keyword_wrapper;
    is >> keyword_wrapper;
    key.keyword_ = new AdServer::Commons::StringHolder(
      std::move(keyword_wrapper));
    key.calc_hash_();
    return is;
  }

  std::ostream&
  operator<<(std::ostream& os, const KeywordHitStatInnerKey& key)
  {
    os << Aux_::StringIoWrapper(key.keyword_->str());
    return os;
  }

  std::istream&
  operator>>(std::istream& is, KeywordHitStatInnerData& data)
  {
    is >> data.hits_;
    return is;
  }

  std::ostream&
  operator<<(std::ostream& os, const KeywordHitStatInnerData& data)
  {
    os << data.hits_;
    return os;
  }
} // namespace LogProcessing
} // namespace AdServer

