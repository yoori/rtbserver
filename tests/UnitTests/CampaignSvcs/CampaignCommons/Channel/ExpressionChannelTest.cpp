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

#include <string>
#include <vector>

#include <String/StringManip.hpp>
#include <Generics/AppUtils.hpp>
#include <Commons/Algs.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannel.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannelIndex.hpp>
#include <CampaignSvcs/CampaignCommons/ExpressionChannelCorbaAdapter.hpp>
#include <CampaignSvcs/CampaignServer/ExpressionChannelParser.hpp>
#include <CampaignSvcs/CampaignServer/NonLinkedExpressionChannelCorbaAdapter.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>
#include <RequestInfoSvcs/ExpressionMatcher/ChannelMatcher.hpp>

using namespace AdServer::CampaignSvcs;

typedef std::unordered_map<unsigned long, ExpressionChannelHolder_var> ChannelCont;

ExpressionChannelBase_var
parse_for_channel(
  const char* expression,
  const ChannelParams& channel_params,
  ChannelCont& channels)
{
  NonLinkedExpressionChannel_var nl_ch =
    ExpressionChannelParser::parse(String::SubString(expression));
  nl_ch->params(channel_params);

  ExpressionChannelInfo channel_info;
  pack_non_linked_expression_channel(channel_info, nl_ch);
  return unpack_channel(channel_info, channels);
}

ExpressionChannelInfo
parse_for_channel(
  const char* expression,
  const ChannelParams& channel_params)
{
  NonLinkedExpressionChannel_var nl_ch =
    ExpressionChannelParser::parse(String::SubString(expression));
  nl_ch->params(channel_params);

  ExpressionChannelInfo channel_info;
  pack_non_linked_expression_channel(channel_info, nl_ch);
  return channel_info;
}

int check_parse() throw()
{
  ChannelCont channels;

  ExpressionChannel::Expression etalon;
  
  ChannelParams simple_channel_params_1;
  simple_channel_params_1.common_params = new ChannelParams::CommonParams;
  simple_channel_params_1.channel_id = 1;
  simple_channel_params_1.status = 'A';
  SimpleChannel_var simple_channel_1(new SimpleChannel(simple_channel_params_1));
  
  ChannelParams simple_channel_params_2;
  simple_channel_params_2.common_params = new ChannelParams::CommonParams;
  simple_channel_params_2.channel_id = 2;
  simple_channel_params_2.status = 'A';
  SimpleChannel_var simple_channel_2(new SimpleChannel(simple_channel_params_2));

  etalon.op = ExpressionChannel::OR;
  etalon.sub_channels.push_back(ExpressionChannel::Expression(simple_channel_1));
  etalon.sub_channels.push_back(ExpressionChannel::Expression(simple_channel_2));

  ChannelParams ex_channel_params;
  ex_channel_params.common_params = new ChannelParams::CommonParams;
  ex_channel_params.channel_id = 3;
  ex_channel_params.status = 'A';
  ExpressionChannelBase_var ch = parse_for_channel(
    "1 | 2", ex_channel_params, channels);
  
  if(channels.find(1) == channels.end() ||
     channels.find(2) == channels.end())
  {
    std::cerr << "not found expected channels : 1, 2" << std::endl;
    return 1;
  }

  channels[1]->channel = simple_channel_1;
  channels[2]->channel = simple_channel_2;

  {
    ChannelIdHashSet tr_channels;
    bool triggered = ch->triggered(&tr_channels, 0);
    if(triggered)
    {
      std::cerr << "channel triggered when non expected, channel: " << std::endl;
      print(std::cerr, ch);
      std::cerr << std::endl;
    }
  }
  
  {
    ChannelIdHashSet tr_channels;
    tr_channels.insert(1);
    bool triggered = ch->triggered(&tr_channels, 0);
    if(!triggered)
    {
      std::cerr << "channel not triggered when expected, channel: " << std::endl;
      print(std::cerr, ch);
      std::cerr << std::endl;
    }
  }
  
  {
    ChannelIdSet triggered_named_channels;
    ChannelIdHashSet tr_channels;
    tr_channels.insert(1);
    ch->triggered_named_channels(triggered_named_channels, tr_channels);

    if(triggered_named_channels.size() != 2 ||
       *triggered_named_channels.begin() != 1 ||
       *(++triggered_named_channels.begin()) != 3)
    {
      std::cerr << "triggered_named_channels: unexpected result: ";
      Algs::print(std::cerr,
        triggered_named_channels.begin(),
        triggered_named_channels.end());
      std::cerr << " instead 1, 3" << std::endl;
    }
  }
  
  return 0;
}

