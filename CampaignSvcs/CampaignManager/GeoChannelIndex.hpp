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

#ifndef GEOCHANNELINDEX_HPP
#define GEOCHANNELINDEX_HPP

#include <eh/Exception.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Commons/StringHolder.hpp>

#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  class GeoChannelIndex: public ReferenceCounting::AtomicCopyImpl
  {
  public:
    struct Key
    {
    public:
      Key(Commons::StringHolder* country,
        Commons::StringHolder* region,
        Commons::StringHolder* city)
        throw();

      bool operator==(const Key& right) const throw();

      unsigned long hash() const throw();

      const std::string& country() const;

      const std::string& region() const;

      const std::string& city() const;

    private:
      const Commons::StringHolder_var country_;
      const Commons::StringHolder_var region_;
      const Commons::StringHolder_var city_;
      unsigned long hash_;
    };

    typedef Generics::GnuHashSet<
      Generics::NumericHashAdapter<unsigned long> >
      GeoChannelIdSet;

    typedef Generics::GnuHashTable<Key, unsigned long> GeoChannelMap;

  public:
    GeoChannelIndex() throw();

    void add(const String::SubString& country,
      const String::SubString& region,
      const String::SubString& city,
      unsigned long channel_id)
      throw();

    void close() throw();

    /* return channels in priority decreasing order : most accurate at begin */
    void match(
      ChannelIdList& result_channels,
      const String::SubString& country,
      const String::SubString& region,
      const String::SubString& city)
      throw();

    const GeoChannelMap& channels() const throw();

    const GeoChannelIdSet& channel_ids() const throw();

  protected:
    virtual
    ~GeoChannelIndex() throw();

  private:
    Commons::StringHolder_var
    resolve_name_(const String::SubString& name) throw();

  private:
    GeoChannelMap channels_;
    GeoChannelIdSet channel_ids_;
    Commons::StringHolder_var empty_string_;

  public:
    typedef Generics::GnuHashSet<Commons::StringHolderHashAdapter>
      NameSet;

    // all_names_: hold strings for SubString references and decrease memory usage
    NameSet all_names_;
  };

  typedef ReferenceCounting::SmartPtr<GeoChannelIndex>
    GeoChannelIndex_var;

  class GeoCoordChannelIndex: public ReferenceCounting::AtomicImpl
  {
  public:
    struct Key
    {
      Key(const CoordDecimal& longitude_val,
        const CoordDecimal& latitude_val,
        const AccuracyDecimal& accuracy_val)
        : longitude(longitude_val),
          latitude(latitude_val),
          accuracy(accuracy_val),
          hash_(0)
      {}

      CoordDecimal longitude;
      CoordDecimal latitude;
      AccuracyDecimal accuracy;

      bool operator==(const Key& right) const
      {
        return longitude == right.longitude &&
          latitude == right.latitude &&
          accuracy == right.accuracy;
      }

      size_t hash() const
      {
        return hash_;
      }

    protected:
      void calc_hash_()
      {
        Generics::Murmur64Hash hasher(hash_);
        hash_add(hasher, longitude);
        hash_add(hasher, latitude);
        hash_add(hasher, accuracy);
      }

    protected:
      size_t hash_;
    };

    struct Value: public ReferenceCounting::AtomicImpl
    {
      ChannelIdList channels;

    protected:
      virtual ~Value() throw()
      {}
    };

    typedef ReferenceCounting::SmartPtr<Value> Value_var;

    typedef Generics::GnuHashTable<Key, Value_var> ChannelMap;

  public:
    GeoCoordChannelIndex() throw();

    void
    add(const Key& key,
      unsigned long channel_id)
      throw();

    void
    match(
      ChannelIdSet& result_channels,
      const CoordDecimal& longitude,
      const CoordDecimal& latitude,
      const AccuracyDecimal& accuracy) const
      throw();

    const ChannelMap&
    channels() const throw();

    const ChannelIdSet&
    channel_ids() const throw();

  protected:
    class CoordIndexImpl;

  protected:
    virtual
    ~GeoCoordChannelIndex() throw();

  private:
    ChannelIdSet channel_ids_;
    ChannelMap channels_;
    std::unique_ptr<CoordIndexImpl> coord_index_impl_;
  };

  typedef ReferenceCounting::SmartPtr<GeoCoordChannelIndex>
    GeoCoordChannelIndex_var;
}
}

