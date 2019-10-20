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

#ifndef _USERCAMPAIGNREACHPROFILEUTILS_HPP_
#define _USERCAMPAIGNREACHPROFILEUTILS_HPP_

#include <Generics/Time.hpp>
#include <UtilCommons/Table.hpp>
#include <Commons/Algs.hpp>
#include <RequestInfoSvcs/RequestInfoCommons/Algs.hpp>
#include <RequestInfoSvcs/RequestInfoCommons/UserCampaignReachProfile.hpp>

namespace AdServer
{
namespace RequestInfoSvcs
{
  void
  print_user_campaign_reach_profile(std::ostream& out,
    const UserCampaignReachProfileReader& reader)
    throw();
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  namespace
  {
    const Table::Column USER_CAMPAIGN_REACH_TABLE_COLUMNS[] =
    {
      Table::Column("campaign_appearance_list", Table::Column::TEXT),
      Table::Column("ccg_appearance_list", Table::Column::TEXT),
      Table::Column("creative_appearance_list", Table::Column::TEXT),
      Table::Column("adv_appearance_list", Table::Column::TEXT),
      Table::Column("adv_text_appearance_list", Table::Column::TEXT),
      Table::Column("adv_display_appearance_list", Table::Column::TEXT),
    };
  }

  inline
  void
  print_user_campaign_reach_profile(
    std::ostream& out,
    const UserCampaignReachProfileReader& reader)
    throw()
  {
    unsigned long columns =
      sizeof(USER_CAMPAIGN_REACH_TABLE_COLUMNS) /
      sizeof(USER_CAMPAIGN_REACH_TABLE_COLUMNS[0]);

    Table table(columns);

    for(unsigned long i = 0; i < columns; i++)
    {
      table.column(i, USER_CAMPAIGN_REACH_TABLE_COLUMNS[i]);
    }

    Table::Row row(table.columns());

    std::string space_align(table.value_align(), ' ');
    row.add_field(stringify_appearance_list(reader.campaign_appears(), space_align.c_str()));
    row.add_field(stringify_appearance_list(reader.ccg_appears(), space_align.c_str()));
    row.add_field(stringify_appearance_list(reader.cc_appears(), space_align.c_str()));
    row.add_field(stringify_appearance_list(reader.adv_appears(), space_align.c_str()));
    row.add_field(stringify_appearance_list(reader.adv_text_appears(), space_align.c_str()));
    row.add_field(stringify_appearance_list(reader.adv_display_appears(), space_align.c_str()));

    table.add_row(row);
    table.dump(out);
  }
}
}

#endif /*_USERCAMPAIGNREACHPROFILEUTILS_HPP_*/
