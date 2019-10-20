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


#ifndef AD_SERVER_CHANNEL_CONTAINER
#define AD_SERVER_CHANNEL_CONTAINER

#include <map>
#include <set>
#include <list>
#include <vector>
#include <ostream>

#include <eh/Exception.hpp>
#include <Logger/Logger.hpp>
#include <Sync/SyncPolicy.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <ChannelSvcs/ChannelServer/ChannelChunk.hpp>
#include "ContainerMatchers.hpp"

namespace AdServer
{
namespace ChannelSvcs
{

  const unsigned int c_special_adv = 1;
  const unsigned int c_special_track = 2;

  class ChannelChunk;
  class ChannelContainer;
  class UpdateContainer;

  template <class Map>
  void
  fill_deleted(
    const Map& map,
    const Generics::Time& master,
    std::map<unsigned long, Generics::Time>& deleted)
    throw(eh::Exception);

  template<typename T>
  typename T::first_type get_first(const T& pair) throw() { return pair.first;}

  typedef std::vector<MatchUrl> MatchUrls;

  struct UnmergedKey
  {
    UnmergedKey(unsigned short lang_value, const std::string& p_trigger) throw()
     : lang(lang_value), trigger(p_trigger){};

    unsigned short lang; 
    const std::string& trigger;
  };

  inline
  bool  operator< (const UnmergedKey& left,const UnmergedKey& right)
  {
    if (left.lang == right.lang)
    {
      return left.trigger < right.trigger;
    }
    return left.lang < right.lang;
  }

  class ChannelContainerBase
  {
  public:

    struct ChannelUpdateData: public ReferenceCounting::AtomicImpl
    {
      virtual ~ChannelUpdateData() throw(){};

      struct TriggerItem
      {
        TriggerItem(unsigned int id = 0, SoftMatcher* data = 0) throw()
          : channel_trigger_id(id),
            matcher(data ? ReferenceCounting::add_ref(data) : 0)
        {
        }
        operator unsigned int () const throw() {return channel_trigger_id;} 

        unsigned int channel_trigger_id;
        SoftMatcher_var matcher;
      };

      typedef std::vector<TriggerItem> TriggerItemVector;

      Generics::Time stamp;//time stamp
      Generics::Time db_stamp;//time stamp
      std::string lang;
      std::string country;
      size_t channel_size;
      TriggerItemVector triggers;
    };

    typedef ReferenceCounting::SmartPtr<ChannelUpdateData>
      ChannelUpdateData_var;

    typedef std::map<unsigned int, ChannelUpdateData_var> ChannelMap;

    ChannelContainerBase() throw();

    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(InvalidArgument, Exception);

    ChannelUpdateData_var get_channel(unsigned long channel_id) const
      throw();

    struct UnmergedItem
    {
      UnmergedItem(unsigned int p_channel_id) throw() 
        : channel_id(p_channel_id) {};

      //operator IdType() const { return word.channel_trigger_id; }

      IdType channel_id;
      SoftTriggerWord word;
    };

    struct MatterItem
    {
      IdVector removed;
      SoftTriggerList added;
      std::string uids;
      unsigned short lang;
    };

    typedef std::list<UnmergedItem> UnmergedItems;

    struct UnmergedValue: public UnmergedItems
    {
    };

    typedef std::map<UnmergedKey, UnmergedValue> UnmergedMap;

  protected:

    /*calculate number of chunk in container by hash sum*/
    unsigned long calc_chunk_num_(
      unsigned long hash_sum,
      unsigned long count_chunks) const
      throw();

  protected:
    ChannelMap channel_ids_;
  };

  struct CheckInformation
  {
    struct CheckData
    {
      Generics::Time stamp;
      unsigned long size;
    };
    typedef std::map<unsigned long, CheckData> CheckMap;

    Generics::Time first_master;
    Generics::Time master;
    Generics::Time longest_update;
    bool special_track;
    bool special_adv;
    CheckMap data;
  };

  enum
  {
    PROGRESS_LOAD = 0,
    PROGRESS_MERGE1,
    PROGRESS_PREMERGE,
    PROGRESS_MERGE2,
    PROGRESS_PREPARE_UPDATE,
    PROGRESS_APPLY_UPDATE,
    PROGRESS_REMOVE_NON_STRICT,
    STAGES_COUNT
  };

