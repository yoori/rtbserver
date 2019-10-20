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

#ifndef COMPOSITE_TAG_REQUEST_PROCESSOR_HPP
#define COMPOSITE_TAG_REQUEST_PROCESSOR_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include "TagRequestProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  /**
   * CompositeTagRequestProcessor
   * delegate request processing to child processors
   */
  class CompositeTagRequestProcessor:
    public virtual TagRequestProcessor,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, TagRequestProcessor::Exception);

    void add_child_processor(TagRequestProcessor* child_processor)
      throw(Exception);

    void process_tag_request(const TagRequestInfo& tag_request_info)
      throw(TagRequestProcessor::Exception);

  protected:
    virtual ~CompositeTagRequestProcessor() throw() {}

  private:
    typedef std::list<TagRequestProcessor_var> TagRequestProcessorList;
    TagRequestProcessorList child_processors_;
  };

  typedef
    ReferenceCounting::SmartPtr<CompositeTagRequestProcessor>
    CompositeTagRequestProcessor_var;
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  /* CompositeTagRequestProcessor */
  inline
  void
  CompositeTagRequestProcessor::add_child_processor(
    TagRequestProcessor* child_processor) throw(Exception)
  {
    TagRequestProcessor_var add_processor(
      ReferenceCounting::add_ref(child_processor));
    child_processors_.push_back(add_processor);
  }

  inline
  void
  CompositeTagRequestProcessor::process_tag_request(
    const TagRequestInfo& tag_request_info)
    throw(TagRequestProcessor::Exception)
  {
    for(TagRequestProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->process_tag_request(tag_request_info);
    }
  }
}
}

#endif /*COMPOSITE_TAG_REQUEST_PROCESSOR_HPP*/