namespace AdServer
{
namespace CampaignSvcs
{
  inline
  GeoChannelIndex::Key::Key(
    Commons::StringHolder* country,
    Commons::StringHolder* region,
    Commons::StringHolder* city)
    throw()
    : country_(ReferenceCounting::add_ref(country)),
      region_(ReferenceCounting::add_ref(region)),
      city_(ReferenceCounting::add_ref(city))
  {
    hash_ = Generics::CRC::quick(0, country_->str().data(), country_->str().length());
    hash_ = Generics::CRC::quick(hash_, region_->str().data(), region_->str().length());
    hash_ = Generics::CRC::quick(hash_, city_->str().data(), city_->str().length());
  }

  inline
  bool
  GeoChannelIndex::Key::operator==(const Key& right) const throw()
  {
    return *country_ == *right.country_ &&
      *region_ == *right.region_ &&
      *city_ == *right.city_;
  }

  inline
  unsigned long
  GeoChannelIndex::Key::hash() const throw()
  {
    return hash_;
  }

  inline
  const std::string&
  GeoChannelIndex::Key::country() const
  {
    return country_->str();
  }

  inline
  const std::string&
  GeoChannelIndex::Key::region() const
  {
    return region_->str();
  }

  inline
  const std::string&
  GeoChannelIndex::Key::city() const
  {
    return city_->str();
  }

  inline
  Commons::StringHolder_var
  GeoChannelIndex::resolve_name_(const String::SubString& name)
    throw()
  {
    Commons::StringHolder_var str(new Commons::StringHolder(name));
    return all_names_.insert(Commons::StringHolderHashAdapter(str)).first->value();
  }

  inline
  void
  GeoChannelIndex::add(
    const String::SubString& country,
    const String::SubString& region,
    const String::SubString& city,
    unsigned long channel_id)
    throw()
  {
    Commons::StringHolder_var packed_country = resolve_name_(country);
    Commons::StringHolder_var packed_region = resolve_name_(region);
    Commons::StringHolder_var packed_city = resolve_name_(city);
    channels_[Key(packed_country, packed_region, packed_city)] = channel_id;
    channel_ids_.insert(channel_id);
  }

  inline
  void
  GeoChannelIndex::close() throw()
  {
    all_names_.clear();
  }

  inline
  void
  GeoChannelIndex::match(
    ChannelIdList& result_channels,
    const String::SubString& country_val,
    const String::SubString& region_val,
    const String::SubString& city_val)
    throw()
  {
    Commons::StringHolder_var country =
      new Commons::StringHolder(country_val);
    GeoChannelMap::const_iterator ind_it;

    if(region_val[0] || city_val[0])
    {
      Commons::StringHolder_var region =
        new Commons::StringHolder(region_val);

      if(city_val[0])
      {
        Commons::StringHolder_var city =
          new Commons::StringHolder(city_val);

        ind_it = channels_.find(Key(country, region, city));

        if(ind_it != channels_.end())
        {
          result_channels.push_back(ind_it->second);
        }
      }

      if(region_val[0])
      {
        ind_it = channels_.find(Key(country, region, empty_string_));

        if(ind_it != channels_.end())
        {
          result_channels.push_back(ind_it->second);
        }
      }
    }

    ind_it = channels_.find(Key(country, empty_string_, empty_string_));
    if(ind_it != channels_.end())
    {
      result_channels.push_back(ind_it->second);
    }
  }

  inline
  const GeoChannelIndex::GeoChannelMap&
  GeoChannelIndex::channels() const throw()
  {
    return channels_;
  }

  inline
  const GeoChannelIndex::GeoChannelIdSet&
  GeoChannelIndex::channel_ids() const throw()
  {
    return channel_ids_;
  }

  // GeoCoordChannelIndex
  inline
  const GeoCoordChannelIndex::ChannelMap&
  GeoCoordChannelIndex::channels() const throw()
  {
    return channels_;
  }

  inline
  const ChannelIdSet&
  GeoCoordChannelIndex::channel_ids() const throw()
  {
    return channel_ids_;
  }
}
}

#endif /*GEOCHANNELINDEX_HPP*/