int check_parse_2() throw()
{
  ChannelCont channels;

  ChannelParams simple_channel_params_1;
  simple_channel_params_1.channel_id = 11;
  simple_channel_params_1.status = 'A';
  SimpleChannel_var simple_channel_1(new SimpleChannel(simple_channel_params_1));
  
  ChannelParams simple_channel_params_2;
  simple_channel_params_2.channel_id = 22;
  simple_channel_params_2.status = 'A';
  SimpleChannel_var simple_channel_2(new SimpleChannel(simple_channel_params_2));
  
  ChannelParams simple_channel_params_3;
  simple_channel_params_3.channel_id = 33;
  simple_channel_params_3.status = 'A';
  SimpleChannel_var simple_channel_3(new SimpleChannel(simple_channel_params_3));

  ExpressionChannel::Expression etalon;

  {
    ExpressionChannel::Expression sub_etalon;
    sub_etalon.op = ExpressionChannel::OR;
    sub_etalon.sub_channels.push_back(ExpressionChannel::Expression(simple_channel_1));
    sub_etalon.sub_channels.push_back(ExpressionChannel::Expression(simple_channel_2));
    
    etalon.op = ExpressionChannel::AND;
    etalon.sub_channels.push_back(sub_etalon);
    etalon.sub_channels.push_back(ExpressionChannel::Expression(simple_channel_3));
  }
  
  ChannelParams ex_channel_params;
  ex_channel_params.channel_id = 1;
  ex_channel_params.status = 'A';
  ExpressionChannelBase_var ch = parse_for_channel(
    " ( ( ( ( ( 11 ) ) | ( ( 22 ) ) ) & ( ( ( 33 ) ) ) ) )",
    ex_channel_params, channels);
  
  if(channels.find(11) == channels.end() ||
     channels.find(22) == channels.end() ||
     channels.find(33) == channels.end())
  {
    std::cerr << "not found expected channels : 11, 22, 33" << std::endl <<
      "presented : ";
    for(ChannelCont::const_iterator ch_it = channels.begin();
        ch_it != channels.end(); ++ch_it)
    {
      std::cerr << (ch_it != channels.begin() ? ", " : "") << ch_it->first;
    }
    std::cerr << std::endl;
    return 1;
  }

  channels[11]->channel = simple_channel_1;
  channels[22]->channel = simple_channel_2;
  channels[33]->channel = simple_channel_3;

  {
    ChannelIdHashSet tr_channels;
    bool triggered = ch->triggered(&tr_channels, 0);
    if(triggered)
    {
      std::cerr << "channel triggered when non expected, channel: " << std::endl;
      print(std::cerr, ch);
      std::cerr << std::endl;
    }
  }
  
  {
    ChannelIdHashSet tr_channels;
    tr_channels.insert(11);
    tr_channels.insert(33);
    bool triggered = ch->triggered(&tr_channels, 0);
    if(!triggered)
    {
      std::cerr << "channel not triggered when expected, channel: " << std::endl;
      print(std::cerr, ch);
      std::cerr << std::endl;
    }
  }

  return 0;
}

int
check_parse_invalid() throw()
{
  typedef std::unordered_map<unsigned long, ExpressionChannelHolder_var> ChannelCont;
  ChannelCont channels;

  ChannelParams ex_channel_params;
  ex_channel_params.channel_id = 1;
  ex_channel_params.status = 'A';

  int result = 0;

  {
    bool is_invalid = false;
    try
    {
      ExpressionChannelBase_var ch = parse_for_channel(
        "()", ex_channel_params, channels);
    }
    catch(const eh::Exception& ex)
    {
      is_invalid = true;
    }

    if(!is_invalid)
    {
      result += 1;
    }
  }

  return result;
}

