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

#ifndef UTILS_CTRGENERATOR_APPLICATION_HPP_
#define UTILS_CTRGENERATOR_APPLICATION_HPP_

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

#include <Utils/Predictor/DTree/SVM.hpp>
#include <Utils/Predictor/DTree/Label.hpp>

using namespace Vanga;

class Application_
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

public:
  Application_() throw();

  virtual
  ~Application_() throw();

  void
  main(int& argc, char** argv) throw(eh::Exception);

protected:
  void
  generate_svm_(
    std::ostream& out,
    std::istream& in,
    const char* column_names,
    const char* result_dictionary);
};

typedef Generics::Singleton<Application_> Application;

#endif /*UTILS_CTRGENERATOR_APPLICATION_HPP_*/
