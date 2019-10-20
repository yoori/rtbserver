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

#include "WebwiseDiscoverItems.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* WebwiseDiscoverItemsTraits::B::base_name_ = "WebwiseDiscoverItems";
template <> const char* WebwiseDiscoverItemsTraits::B::signature_ = "WebwiseDiscoverItems";
template <> const char* WebwiseDiscoverItemsTraits::B::current_version_ = "1.0";

std::istream&
operator>>(std::istream &is, WebwiseDiscoverItemsKey &key)
{
  is >> key.item_id_;
  if (is)
  {
    key.invariant();
    key.calc_hash();
  }
  return is;
}

std::ostream&
operator<<(std::ostream &os, const WebwiseDiscoverItemsKey &key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.item_id_;
  return os;
}

std::istream&
operator>>(std::istream &is, WebwiseDiscoverItemsData &data)
{
  data.holder_ = new WebwiseDiscoverItemsData::DataHolder();

  typedef OptionalValue<WebwiseDiscoverItemsData::StringT,
    Aux_::ClearableOptionalValueTraits<WebwiseDiscoverItemsData::StringT> >
      OptionalStringT;
  OptionalStringT opt_str;
  is >> opt_str;
  if (opt_str.present())
  {
    data.holder_->title = opt_str.get();
  }
  read_tab(is);
  is >> data.holder_->pub_date;
  read_tab(is);
  is >> opt_str;
  if (opt_str.present())
  {
    data.holder_->link = opt_str.get();
  }
  read_tab(is);
  is >> opt_str;
  if (opt_str.present())
  {
    data.holder_->language = opt_str.get();
  }
  if (data.holder_->language.length() > 3)
  {
    data.holder_->language.resize(3);
  }
  return is;
}

std::ostream&
operator<<(std::ostream &os, const WebwiseDiscoverItemsData &data)
{
  typedef OptionalValue<WebwiseDiscoverItemsData::StringT,
    Aux_::ClearableOptionalValueTraits<WebwiseDiscoverItemsData::StringT> >
      OptionalStringT;
  os << OptionalStringT(data.holder_->title) << '\t';
  os << data.holder_->pub_date << '\t';
  os << OptionalStringT(data.holder_->link) << '\t';
  os << OptionalStringT(data.holder_->language);
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

