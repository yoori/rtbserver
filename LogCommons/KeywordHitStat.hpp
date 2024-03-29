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

#ifndef ADSERVER_LOGPROCESSING_KEYWORDHITSTAT_HPP_
#define ADSERVER_LOGPROCESSING_KEYWORDHITSTAT_HPP_

#include <iosfwd>
#include <sstream>
#include <Generics/Time.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include "LogCommons.hpp"
#include "StatCollector.hpp"
#include <Commons/StringHolder.hpp>

namespace AdServer
{
namespace LogProcessing
{
  class KeywordHitStatInnerKey
  {
  public:
    KeywordHitStatInnerKey()
      : keyword_()
    {}

    explicit
    KeywordHitStatInnerKey(
      AdServer::Commons::StringHolder* keyword)
      : keyword_(ReferenceCounting::add_ref(keyword))
    {
      calc_hash_();
    }

    bool
    operator==(const KeywordHitStatInnerKey& rhs) const
    {
      return keyword_->str() == rhs.keyword_->str();
    }

    const std::string&
    keyword() const
    {
      return keyword_->str();
    }

    size_t
    hash() const
    {
      return hash_;
    }

    friend std::istream&
    operator>>(std::istream& is, KeywordHitStatInnerKey& key);

    friend std::ostream&
    operator<<(std::ostream& os, const KeywordHitStatInnerKey& key);

  private:
    void
    calc_hash_()
    {
      Generics::Murmur64Hash hasher(hash_);
      hash_add(hasher, keyword_->str());
    }

  private:
    AdServer::Commons::StringHolder_var keyword_;
    size_t hash_;
  };

  class KeywordHitStatInnerData
  {
  public:
    KeywordHitStatInnerData()
      : hits_()
    {}

    KeywordHitStatInnerData(
      unsigned long hits)
      : hits_(hits)
    {}

    bool
    operator==(const KeywordHitStatInnerData& rhs) const
    {
      if(&rhs == this)
      {
        return true;
      }

      return hits_ == rhs.hits_;
    }

    KeywordHitStatInnerData&
    operator+=(const KeywordHitStatInnerData& rhs)
    {
      hits_ += rhs.hits_;
      return *this;
    }

    unsigned long
    hits() const
    {
      return hits_;
    }

    friend std::istream&
    operator>>(std::istream& is, KeywordHitStatInnerData& data);

    friend std::ostream&
    operator<<(std::ostream& os, const KeywordHitStatInnerData& data);

  private:
    unsigned long hits_;
  };

  struct KeywordHitStatKey
  {
    KeywordHitStatKey()
      : sdate_(), hash_()
    {}

    KeywordHitStatKey(const DayTimestamp& sdate)
      : sdate_(sdate),
        hash_()
    {
      calc_hash_();
    }

    bool
    operator==(const KeywordHitStatKey& rhs) const
    {
      if (&rhs == this)
      {
        return true;
      }

      return sdate_ == rhs.sdate_;
    }

    const DayTimestamp&
    sdate() const
    {
      return sdate_;
    }

    size_t
    hash() const
    {
      return hash_;
    }

    friend std::istream&
    operator>>(std::istream& is, KeywordHitStatKey& key);

    friend std::ostream&
    operator<<(std::ostream& os, const KeywordHitStatKey& key)
      throw(eh::Exception);

  private:
    void
    calc_hash_()
    {
      Generics::Murmur64Hash hasher(hash_);
      sdate_.hash_add(hasher);
    }

    DayTimestamp sdate_;
    size_t hash_;
  };

  typedef StatCollector<KeywordHitStatInnerKey, KeywordHitStatInnerData>
    KeywordHitStatInnerCollector;

  typedef KeywordHitStatInnerCollector KeywordHitStatData;

  typedef StatCollector<KeywordHitStatKey, KeywordHitStatData>
    KeywordHitStatCollector;

  struct KeywordHitStatTraits: LogDefaultTraits<KeywordHitStatCollector>
  {
    template <class FUNCTOR_>
    static
    void
    for_each_old(FUNCTOR_&) throw(eh::Exception)
    {}
  };

} // namespace LogProcessing
} // namespace AdServer

#endif /* ADSERVER_LOGPROCESSING_KEYWORDHITSTAT_HPP_ */

