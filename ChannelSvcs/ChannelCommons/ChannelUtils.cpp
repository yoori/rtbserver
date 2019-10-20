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

#include<string>
#include<eh/Exception.hpp>
#include<String/UTF8Case.hpp>
#include<Commons/Constants.hpp>
#include"CommonTypes.hpp" 
#include"ChannelUtils.hpp" 
#include"Serialization.hpp" 


namespace AdServer
{
  namespace ChannelSvcs
  {
    void MergeAtom::print(std::ostream& ostr) const throw(eh::Exception)
    {
      ostr << "{id = " << id << "; lang = " << lang << ";";
      for (SoftTriggerList::const_iterator its(soft_words.begin());
        its != soft_words.end(); ++its)
      {
        std::string trigger;
        ostr << " words = ";
        ostr << "( " << its->channel_trigger_id;
        ostr << " '" << Serialization::get_trigger(its->trigger, trigger)
          << "' => ";
        ostr << ");";
      }
      ostr << ";}";
    }
    
    void SoftTriggerWord::swap(SoftTriggerWord& word) throw()
    {
      std::swap(channel_trigger_id, word.channel_trigger_id);
      trigger.swap(word.trigger);
    }

    void build_combination(
      const std::vector<size_t>& positions,
      AdServer::ChannelSvcs::MatchWords& match_words,
      size_t max_seq_length)
      throw()
    {
      auto it = positions.begin();
      auto pos = *it;
      for(++it; it != positions.end(); ++it)
      {
        size_t seq_length = 0;
        for(auto it2 = it;
            it2 != positions.end() && seq_length < max_seq_length;
            ++it2, ++seq_length)
        {
          match_words.insert(
            String::SubString(match_words.data_holder_.data() + pos, *it2 - pos - 1));
        }
        pos = *it;
      }
    }
  }
}

