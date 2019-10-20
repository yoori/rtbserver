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


#ifndef _AUTOTEST__OPENRTBEXTENSIONSTEST_
#define _AUTOTEST__OPENRTBEXTENSIONSTEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace
{
  typedef AutoTest::OpenRTBRequest RTBRequest;
};

class OpenRTBExtensionsTest : public BaseDBUnit
{

  typedef AutoTest::OpenRTBResponseChecker OpenRTBResponseChecker;
  typedef OpenRTBResponseChecker::Expected Expected;
  typedef OpenRTBResponseChecker::ExpectedList ExpectedList;

  class IExpectedSetter: public ReferenceCounting::AtomicImpl
  {
  public:
    virtual void set(Expected&, const std::string&) = 0;
  };

  typedef ReferenceCounting::SmartPtr<IExpectedSetter> IExpectedSetter_var;

  template<typename T>
  class ExpectedSetter: public IExpectedSetter
  {
    typedef Expected& (Expected::* SetterFunc)(T);
  public:
    ExpectedSetter(SetterFunc f): f_(f) {};

    void set(Expected& expected, const std::string& val)
    {
      (expected.*f_)(valueof<T>(val));
    }

  private:
    SetterFunc f_;
  };

  class ExpectedSetterHolder
  {
  public:
    ExpectedSetterHolder(std::nullptr_t f = nullptr):
      setter_(f)
    {}

    template<typename T>
    ExpectedSetterHolder(Expected& (Expected::* f)(T)):
      setter_(new ExpectedSetter<T>(f))
    {}

    ~ExpectedSetterHolder()
    {}

    bool empty() const
    {
      return setter_ == nullptr;
    }

    void operator() (Expected& expected, const std::string& val) const
    {
      setter_->set(expected, val);
    }

  private:
    IExpectedSetter_var setter_;
  };

  struct RequestParam
  {
    RequestParam(const RTBRequest::Member& m, const char* v, bool n = false): member(m), value(v), need_fetch(n) {}

    RTBRequest::Member member;
    const char*        value;
    bool               need_fetch;
  };

  typedef std::list<RequestParam> RequestParams;

  struct ImpParam
  {
      const char* id;
      const char* mimes;
      const char* minduration;
      const char* maxduration;
      const char* protocol;
      const char* playbackmethod;
      const char* startdelay;
      const char* linearity;
      const char* ext_adtype;
  };

  typedef std::vector<ImpParam> ImpParams;

  struct ExpectedValue
  {
    ExpectedSetterHolder setter;
    const char* value;
    bool need_fetch;
  };

  //typedef std::pair<ExpectedSetterHolder, const char*> ExpectedValue;
  typedef std::list<ExpectedValue> ExpectedValues;
  typedef std::list<ExpectedValues> ExpectedValuesList;

  struct CaseRequest
  {
    // Request params
    const char* aid;
    const char* src;
    const char* size;
    const char* referer;
    // JSON parameter
    RequestParams request_params;
    //RTBRequest::Member param_setter;
    //const char* request_value;
    // Response
    const char* adid;
    ExpectedValues expected_values;
    //ExpectedSetterHolder exp_setter;
    //const char* response_value;
  };

  struct MultislotCaseRequest
  {
    // Request params
    const char* aid;
    const char* src;
    const char* referer;
    // JSON parameter
    ImpParams request_params;
    // Response
    ExpectedValuesList expected_values_list;
  };

public:
  OpenRTBExtensionsTest(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseDBUnit(stat_var, task_name, params_var)
  { }

  virtual ~OpenRTBExtensionsTest() throw()
  { }

private:

  static const ExpectedSetterHolder NULL_EXPECTED_SETTER;
  static const AutoTest::OpenRTBRequest::Member NULL_REQUEST_SETTER;

  static const CaseRequest OPENX_REQUESTS[];
  static const CaseRequest OPENX_REQUESTS_SLOW[];
  static const CaseRequest VAST_REQUESTS[];
  static const MultislotCaseRequest VAST_MULTISLOT_REQUESTS[];
  static const CaseRequest VAST_NEGATIVE_REQUESTS[];
  static const CaseRequest ALLYES_REQUESTS[];

  // Cases
  void openx_slow_();

  void pre_condition();
  bool run();
  void post_condition();
  void tear_down();

  // helpers
  template<size_t COUNT>
  void
  process_requests_(
    const CaseRequest (&cases)[COUNT], bool send_banner = true);

  template<size_t COUNT>
  void process_multislotcase_requests_(const MultislotCaseRequest (&cases)[COUNT]);
};

#endif // _AUTOTEST__OPENRTBEXTENSIONSTEST_
