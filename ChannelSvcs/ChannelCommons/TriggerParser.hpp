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


#ifndef CHANNEL_SERVICE_CHANNEL_PARSER_HPP_
#define CHANNEL_SERVICE_CHANNEL_PARSER_HPP_

#include <set>
#include <Logger/Logger.hpp>
#include <ChannelSvcs/ChannelCommons/CommonTypes.hpp>
#include <ChannelSvcs/ChannelCommons/Serialization.hpp>
#include <Language/SegmentorCommons/SegmentorInterface.hpp>


namespace AdServer
{
namespace ChannelSvcs
{

  namespace TriggerParser
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    class MergeContainer
    {
    public:
      virtual ~MergeContainer() throw(){};

      virtual size_t add_trigger(MergeAtom& atom)
        throw(TriggerParser::Exception) = 0;

      virtual bool ready() const throw() { return true; }
    };

    class TriggerParser
    {
    public:

      static void parse_word(
        IdType channel_id,
        unsigned int channel_trigger_id,
        char type,
        const std::string& trigger,
        bool negative,
        const Language::Segmentor::SegmentorInterface* segmentor,
        SoftTriggerList& soft_words,
        size_t seq_length,
        Logging::Logger* logger)
        throw(Exception);

      static void parse_url(
        IdType channel_id,
        unsigned int channel_trigger_id,
        const std::string& trigger,
        bool negative,
        const std::set<unsigned short>& allow_ports,
        SoftTriggerList& url_words,
        Logging::Logger* logger)
        throw(Exception);

      static void parse_uids(
        IdType channel_id,
        const std::string& trigger,
        std::string& uids_trigger,
        Logging::Logger* logger)
        throw(Exception);

      static size_t parse_triggers(
        unsigned int id,
        const std::string& lang,
        const TriggerList& triggers,
        const Language::Segmentor::SegmentorInterface* segmentor,
        const std::set<unsigned short>& allow_ports,
        MergeContainer* out,
        size_t seq_length,
        Logging::Logger* logger = 0)
        throw(Exception);
        
      static const unsigned long MAX_TRIGGER_LENGTH;

      static const unsigned long MAX_URL_LENGTH;
      static const unsigned long WORSE_MULT;

    };

    class ParseTracer
    {
    public:
      ParseTracer(bool active = true) throw();

      void start() throw();
      void get_time(Generics::Time& time, Generics::Time& cpu_time) throw();
      void add_time(Generics::Time& time, Generics::Time& cpu_time) throw();

    private:
      Generics::Timer timer_;
      Generics::CPUTimer cpu_timer_;
      bool active_;
    };

  }
}// namespace ChannelSvcs
}

#endif //CHANNEL_SERVICE_CHANNEL_PARSER_HPP_