int check_tree_construct()
{
  ChannelCont channels;
  
  {
    ChannelParams ch_params;
    ch_params.channel_id = 1;
    ExpressionChannelBase_var ch = parse_for_channel(
      "2 | 3", ch_params, channels);
    channels[1] = ExpressionChannelHolder_var(new ExpressionChannelHolder(ch));
  }
  
  {
    ChannelParams ch_params;
    ch_params.channel_id = 2;
    ExpressionChannelBase_var ch = parse_for_channel(
      "4 & 5", ch_params, channels);
    channels[2] = ExpressionChannelHolder_var(new ExpressionChannelHolder(ch));
  }

  {
    channels[3]->channel = new ExpressionChannel(ChannelParams(3));
    channels[4]->channel = new ExpressionChannel(ChannelParams(4));
    channels[5]->channel = new ExpressionChannel(ChannelParams(5));
  }

  return 0;
}

int check_custom_tree_construct(
  const char* file,
  const char* match_file,
  unsigned long cache_size)
{
  const unsigned long REPEAT_COUNT = 1;
  const bool PRINT = false;

  ChannelCont channels;

  std::cout << "indexing started" << std::endl;

  {
    std::vector<ExpressionChannelInfo> channels_info;
    std::ifstream ifile(file);

    while(!ifile.eof())
    {
      std::string str;
      std::getline(ifile, str);

      if(!str.empty())
      {
        String::StringManip::Splitter<String::AsciiStringManip::SepComma> split(str);
        String::SubString channel_id_str;
        String::SubString expression_str;
        if(split.get_token(channel_id_str) &&
          split.get_token(expression_str))
        {
          ExpressionChannelBase_var ch;

          ChannelParams ch_params;
          String::StringManip::str_to_int(channel_id_str, ch_params.channel_id);
          ch_params.common_params = new ChannelParams::CommonParams;
          ch_params.status = 'A';

          if(channel_id_str.compare(expression_str) == 0)
          {
            ch = new SimpleChannel(ch_params);
          }
          else
          {
            ch = parse_for_channel(
              expression_str.str().c_str(), ch_params, channels);
          }

          ExpressionChannelHolder_var& ch_holder = channels[ch_params.channel_id];
          if(ch_holder)
          {
            ch_holder->channel = ch;
          }
          else
          {
            ch_holder = new ExpressionChannelHolder(ch);
          }
        }
        else
        {
          std::cerr << "invalid line '" << str << "'" << std::endl;
        }
      }
    }

    for (auto i = channels_info.begin(); i != channels_info.end(); ++i)
    {
      ChannelCont::iterator it = channels.find(i->channel_id);

      if (it == channels.end())
      {
        channels[i->channel_id] = ExpressionChannelHolder_var(
          new ExpressionChannelHolder(unpack_channel(*i, channels)));
      }
      else
      {
        it->second->channel = unpack_channel(*i, channels);
      }
    }
  }

  std::cout << "channels.size() = " << channels.size() << std::endl;
  // fill stubs
  for(ChannelCont::iterator ch_it = channels.begin();
      ch_it != channels.end(); ++ch_it)
  {
    if(!ch_it->second->channel.in())
    {
      ChannelParams simple_channel_params;
      simple_channel_params.common_params = new ChannelParams::CommonParams;
      simple_channel_params.channel_id = ch_it->first;
      simple_channel_params.status = 'A';
      SimpleChannel_var simple_channel(new SimpleChannel(simple_channel_params));
      ch_it->second->channel = simple_channel;
    }
  }

  AdServer::RequestInfoSvcs::ChannelMatcher_var channel_matcher =
    new AdServer::RequestInfoSvcs::ChannelMatcher(
      0,
      cache_size,
      Generics::Time::ONE_DAY);

  AdServer::RequestInfoSvcs::ChannelMatcher::Config_var config =
    new AdServer::RequestInfoSvcs::ChannelMatcher::Config();
  config->expression_channels.swap(channels);

  channel_matcher->config(config);

  channels.clear();

  /*
  AdServer::CampaignSvcs::ExpressionChannelIndex_var index =
    new AdServer::CampaignSvcs::ExpressionChannelIndex();
  index->index(channels);
  */

  std::list<ChannelIdSet> match_groups;

  std::cout << "indexing finished" << std::endl;
  std::cout << "start testing" << std::endl;

  if(match_file)
  {
    std::ifstream imatch_file(match_file);

    while(!imatch_file.eof())
    {
      std::string str;
      std::getline(imatch_file, str);

      if(!str.empty())
      {
        ChannelIdSet channels;
        String::StringManip::Splitter<String::AsciiStringManip::SepComma> split(str);
        String::SubString channel_id_str;
        while(split.get_token(channel_id_str))
        {
          unsigned long channel_id;
          if(String::StringManip::str_to_int(channel_id_str, channel_id))
          {
            channels.insert(channel_id);
          }
        }

        /*
        {
          ChannelIdSet result_channels;
          ChannelIdSet result_estimate_channels;

          for(unsigned long i = 0; i < REPEAT_COUNT; ++i)
          {
            channel_matcher->process_request(
              channels,
              result_channels,
              &result_estimate_channels);
          }
        }
        */

        match_groups.push_back(channels);
      }
    }
  }
  else
  {
    ChannelIdSet match_channels;
    match_channels.insert(427559);
    match_channels.insert(616269);
    match_channels.insert(1156235);
    match_channels.insert(1891019);
    match_channels.insert(3581706);
    match_channels.insert(3612040);
    match_groups.push_back(match_channels);
  }

  //index->print(std::cout, &match_channels, true);

  auto mit = match_groups.begin();

  //int k = 0;
  Generics::CPUTimer timer;
  timer.start();

  while(mit != match_groups.end())
  {
    ChannelIdSet result_channels;
    ChannelIdSet result_estimate_channels;

    for(unsigned long i = 0; i < REPEAT_COUNT; ++i)
    {
      /*
      Generics::CPUTimer sub_timer;
      sub_timer.start();
      */
      channel_matcher->process_request(
        *mit,
        result_channels,
        &result_estimate_channels);

      /*
      index->match(
        result_channels,
        *mit,
        0);
      */
      /*
      sub_timer.stop();
      std::cout << (k++) << ": " << sub_timer.elapsed_time() << std::endl;
      */
    }

    if(PRINT)
    {
      Algs::print(std::cout, mit->begin(), mit->end(), ",");
      std::cout << " => ";
      Algs::print(std::cout, result_channels.begin(), result_channels.end(), ",");
      std::cout << std::endl;
    }

    ++mit;
  }

  timer.stop();
  std::cout << timer.elapsed_time() << std::endl;
  //std::cout << "result_channels.size() = " << result_channels.size() << std::endl;
  /*
  std::cout << "Index: " << std::endl;
  index->print(std::cout);
  */

  match_groups.clear();

//sleep(10000);

  return 0;
}

