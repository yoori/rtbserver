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


#include "ClickResponseChecker.hpp"
#include <Stream/MemoryStream.hpp>
#include <String/StringManip.hpp>
#include <String/AsciiStringManip.hpp>

namespace AutoTest
{
  namespace
  {
    // Utils
    void get_token(Stream::Parser& stream, std::string& token)
    {
      if (!stream.eof())
      {
        char t[4096];
        stream.getline(t, 4096);
        token.assign(t);
      }
    }
  }

  // ClickResponseChecker class
  const String::RegEx
  ClickResponseChecker::EMPTY_PUB_PRECLICK(String::SubString("^PUBPRECLICK=$"));
  
  const String::RegEx
  ClickResponseChecker::EMPTY_CRCLICK(String::SubString("^CRCLICK=$"));

  ClickResponseChecker::ClickResponseChecker(
    AdClient& client,
    const std::string& exp_pub_preclick,
    const std::string& exp_crclick) :
    client_(client)
  {
    {
      std::string regex;
      String::StringManip::mark(("PUBPRECLICK="+exp_pub_preclick).c_str(), regex,
        String::AsciiStringManip::REGEX_META, '\\');
      exp_pub_preclick_regex_.set_expression("^" + regex + "$");
    }
    {
      std::string regex;
      String::StringManip::mark(("CRCLICK="+exp_crclick).c_str(), regex,
        String::AsciiStringManip::REGEX_META, '\\');
      exp_crclick_regex_.set_expression("^" + regex + "$");
    }
  }

  ClickResponseChecker::ClickResponseChecker(
    AdClient& client,
    const String::RegEx& exp_pub_preclick_regex,
    const String::RegEx& exp_crclick_regex) :
    client_(client),
    exp_pub_preclick_regex_(exp_pub_preclick_regex),
    exp_crclick_regex_(exp_crclick_regex)
  {}

  ClickResponseChecker::~ClickResponseChecker() throw()
  {}

  bool
  ClickResponseChecker::check(bool throw_error)
    throw(eh::Exception)
  {
    std::ostringstream expected_body;
    Stream::Parser body(client_.req_response_data());

    std::string got_pub_preclick, got_crclick;
    get_token(body, got_pub_preclick);
    get_token(body, got_crclick);
    
  
    if (client_.req_status() == 200 &&
      exp_pub_preclick_regex_.match(got_pub_preclick) &&
      exp_crclick_regex_.match(got_crclick))
    {
      return true;
    }

    if (throw_error)
    {
      Stream::Error error;
      error << "Invalid click response: { status="<< client_.req_status() <<
        ", '" << got_pub_preclick  << "', '" << got_crclick <<
        "' } is unexpected for { status=200, '" <<
        exp_pub_preclick_regex_.expression().data() << "', '" <<
        exp_crclick_regex_.expression().data() <<  "' }";
      throw AutoTest::CheckFailed(error);
    }

    return false;
  }
}
