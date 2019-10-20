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


namespace AdServer
{
namespace UnitTests
{
  class TriggerSerializationTest
  {
  public:

    TriggerSerializationTest() : trigger_type_(0) {};

    struct SoftWord
    {
      unsigned int channel_trigger_id;
      std::string trigger;
      ChannelSvcs::Parts parts;
      char type;
      bool exact;
    };

    int run(int argc, char* argv[]) throw();

    void generate_soft_trigger_word(
      size_t iteration,
      size_t max_parts,
      size_t max_word_len,
      SoftWord& word,
      std::string& trigger) throw();

  private:
    int regular_test_case_() throw();
    int print_compary_result_(
      const SoftWord& word1,
      const std::string& trigger_in,
      char type,
      const std::string& trigger,
      bool exact,
      bool negative) throw();
    int print_compary_parts_(
      size_t iteration,
      const ChannelSvcs::Parts& words1,
      const ChannelSvcs::SubStringVector& parts2,
      const char *result,
      size_t len) throw();

    std::string trigger_;
    char trigger_type_;

  };
}
}

