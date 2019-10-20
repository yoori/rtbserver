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

#ifndef _RSYNCFILEROUTER_HPP_
#define _RSYNCFILEROUTER_HPP_

#include <eh/Exception.hpp>

#include "Utils.hpp"
#include "FileRouter.hpp"

namespace AdServer
{
  namespace LogProcessing
  {
    /**
     * wrapper for execute rsync command &
     * control its result
     */
    class RSyncFileRouter: public AppFileRouter
    {
    public:
      DECLARE_EXCEPTION(AlreadyExists, Exception);

    public:
      RSyncFileRouter(
        const char* command_template,
        const char* post_command_template = "")
        throw(Exception);

      virtual ~RSyncFileRouter() throw() {}

      virtual void
      move(
        const FileRouteParams& file_route_params,
        bool sync_mode = false,
        InterruptCallback* interrupter = 0)
        throw(Exception);

    protected:
      /** check rsync output after execution */
      static void check_rsync_output_(
        const FileRouteParams& file_route_params,
        const char* output_str)
        throw(AlreadyExists, Exception);

      /** Returns error message corresponding rsync exit code */
      static const char* rsync_err_msg_(int exit_code) throw();
    };
  }
}


#endif