  class ProgressCounter
  {
  public:
    ProgressCounter(size_t stages, size_t total) throw();

    /*total progress*/
    size_t get_sum_progress() throw ();

    /*proggress of current stage */
    size_t get_progress() throw ();

    /*indicator of total */
    size_t get_total() throw ();

    /*indicator of stage */
    size_t get_stage() throw ();

    /*set proggress of current stage */
    void set_progess(size_t inc) throw();

    void change_stage(size_t stage) throw();

    void set_relative_progress(
      size_t absolute_value,
      size_t div) throw();

    void reset(size_t stages, size_t total) throw();

  protected:
    void change_stage_(size_t stage) throw();

  private:
    typedef Sync::PosixRWLock Mutex_;
    typedef Sync::PosixRGuard ReadGuard_;
    typedef Sync::PosixWGuard WriteGuard_;

    mutable Mutex_ lock_progress_;
    std::vector<size_t> progress_;
    size_t relative_progress_;
    size_t relative_div_;
    size_t absolute_value_;
    size_t stage_;
    size_t count_stages_;
    size_t total_;
  };

  typedef std::unique_ptr<ProgressCounter> ProgressCounterPtr;

  class ChannelContainer: public ChannelContainerBase
  {
  public:

    virtual ~ChannelContainer() throw(){};

    /* argument - count local chunks in container */
    ChannelContainer(unsigned long count_chunks = 1, bool nonstrict = false)
      throw(Exception);

    /* create chunks in container*/
    unsigned long get_count_chunks() const throw();

    typedef std::map<IdType, SoftMatcher_var> IdMatchers;
    typedef std::map<IdType, IdMatchers> ChannelIdToTrigers;
    
    /* add trigger with existing matching in container
     * and uid channels
     * */
    size_t add_stage1(
      UpdateContainer& add,
      UnmergedMap& unmerged,
      ChannelIdToTrigers& added)
      throw(Exception);

    void add_stage2(
      UpdateContainer& add,
      UnmergedMap& unmerged,
      ChannelIdToTrigers& added)
      throw(Exception);

    /*merge 2 containers*/
    void merge(
      UpdateContainer& add,
      const ChannelIdToMatchInfo& info,
      bool update_finished,
      ProgressCounter* progress = 0,
      const Generics::Time& master = Generics::Time::ZERO,
      const Generics::Time& first_load_stamp = Generics::Time::ZERO,
      std::ostream* ostr = 0)
      throw(Exception);

    /*match channels for trigger */
    void match(
      const MatchUrls& url_words,
      const MatchUrls& additional_url_words,
      const MatchWords match_words[CT_MAX],
      const MatchWords& additional_url_keywords,
      const StringVector& exact_words,
      const Generics::Uuid& uid, 
      unsigned int flags,
      TriggerMatchRes& res)
      throw(Exception);

    /*get trigger lists content by id*/
    void fill(ChannelMap& buffer) const
      throw(Exception);

    void get_ccg_update(
      const Generics::Time& old_master,
      unsigned long start,
      unsigned long limit,
      const std::set<unsigned long>& get_id,
      std::vector<CCGKeyword_var>& buffer,
      std::map<unsigned long, Generics::Time>& deleted,
      bool user_only_list) const
      throw(Exception);

    void get_stats(ChannelServerStats& stats) throw();

    void get_master(
      Generics::Time& first_master,
      Generics::Time& master,
      Generics::Time& longest_update) const
      throw();

    void check_update(
      const Generics::Time& master,
      const std::set<unsigned int>& ids,
      CheckInformation& info,
      bool use_only_list) const
      throw();

    void terminate() throw();

    SoftMatcher* get_matcher(
      const std::string& trigger,
      unsigned short lang,
      String::SubString& part) const
      throw(Exception);

    ChannelMatchInfo_var get_active() const throw();

    void check_actual(
      ChannelIdToMatchInfo& info,
      ExcludeContainerType& new_channels_,
      ExcludeContainerType& updated_channels,
      ExcludeContainerType& removed_channels) const
      throw();

    void set_ccg(CCGMap* map) throw();

    CCGMap* get_ccg() const throw();

    ChannelUpdateData_var get_channel(unsigned long channel_id) const
      throw();

  public:

    static void match_parse_refer(
        const String::SubString& value,
        const std::set<unsigned short>& allow_ports,
        bool soft_matching,
        MatchUrls& match_words,
        Logging::Logger* logger)
        throw(eh::Exception);

