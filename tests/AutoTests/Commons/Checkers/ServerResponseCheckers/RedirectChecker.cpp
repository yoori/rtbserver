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


#include "RedirectChecker.hpp"
#include <String/StringManip.hpp>
#include <Stream/MemoryStream.hpp>

namespace AutoTest
{
  // RedirectChecker class
  RedirectChecker::RedirectChecker(
    AdClient& client,
    const std::string& location) :
    client_(client)
  {
    std::string regex;
    String::StringManip::mark(location.c_str(), regex,
      String::AsciiStringManip::REGEX_META, '\\');
    regex_.set_expression("^" + regex + "$");
  }

  RedirectChecker::RedirectChecker(
    AdClient& client,
    const String::RegEx& regex) :
    client_(client),
    regex_(regex)
  {}

  RedirectChecker::~RedirectChecker() throw()
  {}

  bool
  RedirectChecker::check(bool throw_error)
    throw(eh::Exception)
  {
    if (
      client_.find_header_value("Location", location_) &&
      client_.req_status() == 302 &&
      regex_.match(location_))
    {
      return true;
    }
    if (throw_error)
    {
      Stream::Error error;
      error << "Invalid redirect, got: status=" << client_.req_status() <<
        ", location='" << location_ << "' (expected: status=302, location regex='" <<
        regex_.expression().data() << "')";
      throw AutoTest::CheckFailed(error);
    }
    return false;
  }
}
