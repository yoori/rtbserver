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

// @file CampaignServerStatValues.cpp
#include "CampaignServerStatValues.hpp"
#include "CampaignConfig.hpp"
#include "CampaignServerImpl.hpp"

namespace
{
  const AdServer::CampaignSvcs::ProcStatImpl::Key CAMPAIGN_COUNT(
    "campaignCount");
  const AdServer::CampaignSvcs::ProcStatImpl::Key TAG_COUNT(
    "tagCount");
  const AdServer::CampaignSvcs::ProcStatImpl::Key BEHAV_CHANNEL_COUNT(
    "behavChannelCount");
  const AdServer::CampaignSvcs::ProcStatImpl::Key EXPRESSION_CHANNEL_COUNT(
    "expressionChannelCount");
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    const AdServer::CampaignSvcs::ProcStatImpl::Key LAST_DB_UPDATE_TIME(
      "lastDBUpdateTime");
    const AdServer::CampaignSvcs::ProcStatImpl::Key LAST_DB_UPDATE_DELAY(
      "lastDBUpdateDelay");

    void
    CampaignServerStatValues::fill_values(CampaignConfig* new_config)
      throw (eh::Exception)
    {
      set(CAMPAIGN_COUNT,
        static_cast<unsigned long>(new_config->campaigns.active().size()));
      set(TAG_COUNT,
        static_cast<unsigned long>(new_config->tags.active().size()));
      set(BEHAV_CHANNEL_COUNT,
        static_cast<unsigned long>(
          new_config->simple_channels.active().size()));
      set(LAST_DB_UPDATE_TIME, new_config->master_stamp.as_double());
      set(EXPRESSION_CHANNEL_COUNT,
        static_cast<unsigned long>(
          new_config->expression_channels.active().size()));
    }

  }
}
