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


#include <tests/AutoTests/Commons/Admins/AdminsContainer.hpp>
#include "CampaignChecker.hpp"
#include <tests/AutoTests/Commons/Checkers/CompositeCheckers.hpp>

#include <tests/AutoTests/Commons/Stats/ORMStats.hpp>

namespace AutoTest
{

  const CampaignChecker::Expected CampaignChecker::ANY_EXPECTED_;

  CampaignChecker::CampaignChecker(
    BaseUnit* test,
    unsigned long ccg_id,
    const Expected& expected, 
    CampaignAdmin::Modificator expand_channels)
    : test_(test),
      ccg_id_(ccg_id),
      expected_(expected),
      expand_channels_(expand_channels),
      exists_(AEC_EXISTS)
  {}

  CampaignChecker::CampaignChecker(
    BaseUnit* test,
    unsigned long ccg_id,
    NotPresentTag)
    : test_(test),
      ccg_id_(ccg_id),
      expected_(ANY_EXPECTED_),
      expand_channels_(CampaignAdmin::EXPAND),
      exists_(AEC_NOT_EXISTS)
  {}

  CampaignChecker::~CampaignChecker() throw()
  {}

  bool CampaignChecker::check(bool throw_error)
    throw(CheckFailed, eh::Exception)
  {

    AdminExistCheck remote_exists =
      expected_.force_remote_present() ||
        ((!expected_.has_status() || expected_.status() == "A") &&
          (!expected_.has_eval_status() || expected_.eval_status() == "A")) ?
            exists_: AEC_NOT_EXISTS;

    AdminsArray<CampaignAdmin, CT_ALL> central_admins;

    central_admins.initialize(test_,
      CTE_CENTRAL, STE_CAMPAIGN_MANAGER,
      ccg_id_, expand_channels_);
    
    AdminsArray<CampaignAdmin, CT_ALL> remote_admins;

    remote_admins.initialize(test_,
      CTE_ALL_REMOTE, STE_CAMPAIGN_MANAGER,
      ccg_id_, expand_channels_);

    if (central_admins.empty() && remote_admins.empty())
    {
      throw CheckFailed("Admin containers are empty!");
    }

    return
      (central_admins.empty() ||
        admin_checker(
          central_admins,
          expected_,
          exists_).
            check(throw_error)) &&
      (remote_admins.empty() ||
        admin_checker(
          remote_admins,
          remote_exists == AEC_EXISTS? expected_: ANY_EXPECTED_,
          remote_exists).
            check(throw_error));
  }
}