    static void match_parse_urls(
      const String::SubString& refer,
      const String::SubString& refer_words,
      const std::set<unsigned short>& allow_ports,
      bool soft_matching,
      MatchUrls& url_words,
      MatchWords& url_keywords,
      Logging::Logger* logger,
      const Language::Segmentor::SegmentorInterface* segmentor)
      throw(eh::Exception);

  private:
    typedef MatcherVarsSet NSTriggerAtom;

    void trace_update_data_(
      const UpdateContainer& add,
      std::ostream& debug) 
      throw();

    void add_with_existing_matcher_(
      const ChannelChunkArray& array,
      UnmergedValue& items,
      SoftMatcher* matcher,
      const String::SubString& word) const
      throw();

    void add_uids_(
      const ChannelChunkArray& array,
      const SoftMatcher_var uid_matcher,
      IdType channel_id)
      throw(eh::Exception);

    void clean_failed_merge_() throw();

    template<class VECTOR>
    bool remove_action_(
      const ChannelChunkArray& array,
      unsigned int channel_trigger_id,
      SoftMatcher* matcher,
      const VECTOR& words) const
      throw();

    /*calculate number of chunk in container by trigger*/
    template<class HASHADAPTER>
    unsigned long calc_chunk_num_i_(
      const HASHADAPTER& trigger) const
      throw();

    void add_ns_urls_(
      const String::SubString& url,
      SoftMatcher* matcher)
      throw();

    void remove_ns_urls_(
      const String::SubString& url,
      const SoftMatcher_var& matcher)
      throw();

    void remove_ns_words_(
      const SubStringVector& keys,
      const SoftMatcher_var& matcher)
      throw();

    void add_ns_words_(
      const SubStringVector& keys,
      SoftMatcher* matcher,
      const String::SubString& key_word)
      throw();


    void add_ns_(
      SoftMatcher* matcher,
      const String::SubString& key_word)
      throw();

    /* match uid*/
    void match_uid_(
      const Generics::Uuid& uid, 
      const ChannelChunkArray& array,
      TriggerMatchRes& res)
      throw(Exception);

    /* match urls for triggers*/
    void match_urls_(
      const MatchUrls& urls,
      const ChannelChunkArray& array,
      unsigned int flags,
      TriggerMatchRes& res)
      throw(Exception);

    /* match urls for triggers*/
    void match_ns_urls_(
      const MatchUrls& urls,
      const ChannelChunkArray& array,
      unsigned int flags,
      TriggerMatchRes& res)
      throw(Exception);

    /* match words for triggers*/
    void match_words_(
      const MatchWords& key_words,
      const MatchWords& words,
      const ChannelChunkArray& array,
      const StringVector* exact_words,
      MatchType type,
      unsigned int flags,
      TriggerMatchRes& res,
      MatcherVarsSet* already_matched = 0)
      throw(Exception);

    /* match non strict words for triggers*/
    void match_ns_words_(
      const MatchWords& words,
      const ChannelChunkArray& array,
      MatchType type,
      unsigned int flags,
      TriggerMatchRes& res)
      throw(Exception);

    void remove_non_strict_(
      NSTriggerAtom& urls,
      NSTriggerAtom& page_keywords,
      NSTriggerAtom& url_keywords,
      NSTriggerAtom& search_keywords)
      throw();

    static void check_actual_(
      const ChannelMap& info_old,
      ChannelIdToMatchInfo& info_new,
      ExcludeContainerType &new_ids,
      ExcludeContainerType &up_ids,
      ExcludeContainerType &rm_ids) 
      throw();

    unsigned int choose_word_(
      const ChannelChunkArray& array,
      const SubStringVector& parts,
      const LexemesPtrVector& lexemes) const
      throw();

    ChannelMatchInfo* get_rules_() const
      throw();

  protected:

    typedef Sync::PosixRWLock Mutex_;
    typedef Sync::PosixRGuard ReadGuard_;
    typedef Sync::PosixWGuard WriteGuard_;

    typedef std::map<String::SubString, NSTriggerAtom> NSTriggerMapType;
    typedef std::map<String::SubString, NSTriggerAtom> NSUrlMapType;