int check_tree_mem_use()
{
  typedef std::map<unsigned long, ExpressionChannelHolder_var> ChannelCont;
  ChannelCont channels;

  for(unsigned long i = 0; i < 500000; ++i)
  {
    channels[i] = ExpressionChannelHolder_var(
      new ExpressionChannelHolder(
        new ExpressionChannel(ChannelParams(i))));
  }

  AdServer::CampaignSvcs::ExpressionChannelIndex_var index =
    new AdServer::CampaignSvcs::ExpressionChannelIndex();

  //index->index(channels);

  return 0;
}

int main(int argc, char** argv)
{
  int result = 0;

  using namespace Generics::AppUtils;
  Args args(-1);
  Option<unsigned long> opt_cache_size(100*1024*1024);
  args.add(equal_name("cache-size") || short_name("c"), opt_cache_size);
  args.parse(argc - 1, argv + 1);
  const Args::CommandList& commands = args.commands();

  check_custom_tree_construct(
    commands.begin()->c_str(),
    commands.size() > 1 ? (++commands.begin())->c_str() : 0,
    *opt_cache_size);

  //sleep(10000);

  /*
  result += check_parse();
  result += check_parse_2();
  result += check_parse_invalid();
  result += check_tree_construct();
  //check_tree_mem_use();
  */
  return result;
}
