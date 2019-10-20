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

#ifndef VIRTUALSERVERADAPTER_HPP
#define VIRTUALSERVERADAPTER_HPP

#include <Apache/Adapters.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Commons/Algs.hpp>

namespace Apache
{
  /* apache module wrapper for enable module at VirtualHost level */
  template <typename ModuleType, typename HttpResponse = Apache::HttpResponse>
  class ServerNameFilterAdapter:
    public QuickHandlerAdapter<ModuleType, HttpResponse>
  {
  public:
    ServerNameFilterAdapter(int where = APR_HOOK_LAST) throw();

    void add_module_server(const server_rec* server) throw();

    bool module_used() const throw();

    // filter requests by hosts
    virtual int quick_handler(request_rec* r, int) throw();

    virtual bool
    will_handle(const char* uri) throw () = 0;

  protected:
    virtual ~ServerNameFilterAdapter() throw()
    {}

  private:
    typedef Generics::GnuHashSet<
      Algs::ConstPointerHashAdapter<server_rec> >
      ServerSet;

  private:
    ServerSet hosts_;
  };
}

namespace Apache
{
  template<typename ModuleType, typename HttpResponse>
  ServerNameFilterAdapter<ModuleType, HttpResponse>::
    ServerNameFilterAdapter(int where)
    throw()
    : QuickHandlerAdapter<ModuleType, HttpResponse>(where)
  {}

  template<typename ModuleType, typename HttpResponse>
  void
  ServerNameFilterAdapter<ModuleType, HttpResponse>::
    add_module_server(const server_rec* server)
    throw()
  {
    hosts_.insert(server);
  }

  template<typename ModuleType, typename HttpResponse>
  bool
  ServerNameFilterAdapter<ModuleType, HttpResponse>::module_used() const
    throw()
  {
    return !hosts_.empty();
  }

  template<typename ModuleType, typename HttpResponse>
  int
  ServerNameFilterAdapter<ModuleType, HttpResponse>::
    quick_handler(request_rec* r, int)
    throw()
  {
    switch (r->method_number)
    {
    case M_GET:
    case M_POST:
    case M_PUT:
      break;

    default:
      return DECLINED;
    }

    if(hosts_.find(r->server) == hosts_.end() ||
       !will_handle(r->uri))
    {
      return DECLINED;
    }

    try
    {
      HttpRequest request(r);
      HttpResponse response(r);

      // handle request
      return this->handle_request_noparams(request, response);
    }
    catch (const HttpRequest::Exception& e)
    {
      ap_log_error(APLOG_MARK, APLOG_WARNING, 0, r->server, e.what());

      return e.error_code();
    }
    catch (const eh::Exception& e)
    {
      ap_log_error(APLOG_MARK, APLOG_WARNING, 0, r->server, e.what());

      return HTTP_INTERNAL_SERVER_ERROR;
    }
  }
}

#endif /*VIRTUALSERVERADAPTER_HPP*/