    static const char* ASPECT;
  private:
    mutable Mutex_ lock_configuration_;
    mutable Sync::PosixMutex lock_statistic_;
    mutable Mutex_ lock_update_data_;
    mutable Mutex_ lock_ns_map_;
    const unsigned long count_chunks_;
    ChannelChunkArray_var chunks_;//chunks
    NSTriggerMapType ns_trigger_map_;//map triggers to NSTriggerAtom
    NSUrlMapType ns_url_map_;//map domain name to vector of matchers
    CCGMap_var ccgs_;
    Generics::Time master_;
    Generics::Time first_master_;
    Generics::Time start_update_;
    Generics::Time max_update_;
    bool non_strict_;

    ChannelServerStats stats_;
    volatile _Atomic_word queries_;
    volatile _Atomic_word exceptions_;
    volatile sig_atomic_t terminated_;
  };

}
}

namespace AdServer
{
  namespace ChannelSvcs
  {

  template<class Map>
  void fill_deleted(
    const Map& map,
    const Generics::Time& master,
    std::map<unsigned long, Generics::Time>& deleted)
    throw(eh::Exception)
  {
    for (typename Map::InactiveMap::const_iterator del_iter(
      map.inactive().begin());
      del_iter != map.inactive().end(); ++del_iter)
    {
      if(del_iter->second.timestamp > master)
      {
        deleted[del_iter->first] = del_iter->second.timestamp;
      }
    }
  }

  //
  // ChannelContainerBase class
  //
  
  inline
  ChannelContainerBase::ChannelUpdateData_var
  ChannelContainerBase::get_channel(unsigned long channel_id) const
    throw()
  {
    ChannelMap::const_iterator it = channel_ids_.find(channel_id);
    if (it != channel_ids_.end())
    {
      return ReferenceCounting::add_ref(it->second);
    }
    return ChannelUpdateData_var(nullptr);
  }


  inline
  ChannelMatchInfo* ChannelContainer::get_rules_() const
    throw()
  {
    return (*chunks_->rbegin())->get_info_ptr();
  }

  inline
  unsigned long ChannelContainer::get_count_chunks() const throw()
  {
    return count_chunks_;
  }

  /*calculate number of chunk in container by hash sum*/
  inline
  unsigned long ChannelContainerBase::calc_chunk_num_(
    unsigned long hash_sum,
    unsigned long count_chunks) const
    throw()
  {
    return (hash_sum % count_chunks);
  }

  inline
  CCGMap* ChannelContainer::get_ccg() const throw()
  {
    ReadGuard_ lock(lock_update_data_);
    return ReferenceCounting::add_ref(ccgs_);
  }

  //
  // ChannelContainer class
  //
  inline
  ChannelContainerBase::ChannelUpdateData_var
  ChannelContainer::get_channel(
    unsigned long channel_id) const throw()
  {
    ReadGuard_ lock(lock_update_data_);
    return ChannelContainerBase::get_channel(channel_id);
  }

  inline 
  void ChannelContainer::get_master(
    Generics::Time& first_master,
    Generics::Time& master,
    Generics::Time& longest_update) const
    throw()
  {
    master = master_;
    first_master = first_master_;
    longest_update = max_update_;
  }

  inline 
  ChannelMatchInfo_var ChannelContainer::get_active() const
    throw()
  {
    ReadGuard_ lock(lock_configuration_);
    return ReferenceCounting::add_ref(get_rules_());
  }

  inline
  void ChannelContainer::terminate() throw()
  {
    terminated_ = true;
    ChannelChunkArray_var chunk_array;
    {
      ReadGuard_ lock(lock_configuration_);
      chunk_array = ReferenceCounting::add_ref(chunks_);
    }
    for (ChannelChunkArray::const_iterator it(chunk_array->begin());
      it != chunk_array->end(); ++it)
    {
      (*it)->terminate();
    }
  }

  template<>
  inline
  unsigned long ChannelContainer::calc_chunk_num_i_(
    const String::SubString& trigger) const
    throw()
  {
    return calc_chunk_num_i_<Generics::SubStringHashAdapter>(
      Generics::SubStringHashAdapter(trigger));
  }

  /*calculate number of chunk in container*/
  template<class HASHADAPTER>
  unsigned long ChannelContainer::calc_chunk_num_i_(
    const HASHADAPTER& trigger) const
    throw()
  {
    return trigger.hash() % count_chunks_;
  }
  }//namespace ChannelSvcs
}

#endif //AD_SERVER_CHANNEL_CONTAINER

