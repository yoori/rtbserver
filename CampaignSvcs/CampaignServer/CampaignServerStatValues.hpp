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

#ifndef CAMPAIGN_SERVER_STAT_VALUES_HPP
#define CAMPAIGN_SERVER_STAT_VALUES_HPP

#include <Generics/Time.hpp>
#include <CORBACommons/StatsImpl.hpp>

namespace AdServer
{
  namespace CampaignSvcs
  {
    class CampaignConfig;
    extern const Generics::Values::Key LAST_DB_UPDATE_TIME;
    extern const Generics::Values::Key LAST_DB_UPDATE_DELAY;

    class CampaignServerStatValues : public Generics::Values
    {
      template <typename Type>
      Type
      get(const Key& key) const
        throw (eh::Exception, KeyNotFound, InvalidType);

    public:
      using Generics::Values::get;

      /**
       * Getter
       * @param key key to search
       * @value value associated with the key (if any)
       * @return if the operation has been completed successfully or not
       */
      bool
      get(const Key& key, Generics::Values::Floating& value) const
        throw (eh::Exception, InvalidType);

      /**
       * Calls functor for each value stored.
       * @param functor functor to call for each value
       */
      template <typename Functor>
      void
      enumerate_all(Functor& functor) const throw (eh::Exception);

      void
      fill_values(CampaignConfig* new_config) throw (eh::Exception);

    protected:
      virtual
      ~CampaignServerStatValues() throw () = default;
    };
  }
}


namespace AdServer
{
  namespace CampaignSvcs
  {

    inline bool
    CampaignServerStatValues::get(const Key& key,
      Generics::Values::Floating& value) const
      throw (eh::Exception, InvalidType)
    {
      if (key == LAST_DB_UPDATE_DELAY)
      {
        bool result =
          Generics::Values::get<Generics::Values::Floating>(
            LAST_DB_UPDATE_TIME, value);
        if (result)
        {
          value = Generics::Time::get_time_of_day().as_double() - value;
        }
        return result;
      }
      return Generics::Values::get<Generics::Values::Floating>(key, value);
    }

    template <typename Functor>
    void
    CampaignServerStatValues::enumerate_all(Functor& functor) const
      throw (eh::Exception)
    {
      Sync::PosixGuard guard(mutex_);
      functor(data_.size());
      for (Data::const_iterator itor(data_.begin());
        itor != data_.end(); ++itor)
      {
        enumerate_one_(*itor, functor);
      }
      const Generics::Values::Floating* member =
        get_<Generics::Values::Floating>(LAST_DB_UPDATE_TIME);
      if (member)
      {
        functor(LAST_DB_UPDATE_DELAY,
          Generics::Time::get_time_of_day().as_double() - *member);
      }
    }
  }
}

#endif // CAMPAIGN_SERVER_STAT_VALUES_HPP
