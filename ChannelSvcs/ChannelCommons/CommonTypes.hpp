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


#ifndef AD_SERVER_COMMON_TYPES_HPP
#define AD_SERVER_COMMON_TYPES_HPP

#include<vector>
#include<string>
#include<list>
#include<map>
#include<set>
#include<eh/Exception.hpp>
#include<String/SubString.hpp>
#include<Generics/Time.hpp>
#include<Generics/Uuid.hpp>
#include<Generics/GnuHashTable.hpp>
#include<Language/BLogic/NormalizeTrigger.hpp>
#include<ChannelSvcs/ChannelCommons/Lexeme.hpp>

namespace AdServer
{
  namespace ChannelSvcs
  {

    typedef unsigned int IdType;
    typedef std::vector<IdType> IdVector;
    typedef std::list<IdType> IdList;
    typedef std::set<IdType> IdSet;
    typedef std::vector<std::string> StringVector;
    typedef std::vector<String::SubString> SubStringVector;

    typedef std::string HardWord;
    typedef std::vector<HardWord> HardWords;
    typedef SubStringVector SoftWord;
    typedef std::vector<Lexeme_var> LexemesPtrVector;
    typedef Language::Trigger::Trigger::Part Part;
    typedef Language::Trigger::Trigger::Parts Parts;

    typedef std::set<std::string> StringSet;
    typedef std::set<String::SubString> SubStringSet;
    typedef std::list<std::string> StringList;
    typedef std::list<String::SubString> SubStringList;

    struct Trigger
    {
      IdType channel_trigger_id;
      std::string trigger;
      char type;
      bool negative;
    };

    struct SoftTriggerWord
    {
      void swap(SoftTriggerWord& word) throw();

      IdType channel_trigger_id;
      std::string trigger;
    };

    typedef std::list<SoftTriggerWord> SoftTriggerList;

    typedef Trigger UrlWord;

    typedef std::list<Trigger> TriggerList;
    typedef std::vector<Trigger> TriggerVector;
    typedef std::vector<Generics::Uuid> UuidVector;

    typedef TriggerList UrlWords;

    struct MergeAtom
    {
      const MergeAtom& operator=(IdType in) throw();
      bool operator==(IdType in) const throw();
      IdType operator()() const throw();
      void print(std::ostream& ostr) const throw(eh::Exception);
      IdType id; // id
      std::string lang;
      SoftTriggerList soft_words; // words should be merged as soft
      std::string uids_trigger;
    };

    inline
    const MergeAtom& MergeAtom::operator=(IdType in) throw()
    {
      id = in;
      return *this;
    }

    inline
    bool MergeAtom::operator==(IdType in) const throw()
    {
      return (id == in);
    }

    inline
    IdType MergeAtom::operator()() const throw()
    {
      return id;
    }

    struct MatchWords:
      public Generics::GnuHashSet<Generics::SubStringHashAdapter>
    {
      std::string data_holder_;//uses for memory allocation of SubStrings
    };

  }
}

#endif //AD_SERVER_COMMON_TYPES_HPP

