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

#include "ChannelCategoryGrannularUpdateTest.hpp"

REFLECT_UNIT(ChannelCategoryGrannularUpdateTest) (
  "GranularUpdate",
  AUTO_TEST_SLOW
);

namespace
{
  namespace ORM = ::AutoTest::ORM;
  typedef AutoTest::ChannelCategoryChecker ChannelCategoryChecker;
  typedef AutoTest::WaitChecker<ChannelCategoryChecker> ChannelCategoryWaitChecker;
}

void
ChannelCategoryGrannularUpdateTest::set_up()
{
  add_descr_phrase("Setup started.");
}
 
bool 
ChannelCategoryGrannularUpdateTest::run()
{
  add_category();
  deactivate_category();
  
  return true;
}

void
ChannelCategoryGrannularUpdateTest::add_category()
{
  std::string description("Add category.");
  add_descr_phrase(description);

  ORM::ORMRestorer<ORM::CategoryChannel>* category =
    create<ORM::CategoryChannel>();

  category->account = fetch_int("InternalAccount");
  category->name = fetch_string("CategoryName");
  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      category->insert()),
    "must insert category channel!");

  add_checker(
    description,
    ChannelCategoryWaitChecker(
      ChannelCategoryChecker(
        this,
        category->id(),
        ChannelCategoryChecker::Expected().
          channel_id(category->id()).
          parent_channel_id(0))));
}

void
ChannelCategoryGrannularUpdateTest::deactivate_category()
{
  std::string description("Deactivate category.");
  add_descr_phrase(description);

  ORM::ORMRestorer<ORM::CategoryChannel>* category =
    create<ORM::CategoryChannel>(fetch_int("ChannelCategory"));

  FAIL_CONTEXT(
    ChannelCategoryWaitChecker(
      ChannelCategoryChecker(this,
        category->id(),
        ChannelCategoryChecker::Expected().
          channel_id(category->id()).
          parent_channel_id(0))).check(),
    description + " Initial");

 category->status = "D";
 category->display_status_id = 5; // 5 = Deleted
  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      category->update(false)),
   "must deactivate channel category");
 
 add_checker(
   description,
   ChannelCategoryWaitChecker(
     ChannelCategoryChecker(
        this,
        category->id(),
        ChannelCategoryChecker::Expected(),
        AutoTest::AEC_NOT_EXISTS)));
}

void
ChannelCategoryGrannularUpdateTest::tear_down()
{
  add_descr_phrase("Tear down started.");
}
