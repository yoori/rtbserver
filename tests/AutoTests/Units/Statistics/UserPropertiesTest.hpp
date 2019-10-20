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


#ifndef _UNITTEST__USERPROPERTIESTEST_
#define _UNITTEST__USERPROPERTIESTEST_

#include <tests/AutoTests/Commons/Common.hpp>

typedef AutoTest::NSLookupRequest  NSLookupRequest;

/**
 * @class UserPropertiesTest
 * @brief Check user properties logging
 */
class UserPropertiesTest: public BaseDBUnit
{

  struct PropertyKey
  {
    std::string name;
    std::string user_status;
    std::string value;

    const std::string key() const;

    bool operator<(const PropertyKey &other) const;
  };
  
  typedef std::set<PropertyKey> PropertyKeys;
  
public:

  enum PropertyStatusEnum
  {
    PSE_OPTIN,         // Optin user
    PSE_NON_OPTIN,     // User without UID
    PSE_OPTOUT,        // Optout user
    PSE_TEMPORATY,     // Temporary user
    PSE_PROBE          // Probe user
  };

  /**
   * @class Property
   * @brief Describe scenario for one property
   */
  struct Property {
    const char* exp_properties;            // UserProperties.property_name
    NSLookupRequest::Member request_param; // Nslookup parameter used for enabling property
    const char* request_param_value;       // Parameter value
    const char* tid;                       // Nslookup.tid
    const char* referer_kw;                // Nslookup.referer_kw
    const char* expected_ccid;             // Expected DebugInfo.ccid
    PropertyStatusEnum user_type;          // User type (optin, optout, non-optin, temporary)
    unsigned long requests;                // Expected UserProperties.requests
    unsigned long impressions;             // Expected UserProperties.impression
    unsigned long clicks;                  // Expected UserProperties.clicks
    unsigned long actions;                 // Expected UserProperties.actions
    unsigned long imps_unverified;         // Expected UserProperties.imps_unverified
    unsigned long profiling_requests;      // Expected UserProperties.profiling_requests
    
  };

  typedef unsigned long Property::* Counter;
  
public:
 
  UserPropertiesTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var): 
    BaseDBUnit(stat_var, task_name, params_var)
  { }
 
  virtual ~UserPropertiesTest() throw()
  { }


protected:

  virtual bool run();

  virtual void set_up();
  
  virtual void tear_down();


private:

  AutoTest::Time today;

  // Helper functions

  template<unsigned long PropsSize>
  unsigned long get_count(
    const Property (&properties)[PropsSize],
    const PropertyKey& property,
    Counter counter);

  // Common function for all test cases
  template<unsigned long PropsSize>
    void test_case(
      const Property (&properties)[PropsSize],
      const char* colo_name);

  // Cases

  void os_browser_case();

  void country_case();

  void version_case();

  void app_props_case();

  void user_status_case();
  
  void inactive_tag_case();
  
  void no_ads_isp_case();

  void probe_case();

  void upvalue_case();

};

#endif  // __USERPROPERTIESTEST_HPP
