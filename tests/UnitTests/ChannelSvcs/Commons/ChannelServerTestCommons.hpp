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

#ifndef CHANNELSERVERTESTCOMMONS_HPP
#define CHANNELSERVERTESTCOMMONS_HPP
#include <string>
#include <sstream>
#include <eh/Exception.hpp>
#include <Generics/Uuid.hpp>
#include <Generics/Statistics.hpp>
#include <ChannelSvcs/ChannelServer/ChannelContainer.hpp>
#include <ChannelSvcs/ChannelServer/UpdateContainer.hpp>

namespace AdServer
{
namespace UnitTests
{
  template<class T>
  void read_number(
    const char* in,
    T min_value,
    T max_value,
    T &value) throw()
  {
    T converted;
    std::stringstream convert;
    convert << in;
    convert >> converted;
    if(converted >= min_value && converted <= max_value)
    {
      value = converted;
    }
  }

  class FixedBuf : protected std::string
  {
    typedef std::string Base;
  public:
    FixedBuf(std::size_t max_size = 512)
      : max_size_(max_size)
    {}

    using Base::size;
    using Base::c_str;
    using Base::begin;
    using Base::end;
    using Base::clear;

    bool
    append(const char* str, std::size_t len)
    {
      if (size() + len <= max_size_)
      {
        Base::append(str, len);
        return true;
      }
      Base::append(str, max_size_ - size());
      return false;
    }

    void
    reset_max_size(std::size_t new_size)
    {
      clear();
      max_size_ = new_size;
      reserve(max_size_);
    }

    std::size_t
    max_size() const
    {
      return max_size_;
    }

    operator String::SubString() const throw ()
    {
      return String::SubString(*this);
    }
  private:
    std::size_t max_size_;
  };

  class ChannelServerTestCommons
  {
  public:
    /**
     * word result of generation
     * length count of unicode symbols
     * */
    static std::string& generate_uid_word(std::string& word)
      throw(eh::Exception);
    /**
     * word result of generation
     * length count of unicode symbols
     * */
    static std::string& generate_word(std::string& word, size_t length = 0)
      throw(eh::Exception);
    /**
     * @return true, if wrote whole word into word
     */
    static bool
    generate_asc_word(
      FixedBuf& word,
      size_t length)
      throw (eh::Exception);
    static void generate_url(std::string& word, size_t domain_length, size_t path_length, size_t path_steps = 2)
      throw(eh::Exception);
    static void generate_asc_word(std::string& word, size_t length = 10)
      throw(eh::Exception);
    static void print_result(
      std::ostream& out,
      const ChannelSvcs::TriggerMatchRes& res)
      throw();
    static ChannelSvcs::MergeAtom& new_atom(
      unsigned long id,
      size_t type,
      ChannelSvcs::MergeAtom& atom,
      ChannelSvcs::ChannelIdToMatchInfo* info)
      throw(eh::Exception);
  };

  class TestTemplate
  {
  public:
    TestTemplate() throw();

    void parse_arguments(int argc, char* argv[]) throw();
  protected:
    void make_match_(
      const ChannelSvcs::MatchUrls& urls,
      const ChannelSvcs::MatchWords words[ChannelSvcs::CT_MAX])
      throw();

    void dump_statistic_(
      std::ostream& ost,
      Generics::Statistics::TimedStatSink* time_stat = 0,
      Generics::Statistics::TimedStatSink* cpu_time_stat = 0)
      throw();

    void dump_memory_statistic_(std::ostream& ost) throw();

  protected:
    bool body_;
    unsigned long count_urls_;
    unsigned long count_hards_;
    unsigned long count_soft_;
    unsigned long urls_length_;
    unsigned long hard_length_;
    unsigned long sentence_words_;
    unsigned long split_percent_;
    FixedBuf match_data_;
    unsigned int verbose_level_;
    unsigned long soft_length_;//length of soft word;
    unsigned long word_length_;//count words
    unsigned long count_chunks_;//count chunks in container
    unsigned long quires_;//count quieres
    unsigned long res_size_;//count matched triggers
    unsigned long zero_match_;//count quires without matching
    unsigned long data_size_;//size of generating data
    unsigned long match_percent_;//percent of matching quires
    unsigned long avg_limit_;//limit on averagy time
    time_t time_;
    ChannelSvcs::ChannelContainer container_;
    Generics::Statistics::TimedStatSink_var time_stat_;
    Generics::Statistics::TimedStatSink_var cpu_time_stat_;
  };
}
}
#endif

