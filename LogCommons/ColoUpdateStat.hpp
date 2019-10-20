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

#ifndef AD_SERVER_LOG_PROCESSING_COLO_UPDATE_STAT_HPP
#define AD_SERVER_LOG_PROCESSING_COLO_UPDATE_STAT_HPP

#include <iosfwd>
#include <algorithm>
#include <Generics/Time.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include "LogCommons.hpp"
#include "StatCollector.hpp"


namespace AdServer {
namespace LogProcessing {

class ColoUpdateStatKey
{
public:
  explicit
  ColoUpdateStatKey(unsigned long colo_id = 0) throw()
  :
    colo_id_(colo_id)
  {
  }

  bool operator==(const ColoUpdateStatKey& rhs) const throw()
  {
    return colo_id_ == rhs.colo_id_;
  }

  unsigned long colo_id() const throw()
  {
    return colo_id_;
  }

  size_t hash() const throw()
  {
    return colo_id_;
  }

  friend
  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ColoUpdateStatKey& key);

  friend
  std::ostream&
  operator<<(std::ostream& os, const ColoUpdateStatKey& key)
    throw(eh::Exception);

private:
  unsigned long colo_id_;
};

class ColoUpdateStatData
{
public:

  class Version: public StringIoWrapperOptional
  {
  public:
    Version() throw()
    {
      memset(subversions_, 0, sizeof(subversions_));
    }

    Version(const OptionalString& version) throw(eh::Exception)
    :
      StringIoWrapperOptional(version)
    {
      parse_();
    }

    bool
    operator<(const Version& other) const throw()
    {
      return std::lexicographical_compare(
        subversions_,
        subversions_ + sizeof(subversions_) / sizeof(subversions_[0]),
        other.subversions_,
        other.subversions_ + sizeof(subversions_) / sizeof(subversions_[0])
      );
    }

    friend FixedBufStream<TabCategory>&
    operator>>(FixedBufStream<TabCategory>& is, Version& version)
      throw(eh::Exception);

  private:
    void
    parse_() throw(eh::Exception)
    {
      memset(subversions_, 0, sizeof(subversions_));
      Stream::Parser parser(get());
      parser >> subversions_[0];
      for (std::size_t i = 1;
        i < sizeof(subversions_) / sizeof(subversions_[0]); ++i)
      {
        parser.get(); //Skip period
        parser >> subversions_[i];
      }
    }
    unsigned subversions_[4]; // major,minor,revision,build
  };

  ColoUpdateStatData() throw()
  :
    last_channels_update_(),
    last_campaigns_update_(),
    version_()
  {
  }

  ColoUpdateStatData(
    const OptionalSecondsTimestamp& last_channels_update,
    const OptionalSecondsTimestamp& last_campaigns_update,
    const OptionalString& version
  )
  :
    last_channels_update_(last_channels_update),
    last_campaigns_update_(last_campaigns_update),
    version_(version)
  {
  }

  bool operator==(const ColoUpdateStatData& rhs) const throw()
  {
    if (&rhs == this)
    {
      return true;
    }
    return last_channels_update_ == rhs.last_channels_update_ &&
      last_campaigns_update_ == rhs.last_campaigns_update_ &&
      version_ == rhs.version_;
  }

  ColoUpdateStatData& operator+=(const ColoUpdateStatData& rhs)
  {
    last_channels_update_ =
      std::max(last_channels_update_, rhs.last_channels_update_);
    last_campaigns_update_ =
      std::max(last_campaigns_update_, rhs.last_campaigns_update_);
    version_ = std::max(version_, rhs.version_);
    return *this;
  }

  const OptionalSecondsTimestamp& last_channels_update() const throw()
  {
    return last_channels_update_;
  }

  const OptionalSecondsTimestamp& last_campaigns_update() const throw()
  {
    return last_campaigns_update_;
  }

  const StringIoWrapperOptional&
  version() const throw()
  {
    return version_;
  }

  friend
  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ColoUpdateStatData& data);

  friend
  std::ostream&
  operator<<(std::ostream& os, const ColoUpdateStatData& data);

private:
  OptionalSecondsTimestamp last_channels_update_;
  OptionalSecondsTimestamp last_campaigns_update_;
  Version version_;
};

typedef StatCollector<ColoUpdateStatKey, ColoUpdateStatData, false, true>
  ColoUpdateStatCollector;

typedef LogDefaultTraits<ColoUpdateStatCollector, false> ColoUpdateStatTraits;

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_COLO_UPDATE_STAT_HPP */

