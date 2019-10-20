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


#include "PageLoadsDailyStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* PageLoadsDailyStatTraits::B::base_name_ =
  "PageLoadsDailyStat";
template <> const char* PageLoadsDailyStatTraits::B::signature_ =
  "PageLoadsDailyStat";
template <> const char* PageLoadsDailyStatTraits::B::current_version_ =
  "3.3";

std::istream&
operator>>(std::istream& is, PageLoadsDailyStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const PageLoadsDailyStatKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, PageLoadsDailyStatInnerKey& key)
  throw(eh::Exception)
{
  key.holder_ = new PageLoadsDailyStatInnerKey::DataHolder();
  is >> key.holder_->site_id;
  is >> key.holder_->country;
  is >> key.holder_->tags;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const PageLoadsDailyStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.holder_->site_id << '\t';
  os << key.holder_->country << '\t';
  os << key.holder_->tags;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, PageLoadsDailyStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.page_loads_;
  is >> data.utilized_page_loads_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const PageLoadsDailyStatInnerData& data)
  throw(eh::Exception)
{
  os << data.page_loads_ << '\t';
  os << data.utilized_page_loads_;
  return os;
}

std::istream&
operator>>(std::istream& is, PageLoadsDailyStatKey_V_1_0& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

} // namespace LogProcessing
} // namespace AdServer

