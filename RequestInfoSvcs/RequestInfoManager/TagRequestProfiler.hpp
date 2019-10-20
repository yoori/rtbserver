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

#ifndef TAGREQUESTPROFILER_HPP
#define TAGREQUESTPROFILER_HPP

#include <list>
#include <ReferenceCounting/DefaultImpl.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Sync/Condition.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <HTTP/UrlAddress.hpp>

#include "TagRequestProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  struct RequestPool;

  /**
   * TagRequestProfiler
   */
  class TagRequestProfiler:
    public TagRequestProcessor,
    public Generics::CompositeActiveObject,
    public ReferenceCounting::AtomicImpl
  {
  public:
    typedef std::list<std::string> AddressList;

    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    TagRequestProfiler(
      Logging::Logger* logger,
      Generics::ActiveObjectCallback* callback,
      unsigned long thread_count,
      const Generics::Time& window_size,
      unsigned long max_request_pool_size,
      const char* uid_private_key,
      const AddressList& addresses,
      const Generics::Time& repeat_trigger_timeout)
      throw(Exception);

    virtual void
    process_tag_request(const TagRequestInfo&)
      throw(TagRequestProcessor::Exception);

  protected:
    typedef ReferenceCounting::SmartPtr<RequestPool>
      RequestPool_var;

  protected:
    virtual ~TagRequestProfiler() throw();

  private:
    Logging::Logger_var logger_;
    const unsigned long max_request_pool_size_;
    const Generics::Time repeat_trigger_timeout_;
    RequestPool_var request_pool_;
  };

  typedef ReferenceCounting::SmartPtr<TagRequestProfiler>
    TagRequestProfiler_var;
}
}

#endif /*TAGREQUESTPROFILER_HPP*/
