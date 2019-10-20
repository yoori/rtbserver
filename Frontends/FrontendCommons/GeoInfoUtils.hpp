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

#ifndef GEOINFOUTILS_HPP__
#define GEOINFOUTILS_HPP__

#include <Commons/CorbaAlgs.hpp>
#include <CampaignSvcs/CampaignManager/CampaignManager.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>
#include "Location.hpp"

namespace FrontendCommons
{
  bool
  fill_geo_location_info(
    AdServer::CampaignSvcs::CampaignManager::GeoInfoSeq& location_seq,
    const Location* location);

  bool
  fill_geo_coord_location_info(
    AdServer::CampaignSvcs::CampaignManager::GeoCoordInfoSeq& coord_location_seq,
    const CoordLocation* coord_location);
}

namespace FrontendCommons
{
  inline
  bool
  fill_geo_location_info(
    AdServer::CampaignSvcs::CampaignManager::GeoInfoSeq& location_seq,
    const Location* location)
  {
    if (location)
    {
      location_seq.length(1);
      location_seq[0].country << location->country;
      location_seq[0].region << location->region;
      location_seq[0].city << location->city;

      return true;
    }

    return false;
  }

  inline
  bool
  fill_geo_coord_location_info(
    AdServer::CampaignSvcs::CampaignManager::GeoCoordInfoSeq& coord_location_seq,
    const CoordLocation* coord_location)
  {
    if (coord_location)
    {
      CORBA::ULong length = coord_location_seq.length();
      coord_location_seq.length(length + 1);

      coord_location_seq[length].longitude =
        CorbaAlgs::pack_decimal(coord_location->longitude);
      coord_location_seq[length].latitude =
        CorbaAlgs::pack_decimal(coord_location->latitude);
      coord_location_seq[length].accuracy =
        CorbaAlgs::pack_decimal(coord_location->accuracy);

      return true;
    }

    return false;
  }
}

#endif /*GEOINFOUTILS_HPP__*/

