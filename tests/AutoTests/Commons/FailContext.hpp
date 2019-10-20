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


#ifndef _AUTOTESTS_COMMONS_FAILCONTEXT_HPP
#define _AUTOTESTS_COMMONS_FAILCONTEXT_HPP

#include <tests/AutoTests/Commons/Utils.hpp>

/** Help macroses for define test fail context (stack + notes) */

namespace AutoTest
{
  const size_t length_valid_description_length = 6;

  class ValidateDescriptionLength
  {
  public:
    template<size_t Count>
    ValidateDescriptionLength(
      const char (&) [Count],
      const unsigned int,
      const char*)
    {
      static_assert(Count > (length_valid_description_length-1),
        "description length is not valid");
    }

    ValidateDescriptionLength(
      const String::SubString&,
      const unsigned int,
      const char*)
    {}
  };

  //DECLARE_EXCEPTION(CheckFailed, eh::DescriptiveException);
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  void fail_message(
    Stream::Error& out,
    const String::SubString& description,
    const char* file_name,
    const char* function_name,
    const unsigned int source_line,
    const char* notes)
    throw();

  void fail(
    const String::SubString& description,
    const char* file_name,
    const char* function_name,
    const unsigned int source_line,
    const char* notes)
    throw(Exception);

}

// only one argument allowed at ... places : const char* or std::string
#define FAIL_CONTEXT(call, ...) \
  { \
    try \
    { \
      call; \
    } \
    catch(const eh::Exception& ex) \
    { \
      AutoTest::fail( \
        String::SubString(ex.what()), \
        __FILE__, \
        __FUNCTION__, \
        __LINE__, \
        std::string(__VA_ARGS__).c_str());  \
    } \
  };

#define NOSTOP_FAIL_CONTEXT(call, ...) \
  { \
    try \
    { \
      call; \
    } \
    catch(const eh::Exception& ex) \
    { \
      Stream::Error msg; \
      AutoTest::fail_message( \
        msg, \
        String::SubString(ex.what()), \
        __FILE__, \
        __FUNCTION__, \
        __LINE__, \
        std::string(__VA_ARGS__).c_str()); \
      AutoTest::Logger::thlog().log(msg.str(), Logging::Logger::ERROR); \
      msg.str().append_to(stat_.error); \
      stat_.error += "\n"; \
    } \
  };

// Testcase macros.
// Testcase have own logger.
#define AUTOTEST_CASE(call, ...) \
  { \
    std::string __description__(__VA_ARGS__); \
    AutoTest::ValidateDescriptionLength(__description__, __LINE__, __FILE__); \
    add_descr_phrase(__description__); \
    AutoTest::Logger& case_logger = \
      add_logger(task_name_ + "." + AutoTest::get_logger_name(__description__)); \
    if (!case_logger.empty()) \
    { \
      AutoTest::LoggerSwitcher guard(case_logger); \
      try \
      { \
        call; \
      } \
      catch(const eh::Exception& ex) \
      { \
        Stream::Error msg; \
        AutoTest::fail_message( \
          msg, \
          String::SubString(ex.what()), \
          __FILE__, \
          __FUNCTION__, \
          __LINE__, \
          __description__.c_str()); \
        AutoTest::Logger::thlog().log(msg.str(), Logging::Logger::ERROR); \
        msg.str().append_to(stat_.error); \
        stat_.error += "\n"; \
        case_logger.clear_loggers(); \
      } \
    } \
    else \
    { \
      AutoTest::Logger::thlog().stream(Logging::Logger::INFO) << \
        "Skip '" << __description__ << "' case"; \
    } \
  };

#endif  // _AUTOTESTS_COMMONS_FAILCONTEXT_HPP
