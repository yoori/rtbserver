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

#include "Utils.hpp"
#include <Generics/DirSelector.hpp>


namespace AdServer
{
  namespace Predictor
  {

    std::string remove_ext(
      const std::string& filename)
    {
      return filename.substr(0, filename.rfind('.'));
    }

    void
    remove_files (
      const char* path,
      const char* mask,
      Logging::Logger* logger) throw()
    {

      static const char* FUN = "remove_files";
      
      try
      {
        
        FileList file_list;
        Generics::DirSelect::directory_selector(
          path,
          Generics::DirSelect::list_creator(std::back_inserter(file_list)),
          mask,
          Generics::DirSelect::DSF_DONT_RESOLVE_LINKS |
          Generics::DirSelect::DSF_EXCEPTION_ON_OPEN);

        for (FileList::const_iterator it = file_list.begin();
             it != file_list.end(); ++it)
        {
          struct stat fs;
          
          if(::stat(it->c_str(), &fs) < 0)
          {
            Stream::Error ostr;
            ostr << FUN << ": Failed stat: '" << *it << "'";
            logger->log(ostr.str(),
              Logging::Logger::CRITICAL,
              ASPECT);
          }
          else
          {
            Generics::Time mod_time(fs.st_mtime);
            Generics::Time now = Generics::Time::get_time_of_day();
            
            if(logger->log_level() >= Logging::Logger::TRACE)
            {
              Stream::Error ostr;
              ostr << FUN << ": Remove file: '" << *it << "'";
              logger->log(ostr.str(),
                Logging::Logger::TRACE,
                ASPECT);
            }
            unlink(it->c_str());
          }
        }
      }
      catch (const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": Unecpected exception: " << ex.what();
        logger->log(ostr.str(),
          Logging::Logger::ERROR,
          ASPECT);
      }
    }

    void
    remove_obsolete_files(
      const char* path,
      const char* mask,
      Generics::Time keep_interval,
      Logging::Logger* logger) throw()
    {
      static const char* FUN = "remove_obsolete_files";
      
      try
      {
        FileList file_list;
        Generics::DirSelect::directory_selector(
          path,
          Generics::DirSelect::list_creator(std::back_inserter(file_list)),
          mask,
          Generics::DirSelect::DSF_DONT_RESOLVE_LINKS |
          Generics::DirSelect::DSF_EXCEPTION_ON_OPEN);
        
        for (FileList::const_iterator it = file_list.begin();
             it != file_list.end(); ++it)
        {
          struct stat fs;
          
          if(::stat(it->c_str(), &fs) < 0)
          {
            Stream::Error ostr;
            ostr << FUN << ": Failed stat: '" << *it << "'";
            logger->log(ostr.str(),
              Logging::Logger::CRITICAL,
              ASPECT);
          }
          else
          {
            Generics::Time mod_time(fs.st_mtime);
            Generics::Time now = Generics::Time::get_time_of_day();
            
            if (now - mod_time >= keep_interval)
            {
              Stream::Error ostr;
              ostr << FUN << ": Remove file '" << *it << "'" <<
                ", " << "last modification: '" <<
                mod_time.get_gm_time().format("%Y-%m-%d %H:%M:%S") <<
                "' more than " <<
                keep_interval.tv_sec / Generics::Time::ONE_DAY.tv_sec <<
                " days";
              
              logger->log(ostr.str(),
                Logging::Logger::INFO,
                ASPECT);
              
              unlink(it->c_str());
            }
          }
        }
      }
      catch (const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": Unecpected exception: " << ex.what();
        logger->log(ostr.str(),
          Logging::Logger::ERROR,
          ASPECT);

      }
    }

  }
}
    
