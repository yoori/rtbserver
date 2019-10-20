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


#ifndef AD_SERVER_UPDATE_CONTAINER
#define AD_SERVER_UPDATE_CONTAINER

#include <map>
#include <set>
#include <deque>
#include <vector>

#include <eh/Exception.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <ChannelSvcs/ChannelCommons/TriggerParser.hpp>
#include "ChannelContainer.hpp"
#include "DictionaryMatcher.hpp"

namespace AdServer
{
namespace ChannelSvcs
{

  class ChannelContainerBase; 

  class UpdateContainer:
    public TriggerParser::MergeContainer,
    public ChannelContainerBase
  {
  public:
    UpdateContainer(
      ChannelContainer* cont,
      DictionaryMatcher* dict)
      throw();

    ~UpdateContainer() throw() {};

    /*Callback of parser
     * add new triggers to container */
    virtual size_t add_trigger(MergeAtom& merge_atom)
      throw(TriggerParser::Exception);

    /*Build list of removed trigger
     * @info - actual channel information
     * Build list of removed trigger*/
    size_t check_actual(ChannelIdToMatchInfo& info) throw();

    /* list of updated channels*/
    const ExcludeContainerType& get_updated() throw();

    /* list of removed channels*/
    const ExcludeContainerType& get_removed() throw();

    /* list of new channels*/
    const ExcludeContainerType& get_new() throw();

    /*select triggers for parsing and mark removed triggers
     * @channel_id - channel
     * @triggers - list of triggers for parsing
     * @load_once - this channel can't be updated
     * just loaded for one time
     */
    size_t select_parsed_triggers(
      unsigned long channel_id,
      TriggerList& triggers,
      bool load_once)
      throw();

    virtual bool ready() const throw();

    const ChannelContainer* get_helper() const throw();

    /* fill cache of lexemes for unmerged triggers */
    void prepare_merge(const UnmergedMap& unmerged) throw(Exception);

    /* get lexemes for new triggers
     * @lang - language
     * @parts - parts of triggers
     * @lexemes - founded lexemes for triggers
     * */
    void fill_lexemes(
      unsigned short lang,
      const SubStringVector& parts,
      LexemesPtrVector& lexemes) const
      throw ();

    /* get old information about channels
     * @returns pointer to information
     * */
    ChannelIdToMatchInfo_var get_old_info() throw ();

    typedef std::map<unsigned int, MatterItem> Matters;

    Matters& get_matters() throw();

    const Matters& get_matters() const throw();

    typedef std::map<
      unsigned short, DictionaryMatcher::LexemeCache> Lexemes;

  private:

    void cleanup_() throw();

  private:
    Matters matters_;
    ExcludeContainerType new_channels_;
    ExcludeContainerType updated_channels_;
    ExcludeContainerType removed_channels_;
    const ChannelContainer* helper_;
    DictionaryMatcher* dict_matcher_;
    Lexemes lexemes_;
  };

}
}

namespace AdServer
{
  namespace ChannelSvcs
  {

  inline
  UpdateContainer::Matters& UpdateContainer::get_matters() throw()
  {
    return matters_;
  }

  inline
  const UpdateContainer::Matters& UpdateContainer::get_matters() const throw()
  {
    return matters_;
  }


  inline
  bool UpdateContainer::ready() const throw()
  {
    if(!dict_matcher_)
    {
      return true;
    }
    return dict_matcher_->ready();
  }

  inline
  const ChannelContainer* UpdateContainer::get_helper() const throw()
  {
    return helper_;
  }

  inline
  const ExcludeContainerType& UpdateContainer::get_removed() throw()
  {
    return removed_channels_;
  }

  inline
  const ExcludeContainerType& UpdateContainer::get_updated() throw()
  {
    return updated_channels_;
  }

  inline
  const ExcludeContainerType& UpdateContainer::get_new() throw()
  {
    return new_channels_;
  }

  }//namespace ChannelSvcs
}

#endif //AD_SERVER_UPDATE_CONTAINER

