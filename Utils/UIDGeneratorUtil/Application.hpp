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

#ifndef UTILS_UIDGENERATORUTIL_APPLICATION_HPP_
#define UTILS_UIDGENERATORUTIL_APPLICATION_HPP_

#include <arpa/inet.h>
#include <map>
#include <set>
#include <string>

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Generics/Singleton.hpp>
#include <Generics/Time.hpp>

class Application_
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  Application_() throw();

  virtual ~Application_() throw();

  void main(int& argc, char** argv) throw(eh::Exception);

protected:
  void
  generate_request_(
    std::ostream& out,
    const String::SubString& ids_str,
    const String::SubString& buckets_str,
    const String::SubString& obuckets_str)
    throw();

  void
  send_(
    const String::SubString& socket_str,
    const Generics::Time& timeout,
    unsigned long max_portion)
    throw();

  void
  generate_request_buf_(
    std::vector<unsigned char>& buf,
    const String::SubString& ids_str,
    const String::SubString& buckets_str,
    const String::SubString& obuckets_str)
    throw();
};

typedef Generics::Singleton<Application_> Application;

#endif /*UTILS_UIDGENERATORUTIL_APPLICATION_HPP_*/
