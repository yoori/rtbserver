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

#ifndef UTILS_USERIDUTIL_HPP
#define UTILS_USERIDUTIL_HPP

#include <iostream>
#include <eh/Exception.hpp>
#include <Generics/Singleton.hpp>
#include <String/SubString.hpp>

class Application_
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

  Application_() throw();

  virtual ~Application_() throw();

  void main(int& argc, char** argv) throw(eh::Exception);

protected:
  void
  uid_to_sspuid_(
    std::istream& in,
    std::ostream& out,
    const String::SubString& source_id,
    const String::SubString& global_key,
    const String::SubString& sep,
    unsigned long column)
    throw(InvalidArgument, eh::Exception);

  void
  sspuid_to_uid_(
    std::istream& in,
    std::ostream& out,
    const String::SubString& source_id,
    const String::SubString& global_key,
    const String::SubString& sep,
    unsigned long column)
    throw(InvalidArgument, eh::Exception);

  std::string
  uid_to_sspuid_(
    const AdServer::Commons::UserId& user_id,
    const AdServer::Commons::AesEncryptKey& encrypt_key)
    throw();

  void
  sign_uid_(
    std::istream& in,
    std::ostream& out,
    const String::SubString& private_key_file,
    const String::SubString& sep,
    unsigned long column)
    throw(InvalidArgument, eh::Exception);
};

typedef Generics::Singleton<Application_> Application;

#endif /*UTILS_USERIDUTIL_HPP*/
