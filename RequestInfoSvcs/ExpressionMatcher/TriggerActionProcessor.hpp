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

#ifndef EXPRESSIONMATCHER_TRIGGERACTIONPROCESSOR_HPP
#define EXPRESSIONMATCHER_TRIGGERACTIONPROCESSOR_HPP

#include <ostream>
#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>

#include "InventoryActionProcessor.hpp"

namespace AdServer
{
namespace RequestInfoSvcs
{
  class TriggerActionProcessor:
    public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    // channel_trigger_id,channel_id => divided imps
    struct MatchCounter: public RevenueDecimal
    {
      MatchCounter(): RevenueDecimal(RevenueDecimal::ZERO)
      {}
    };

    struct MatchCounterKey
    {
      unsigned long channel_trigger_id;
      unsigned long channel_id;

      MatchCounterKey() throw ()
        : channel_trigger_id(0), channel_id(0)
      {}

      MatchCounterKey(
        unsigned long channel_trigger_id_val,
        unsigned long channel_id_val)
        throw ()
        : channel_trigger_id(channel_trigger_id_val), channel_id(channel_id_val)
      {}

      bool
      operator< (const MatchCounterKey& arg) const
      {
        if (channel_trigger_id != arg.channel_trigger_id)
        {
          return (channel_trigger_id < arg.channel_trigger_id);
        }

        return (channel_id < arg.channel_id);
      }
    };

    struct MatchCountMap: public std::map<MatchCounterKey, MatchCounter>
    {
      void
      add(const MatchCountMap& right);

      void
      negate();

      void
      print(
        std::ostream& out,
        const char* offset = "") const;
    };

    struct TriggersMatchInfo
    {
      Generics::Time time;

      MatchCountMap page_matches;
      MatchCountMap search_matches;
      MatchCountMap url_matches;
      MatchCountMap url_keyword_matches;

      void
      print(
        std::ostream& out,
        const char* offset = "") const;
    };

  public:
    virtual void process_triggers_impression(
      const TriggersMatchInfo& match_info)
      throw(Exception) = 0;

    virtual void process_triggers_click(
      const TriggersMatchInfo& match_info)
      throw(Exception) = 0;

  protected:
    virtual
    ~TriggerActionProcessor() throw () = default;
  };

  typedef ReferenceCounting::SmartPtr<TriggerActionProcessor>
    TriggerActionProcessor_var;
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  inline
  void
  TriggerActionProcessor::
  MatchCountMap::add(const MatchCountMap& right)
  {
    for(const_iterator it = right.begin(); it != right.end(); ++it)
    {
      (*this)[it->first] += it->second;
    }
  }

  inline
  void
  TriggerActionProcessor::
  MatchCountMap::negate()
  {
    for(iterator it = begin(); it != end(); ++it)
    {
      it->second.negate();
    }
  }

  inline
  void
  TriggerActionProcessor::
  MatchCountMap::print(std::ostream& out, const char* offset) const
  {
    for(const_iterator it = begin(); it != end(); ++it)
    {
      out << offset << '(' << it->first.channel_trigger_id << ':' << it->first.channel_id <<
        "): " << it->second << std::endl;
    }
  }

  inline
  void
  TriggerActionProcessor::
  TriggersMatchInfo::print(
    std::ostream& out, const char* offset) const
  {
    out << offset << "time: " << time.get_gm_time() << std::endl <<
      offset << "page_matches: " << std::endl;
    page_matches.print(out, (std::string(offset) + "  ").c_str());
    out << offset << "search_matches: " << std::endl;
    search_matches.print(out, (std::string(offset) + "  ").c_str());
    out << offset << "url_matches: " << std::endl;
    url_matches.print(out, (std::string(offset) + "  ").c_str());
    out << offset << "url_keyword_matches: " << std::endl;
    url_keyword_matches.print(out, (std::string(offset) + "  ").c_str());
  }
}
}

#endif /*EXPRESSIONMATCHER_TRIGGERACTIONPROCESSOR_HPP*/
