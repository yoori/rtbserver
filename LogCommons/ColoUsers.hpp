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

#ifndef AD_SERVER_LOG_PROCESSING_COLO_USERS_HPP
#define AD_SERVER_LOG_PROCESSING_COLO_USERS_HPP


#include <LogCommons/LogCommons.hpp>
#include <LogCommons/StatCollector.hpp>

namespace AdServer {
namespace LogProcessing {

class ColoUsersKey
{
public:
  ColoUsersKey()
  :
    colo_id_(),
    created_(),
    hash_()
  {
  }

  ColoUsersKey(
    const OptionalDayTimestamp& sdate,
    const OptionalDayTimestamp& isp_sdate,
    unsigned long colo_id,
    unsigned long created
  )
  :
    sdate_(sdate),
    isp_sdate_(isp_sdate),
    colo_id_(colo_id),
    created_(created),
    hash_()
  {
    calc_hash_();
  }

  bool operator==(const ColoUsersKey& rhs) const
  {
    if (&rhs == this)
    {
      return true;
    }
    return sdate_ == rhs.sdate_ &&
      isp_sdate_ == rhs.isp_sdate_ &&
      colo_id_ == rhs.colo_id_ &&
      created_ == rhs.created_;
  }

  const OptionalDayTimestamp& sdate() const
  {
    return sdate_;
  }

  const OptionalDayTimestamp& isp_sdate() const
  {
    return isp_sdate_;
  }

  unsigned long colo_id() const
  {
    return colo_id_;
  }

  unsigned long created() const
  {
    return created_;
  }

  size_t hash() const
  {
    return hash_;
  }

  friend
  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ColoUsersKey& key);

  friend
  std::ostream&
  operator<<(std::ostream& os, const ColoUsersKey& key) throw(eh::Exception);

private:
  void calc_hash_()
  {
    Generics::Murmur64Hash hasher(hash_);
    hash_add(hasher, colo_id_);
    hash_add(hasher, created_);
    sdate_.get().hash_add(hasher);
    isp_sdate_.get().hash_add(hasher);
  }

  void invariant() const throw(eh::Exception)
  {
    if (sdate_.present() && isp_sdate_.present())
    {
      throw ConstraintViolation("ColoUsersKey::invariant(): "
        "either sdate or isp_sdate must be NULL");
    }
  }

  OptionalDayTimestamp sdate_;
  OptionalDayTimestamp isp_sdate_;

  unsigned long colo_id_;
  unsigned long created_;
  size_t hash_;
};

class ColoUsersData
{
public:
  ColoUsersData()
  :
    users_count_(),
    weekly_users_count_(),
    monthly_users_count_(),
    daily_network_users_count_(),
    monthly_network_users_count_()
  {
  }

  ColoUsersData(
    unsigned long users_count,
    unsigned long weekly_users_count,
    unsigned long monthly_users_count,
    unsigned long daily_network_users_count,
    unsigned long monthly_network_users_count
  )
  :
    users_count_(users_count),
    weekly_users_count_(weekly_users_count),
    monthly_users_count_(monthly_users_count),
    daily_network_users_count_(daily_network_users_count),
    monthly_network_users_count_(monthly_network_users_count)
  {
  }

  bool operator==(const ColoUsersData& rhs) const
  {
    if (&rhs == this)
    {
      return true;
    }
    return users_count_ == rhs.users_count_ &&
      weekly_users_count_ == rhs.weekly_users_count_ &&
      monthly_users_count_ == rhs.monthly_users_count_ &&
      daily_network_users_count_ == rhs.daily_network_users_count_ &&
      monthly_network_users_count_ == rhs.monthly_network_users_count_;
  }

  ColoUsersData& operator+=(const ColoUsersData& rhs)
  {
    users_count_ += rhs.users_count_;
    weekly_users_count_ += rhs.weekly_users_count_;
    monthly_users_count_ += rhs.monthly_users_count_;
    daily_network_users_count_ += rhs.daily_network_users_count_;
    monthly_network_users_count_ += rhs.monthly_network_users_count_;
    return *this;
  }

  unsigned long users_count() const
  {
    return users_count_;
  }

  unsigned long weekly_users_count() const
  {
    return weekly_users_count_;
  }

  unsigned long monthly_users_count() const
  {
    return monthly_users_count_;
  }

  unsigned long daily_network_users_count() const
  {
    return daily_network_users_count_;
  }

  unsigned long monthly_network_users_count() const
  {
    return monthly_network_users_count_;
  }

  friend
  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ColoUsersData& data);

  friend
  std::ostream&
  operator<<(std::ostream& os, const ColoUsersData& data);

private:
  unsigned long users_count_;
  unsigned long weekly_users_count_;
  unsigned long monthly_users_count_;
  unsigned long daily_network_users_count_;
  unsigned long monthly_network_users_count_;
};

typedef StatCollector<ColoUsersKey, ColoUsersData, false, true>
  ColoUsersCollector;

typedef LogDefaultTraits<ColoUsersCollector, false> ColoUsersTraits;

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_COLO_USERS_HPP */

