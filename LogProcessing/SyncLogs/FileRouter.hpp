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

#ifndef SYNCLOGS_FILEROUTER_HPP
#define SYNCLOGS_FILEROUTER_HPP

#include <string>
#include <eh/Exception.hpp>
#include <String/TextTemplate.hpp>

#include "Utils.hpp"

namespace AdServer
{
namespace LogProcessing
{
  namespace TemplateParams
  {
    const String::SubString MARKER("##", 2);
    const char HOST[]      = "HOST";
    const char PATH[]      = "PATH";
    const char PATTERN[]   = "PATTERN";
    const char FILE_NAME[] = "FILE_NAME";
    const char SRC_HOST[]  = "SRC_HOST";
    const char SRC_PATH[]  = "SRC_PATH";
    const char SRC_DIR[]   = "SRC_DIR";
    const char DST_HOST[]  = "DST_HOST";
    const char DST_PATH[]  = "DST_PATH";
    const char DST_DIR[]  = "DST_DIR";
    const char HASH[]  = "HASH";
  }

  struct FileRouteParams
  {
    std::string src_file_name; // to keep between calls
    std::string src_file_path;
    std::string src_host;
    std::string src_dir;

    std::string dst_file_name; // == " ==
    std::string dst_host;

    unsigned int try_number;
  };

  class FileRouter: public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    // if sync_mode == true that means the router must perform a
    // directory sync operation
    virtual void
    move(
      const FileRouteParams& file_route_params,
      bool sync_mode = false,
      InterruptCallback* interrupter = 0)
      throw(Exception) = 0;

  protected:
    virtual ~FileRouter() throw() {};
  };

  typedef ReferenceCounting::QualPtr<FileRouter> FileRouter_var;
  typedef ReferenceCounting::FixedPtr<FileRouter> FixedFileRouter_var;

  class AppFileRouter:
    public FileRouter,
    public ReferenceCounting::AtomicImpl
  {
  public:
    AppFileRouter(
      const char* command_template,
      const char* post_command_template = "")
      throw(Exception);

    virtual void
    move(
      const FileRouteParams& file_route_params,
      bool sync_mode = false,
      InterruptCallback* interrupter = 0)
      throw(Exception);

  protected:
    virtual
    ~AppFileRouter() throw() {}

    void
    init_command_(
      const String::TextTemplate::IStream& command_template,
      const FileRouteParams& file_route_params,
      std::string& command,
      bool sync_mode)
      const
      throw(Exception);

    int
    run_command_(
      const String::TextTemplate::IStream& command_template,
      const FileRouteParams& file_route_params,
      bool sync_mode,
      std::string& command,
      std::string& output,
      InterruptCallback* interrupter = 0)
      const
      throw(Exception);

    void
    check_and_set_command_template_(
      const char* str_command_template,
      String::TextTemplate::IStream& command_template)
      throw(Exception);

  protected:
    String::TextTemplate::IStream command_template_;
    String::TextTemplate::IStream post_command_template_;
  };
}
}


#endif /*SYNCLOGS_FILEROUTER_HPP*/
