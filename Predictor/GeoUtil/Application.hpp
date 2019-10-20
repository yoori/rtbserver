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

#ifndef PREDICTOR_GEOUTIL_APPLICATION_HPP_
#define PREDICTOR_GEOUTIL_APPLICATION_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <deque>

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Generics/Singleton.hpp>
#include <Generics/Time.hpp>
#include <Commons/Containers.hpp>

class Application_
{
public:
  Application_() throw();

  virtual
  ~Application_() throw();

  void
  main(int& argc, char** argv) throw(eh::Exception);

protected:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

protected:
  void
  add_geo_(
    std::istream& in,
    std::ostream& out);
};

typedef Generics::Singleton<Application_> Application;

#endif /*PREDICTOR_GEOUTIL_APPLICATION_HPP_*/
