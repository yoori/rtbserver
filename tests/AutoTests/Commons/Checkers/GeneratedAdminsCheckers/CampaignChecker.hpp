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


#ifndef __AUTOTESTS_COMMONS_ADMINS_CAMPAIGNCHECKER_HPP
#define __AUTOTESTS_COMMONS_ADMINS_CAMPAIGNCHECKER_HPP

#include <tests/AutoTests/Commons/Admins/CampaignAdmin.hpp>
#include <tests/AutoTests/Commons/Checkers/AdminsChecker.hpp>
#include <tests/AutoTests/Commons/Checkers/Checker.hpp>

namespace AutoTest
{
  /*
   * @class CampaignChecker
   * @brief CampaignAdmin call checking wrapper
   */
  class CampaignChecker:  
    public AutoTest::Checker  
  {  
  public:

    enum NotPresentTag
    {
      NOT_PRESENT
    };

    typedef CampaignAdmin::Expected Expected;      
    
    /**
     * @brief Constructor.
     *
     * @param reference to global AutoTests params.
     * @param CCG id.
     * @param expected values set, if not defined simple check that campaign exists
     */
    CampaignChecker(
      BaseUnit* test,
      unsigned long ccg_id,
      const Expected& expected = Expected(),
      CampaignAdmin::Modificator expand = CampaignAdmin::EXPAND);

    CampaignChecker(
      BaseUnit* test,
      unsigned long ccg_id,
      NotPresentTag);
            
    virtual ~CampaignChecker() throw();
            
    bool check(bool throw_error = true) throw (CheckFailed, eh::Exception);      
            
  private:    

    BaseUnit* test_;      
    unsigned long ccg_id_;      
    Expected expected_;      
    CampaignAdmin::Modificator expand_channels_;
    AdminExistCheck exists_;
    static const Expected ANY_EXPECTED_;
  };  
}
#endif  // __AUTOTESTS_COMMONS_ADMINS_CAMPAIGNCHECKER_HPP

