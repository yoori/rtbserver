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

#ifndef COMPOSITE_ADV_ACTION_PROCESSOR_HPP
#define COMPOSITE_ADV_ACTION_PROCESSOR_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include "RequestActionProcessor.hpp"

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    /**
     * CompositeAdvActionProcessor
     * delegate request processing to child processors
     */
    class CompositeAdvActionProcessor:
      public virtual AdvActionProcessor,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, AdvActionProcessor::Exception);

      void add_child_processor(AdvActionProcessor* child_processor)
        throw(Exception);

      /** AdvActionProcessor interface */
      virtual void process_adv_action(
        const AdvActionInfo& adv_action_info)
        throw(AdvActionProcessor::Exception);

      virtual void process_custom_action(
        const AdvExActionInfo& adv_custom_action_info)
        throw(AdvActionProcessor::Exception);

    protected:
      virtual ~CompositeAdvActionProcessor() throw() {}

    private:
      typedef std::list<AdvActionProcessor_var> AdvActionProcessorList;
      AdvActionProcessorList child_processors_;
    };

    typedef
      ReferenceCounting::SmartPtr<CompositeAdvActionProcessor>
      CompositeAdvActionProcessor_var;
  }
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  /* CompositeAdvActionProcessor */
  inline
  void
  CompositeAdvActionProcessor::add_child_processor(
    AdvActionProcessor* child_processor) throw(Exception)
  {
    AdvActionProcessor_var add_processor(
      ReferenceCounting::add_ref(child_processor));
    child_processors_.push_back(add_processor);
  }

  inline
  void CompositeAdvActionProcessor::process_adv_action(
    const AdvActionInfo& adv_action_info)
    throw(AdvActionProcessor::Exception)
  {
    for(AdvActionProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->process_adv_action(adv_action_info);
    }
  }

  inline
  void CompositeAdvActionProcessor::process_custom_action(
    const AdvExActionInfo& adv_custom_action_info)
    throw(AdvActionProcessor::Exception)
  {
    for(AdvActionProcessorList::iterator it = child_processors_.begin();
        it != child_processors_.end();
        ++it)
    {
      (*it)->process_custom_action(adv_custom_action_info);
    }
  }
}
}

#endif /*COMPOSITE_ADV_ACTION_PROCESSOR_HPP*/
