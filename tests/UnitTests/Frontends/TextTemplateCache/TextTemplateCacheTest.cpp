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

#include <iostream>
#include <string>
#include <Commons/TextTemplateCache.hpp>

using namespace AdServer;

unsigned long ONE_MB = 1024 * 1024;

int main(int /*argc*/, char** /*argv*/)
{
  ::system("mkdir ~tmp 2>/dev/null ; echo '%%TEST%%' >~tmp/t");

  typedef std::map<String::SubString, std::string> ArgMap;
  ArgMap args_cont;
  args_cont[String::SubString("TEST")] = "XX";
  String::TextTemplate::ArgsContainer<ArgMap> args(&args_cont);

  {
    Commons::TextTemplateCache_var cache(new Commons::TextTemplateCache(
      ONE_MB,
      Generics::Time::ONE_MINUTE,
      Commons::TextTemplateCacheConfiguration<Commons::TextTemplate>(
        Generics::Time::ONE_SECOND)));

    Generics::Timer timer;
    timer.start();

    for(int i = 0; i < 10000; ++i)
    {
      Commons::TextTemplate_var t = cache->get("~tmp/t");
      std::string res = t->instantiate(args);
      /*
      std::cout << res << std::endl;
      */
    }

    timer.stop();
    std::cout << timer.elapsed_time() << std::endl;
  }

  {
    Commons::TextTemplateCache_var cache(new Commons::TextTemplateCache(
      ONE_MB,
      Generics::Time::ONE_MINUTE,
      Commons::TextTemplateCacheConfiguration<Commons::TextTemplate>(
        Generics::Time::ZERO)));

    Generics::Timer timer;
    timer.start();

    for(int i = 0; i < 10000; ++i)
    {
      Commons::TextTemplate_var t = cache->get("~tmp/t");
      std::string res = t->instantiate(args);
      /*
      std::cout << res << std::endl;
      */
    }

    timer.stop();
    std::cout << timer.elapsed_time() << std::endl;
  }

  return 0;
}
