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


#ifndef __AUTOTESTS_COMMONS_STATS_COMMON_HPP
#define __AUTOTESTS_COMMONS_STATS_COMMON_HPP

#include <tests/AutoTests/Commons/Stats/ORMStats.hpp>
#include <tests/AutoTests/Commons/Stats/StatsContainers.hpp>
#include <tests/AutoTests/Commons/Stats/DiffStats.hpp>

#include <tests/AutoTests/Commons/Stats/ChannelInventoryStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelInventoryByCPMStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelOverlapUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/AdvertiserUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/CampaignUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/CCGUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/CCUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/SiteUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/HourlyStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelImpInventory.hpp>
#include <tests/AutoTests/Commons/Stats/UserPropertiesStats.hpp>
#include <tests/AutoTests/Commons/Stats/CMPRequestStats.hpp>
#include <tests/AutoTests/Commons/Stats/PageLoadsDaily.hpp>
#include <tests/AutoTests/Commons/Stats/ExpressionPerformanceStats.hpp>
#include <tests/AutoTests/Commons/Stats/PublisherInventory.hpp>
#include <tests/AutoTests/Commons/Stats/ColoUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/CreatedUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/GlobalColoUserStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelInventoryEstimStats.hpp>
#include <tests/AutoTests/Commons/Stats/WebStats.hpp>
#include <tests/AutoTests/Commons/Stats/OptOutStats.hpp>
#include <tests/AutoTests/Commons/Stats/PassbackStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelPerformance.hpp>
#include <tests/AutoTests/Commons/Stats/CCGKeywordStatsHourly.hpp>
#include <tests/AutoTests/Commons/Stats/CCGKeywordStatsTotal.hpp>
#include <tests/AutoTests/Commons/Stats/CCGKeywordStatsDaily.hpp>
#include <tests/AutoTests/Commons/Stats/CCGKeywordStatsToW.hpp>
#include <tests/AutoTests/Commons/Stats/ActionStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelUsageStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelIdBasedStats.hpp>
#include <tests/AutoTests/Commons/Stats/ColoIdBasedStats.hpp>
#include <tests/AutoTests/Commons/Stats/SiteIdBasedStats.hpp>
#include <tests/AutoTests/Commons/Stats/TagIdBasedStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelCountStats.hpp>
#include <tests/AutoTests/Commons/Stats/AdvertiserStatsDaily.hpp>
#include <tests/AutoTests/Commons/Stats/PublisherStatsDaily.hpp>
#include <tests/AutoTests/Commons/Stats/ActionRequests.hpp>
#include <tests/AutoTests/Commons/Stats/SiteChannelStats.hpp>
#include <tests/AutoTests/Commons/Stats/ChannelTriggerStats.hpp>
#include <tests/AutoTests/Commons/Stats/CcgAuctionStatsDaily.hpp>

#endif  // __AUTOTESTS_COMMONS_STATS_COMMON_HPP

