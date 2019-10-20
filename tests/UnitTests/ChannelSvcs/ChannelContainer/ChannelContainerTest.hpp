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


#ifndef _AD_SERVER_CHANNEL_CONTAINER_TEST_HPP_
#define _AD_SERVER_CHANNEL_CONTAINER_TEST_HPP_

namespace AdServer
{
  namespace Testing
  {
    #include <eh/Exception.hpp>
    #include <Generics/Time.hpp>
    #include <ChannelSvcs/ChannelCommons/CommonTypes.hpp>

    class ChannelContainerTest
    {
    public:
      DECLARE_EXCEPTION(ErrorDescriptor, eh::DescriptiveException);

      enum TestCases
      {
        TC_NONE = 0x00,
        TC_CONSTRUCTOR = 0x01,
        TC_ADD_TRIGGER = 0x02,
        TC_MERGE = 0x04,
        TC_MATCH = 0x08,
        TC_UPDATE = 0x10,
        TC_UIDS = 0x20,
        TC_ALL_CASES =
          TC_CONSTRUCTOR + TC_ADD_TRIGGER + TC_MERGE + TC_MATCH + TC_UPDATE
      }; 
      struct IdToTrigger
      {
        unsigned int id;
        const char* trigger;
        bool flag;//url or not
        bool match;
      };

      ChannelContainerTest() throw();
      int run(int argc, char* argv[]) throw();

    private:
      struct TestNames
      {
        const char* name;
        TestCases value;
      };
      typedef std::unique_ptr<ChannelSvcs::ChannelContainer> ContPtr;
      typedef std::unique_ptr<ChannelSvcs::UpdateContainer> UpdPtr;

      void parse_argc_(int argc, char* argv[]) throw();
      void usage_() throw();
      void parse_tests_(const char* tests) throw();
      void log_parameters_() throw();
      void
      log_action_(
        const std::string& action,
        unsigned int level = 1,
        const char* function = 0) const
        throw (eh::Exception);

      template<class ResultType>
      ResultType& gen_hard_words_(
        const char* prefix,
        ResultType& result,
        size_t number,
        bool negative, 
        size_t count = 2)
        throw(eh::Exception);

      void gen_soft_words_(
        const char* prefix,
        ChannelSvcs::SoftTriggerList& result,
        size_t number,
        size_t count_words,
        bool negative, 
        size_t count = 2)
        throw(eh::Exception);

      template<class CompType, class CompType2>
      bool compary_words_(
        const CompType& orig,
        const CompType2& res)
        throw();

      void gen_url_(
        const char* prefix,
        ChannelSvcs::SoftTriggerList& result,
        size_t number,
        bool negative = false,
        size_t count = 2)
        throw(eh::Exception);

      void gen_atom_(
        ChannelSvcs::MergeAtom& atom,
        size_t num,
        const char* prefix = 0)
        throw(eh::Exception);
      void check_atom_(
        const char* test_place,
        const ChannelSvcs::ChannelContainerBase::ChannelUpdateData& value,
        const ChannelSvcs::MergeAtom& atom)
        throw(ErrorDescriptor);

      void create_data_for_container_(
        unsigned long start,
        unsigned long last,
        ChannelSvcs::UpdateContainer* cont,
        const char* prefix = 0,
        ChannelSvcs::ChannelContainerBase::ChannelMap* buffer = 0)
        throw(eh::Exception);
      void check_data_of_container_(
        unsigned long start,
        unsigned long last,
        const ChannelSvcs::ChannelContainerBase::ChannelMap& buffer,
        const char* func,
        const char* prefix = 0)
        throw(ErrorDescriptor);

      int check_constructor_() throw();
      int check_add_trigger_() throw();
      int check_merge_() throw();
      int check_match_() throw();
      int check_update_() throw();
      int check_uids_() throw ();

      typedef std::set<Generics::Uuid> Uuids;
      typedef std::map<unsigned long, Uuids> UidsData;
      typedef ChannelSvcs::ChannelChunk::AddUidMap AddUidMap;
      typedef ChannelSvcs::IdSet IdSet;

      void check_uids_prepare_removing_(
        IdSet& channels,
        IdSet& channels2,
        size_t percent)
        throw();

      void check_uids_perf_update_(
        ChannelSvcs::UidMap& res,
        AddUidMap& added,
        const IdSet& removed_uid_channels)
        throw();

      void init_triggers_(
        const IdToTrigger triggers[],
        size_t size_triggers,
        ChannelSvcs::ChannelContainer& base,
        unsigned int offset = 0) const
        throw(eh::Exception);

      AddUidMap& prepare_add_map_(
        const UidsData& uid_data,
        AddUidMap& add_uids,
        const IdSet& channels,
        bool add = true)
        throw ();

      void check_uids_data_(
        const UidsData& data,
        const ChannelSvcs::UidMap& res,
        const IdSet& channels = IdSet())
        throw(ErrorDescriptor);
      //

    private:
      static TestNames test_names_[];
      unsigned long cases_;
      unsigned long count_chunks_;
      unsigned long count_triggers_;
      unsigned long count_channels_;
      unsigned long count_uids_;
      unsigned int verbose_;
      Generics::Time time_;
      Generics::Time cpu_time_;
    };
  }
}

#endif//_AD_SERVER_CHANNEL_CONTAINER_TEST_HPP_

