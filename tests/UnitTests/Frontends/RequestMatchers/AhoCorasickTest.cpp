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

#include <iostream>
#include <vector>
#include <Commons/Algs.hpp>
#include <Frontends/FrontendCommons/AhoCorasick.hpp>

using namespace FrontendCommons;

typedef int Tag;
typedef std::vector<Tag> Tags;

class MatchCallback : public std::unary_function<int, void>
{
public:
  MatchCallback(Tags& matched_tags) throw ()
    : matched_tags_(matched_tags)
  {}

  bool
  operator()(const AhoCorasik<Tag>::MatchDetails& details) const throw ()
  {
    matched_tags_.push_back(details.tag);
    return false;
  }

private:
  Tags& matched_tags_;
};

template<const unsigned int ETALON_SIZE>
bool
check_match_result(
  const char* test_name,
  const Tags& matched_tags,
  const int (&etalon_tags)[ETALON_SIZE])
{
  if(matched_tags.size() != ETALON_SIZE ||
     !std::equal(matched_tags.begin(), matched_tags.end(), etalon_tags))
  {
    std::cerr << test_name << ": unexpected match result:" << std::endl << "  ";
    Algs::print(std::cerr, matched_tags.begin(), matched_tags.end());
    std::cerr << std::endl << "instead" << std::endl << "  ";
    Algs::print(std::cerr, etalon_tags, etalon_tags + ETALON_SIZE);
    std::cerr << std::endl;
    return false;
  }

  return true;
}

int
empty_do_not_fault_test() throw ()
{
  static const char* TEST = "empty_do_not_fault_test";

  Tags matched_tags;
  MatchCallback callback(matched_tags);
  AhoCorasik<Tag> matcher;

  matcher.match(String::SubString("AAAAA"), callback);
  if(!matched_tags.empty())
  {
    std::cerr << TEST << ": error, non empty result: ";
    Algs::print(std::cerr, matched_tags.begin(), matched_tags.end());
    std::cerr << std::endl;
    return 1;
  }
  std::cout << TEST << ": success" << std::endl;
  return 0;
}

int
match_test_1() throw ()
{
  static const char* TEST = "match_test_1";

  Tags matched_tags;
  MatchCallback callback(matched_tags);
  AhoCorasik<Tag> matcher(false);

  matcher.add_pattern("win", 1);
  matcher.add_pattern("winwin", 2);
  matcher.add_pattern("abbat", 3);
  matcher.add_pattern("Abba", 4);
  matcher.add_pattern("AAB", 5);

  matcher.match(String::SubString("awinwinwinwinAbbatAAABBA"), callback);

  const int ETALON_TAGS[] = { 1, 1, 2, 1, 2, 1, 2, 4, 3, 5, 4 };
  if(!check_match_result(TEST, matched_tags, ETALON_TAGS))
  {
    return 1;
  }

  std::cout << TEST << ": success" << std::endl;
  return 0;
}

int
match_test_2() throw ()
{
  // regression
  static const char* TEST = "match_test_2";

  Tags matched_tags;
  MatchCallback callback(matched_tags);
  AhoCorasik<Tag> matcher(false);

  matcher.add_pattern("abcde", 1);
  matcher.add_pattern("bcd", 2);
  matcher.add_pattern("abc", 3);
  matcher.add_pattern("cde", 4);

  matcher.match(String::SubString("ababcdeab"), callback);

  const int ETALON_TAGS[] = { 3, 2, 1, 4 };
  if(!check_match_result(TEST, matched_tags, ETALON_TAGS))
  {
    return 1;
  }

  std::cout << TEST << ": success" << std::endl;
  return 0;
}

int main() throw ()
{
  int res = empty_do_not_fault_test();
  res += match_test_1();
  res += match_test_2();
  return res;
}
