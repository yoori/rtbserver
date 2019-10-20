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

#pragma once

#include <vector>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>
#include <BiddingFrontend/BiddingFrontendStat.hpp>
#include <Logger/Logger.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Frontends/CommonModule/CommonModule.hpp>

namespace AdServer
{
  namespace Frontends
  {
    /**
     * @class FrontendsPool
     *
     * @brief HTTP frontends pool.
     */
    class FrontendsPool :
      public virtual FrontendCommons::FrontendInterface,
      public virtual ReferenceCounting::AtomicImpl  
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      enum ModuleId
      {
        M_BIDDING,
        M_PUBPIXEL,
        M_CONTENT,
        M_DIRECTORY,
        M_WEBSTAT,
        M_ACTION,
        M_USERBIND,
        M_PASSBACK,
        M_PASSBACKPIXEL,
        M_OPTOUT,
        M_NULLAD,
        M_ADINST,
        M_CLICK,
        M_IMPRTRACK,
        M_AD
      };

      typedef std::vector<ModuleId> ModuleIdArray;

    public:
      /**
       * @brief Constructor.
       * @param config path
       */
      FrontendsPool(
        const char* config_path,
        const ModuleIdArray& modules,
        Logging::Logger* logger,
        StatHolder* stats);

      /**
       * @brief Handle or not URI.
       * @param uri.
       */
      virtual bool
      will_handle(const String::SubString& uri) throw ();

      /**
       * @brief Handle HTTP request.
       * @param HTTP request
       * @param[out] HTTP response
       */
      virtual int
      handle_request(
        const FCGI::HttpRequest& request,
        FCGI::HttpResponse& response) throw();

      /**
       * @brief Handle HTTP request without params.
       * @param HTTP request
       * @param[out] HTTP response
       */
      virtual int
      handle_request_noparams(
        FCGI::HttpRequest& request,
        FCGI::HttpResponse& response)
        throw(eh::Exception);

      /**
       * @brief Initialize frontend.
       */
      virtual void
      init() throw(eh::Exception);

      /**
       * @brief Shutdown frontend.
       */
      virtual void
      shutdown() throw();

    protected:
      virtual
      ~FrontendsPool() throw() = default;

    private:
      /**
       * @brief Init a frontend.
       * @param frontend config
       * @param frontend params
       */
      template <class Frontend, typename Config, typename ...T>
      void
      init_frontend(
        const Config& cfg,
        T&&... params);

    private:
      Configuration_var config_;
      ModuleIdArray modules_;
      Logging::Logger_var logger_;
      StatHolder_var stats_;
      CommonModule_var common_module_;
      std::vector<FrontendCommons::Frontend_var> frontends_;
    };
  }
}
