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

#include "../../TestHelpers.hpp"

#include <UserInfoSvcs/UserInfoCommons/UserOperationProfiles.hpp>
#include <UserInfoSvcs/UserInfoManager/UserOperationSaver.hpp>
#include <UserInfoSvcs/UserInfoManager/Compatibility/UserOperationProfiles_v310.hpp>
#include <UserInfoSvcs/UserInfoManager/Compatibility/UserOperationProfilesAdapter.hpp>

TEST(UserOperationProfilesAdapter)
{
  Generics::SmartMemBuf_var mem_buf(new Generics::SmartMemBuf());
  AdServer::UserInfoSvcs_v310::UserFreqCapUpdateOperationWriter writer;
  writer.operation_type() = AdServer::UserInfoSvcs::UserOperationSaver::UO_FC_UPDATE;
  writer.version() = 310;
  writer.user_id() = "7C4hkdl4R6Swwrb0iu1gbg..";
  writer.time() = 1442591736;
  writer.request_id() = "WnOBpn1NSHuLIVPc1JAJ9w..";

  AdServer::UserInfoSvcs_v310::SeqOrderDescriptorWriter seq_order;
  seq_order.ccg_id() = 1001;
  seq_order.set_id() = 555;
  seq_order.imps() = 4;
  writer.seq_orders().push_back(seq_order);

  seq_order.ccg_id() = 1002;
  seq_order.set_id() = 556;
  seq_order.imps() = 3;
  writer.seq_orders().push_back(seq_order);

  mem_buf->membuf().alloc(writer.size());
  writer.save(mem_buf->membuf().data(), mem_buf->membuf().size());
  AdServer::UserInfoSvcs::UserFreqCapUpdateOperationProfilesAdapter()(mem_buf);

  const AdServer::UserInfoSvcs::UserFreqCapUpdateOperationReader reader(
    mem_buf->membuf().data(), mem_buf->membuf().size());

  ASSERT_EQUALS (reader.version(), 351U);
  ASSERT_EQUALS (reader.time(), 1442591736U);
  ASSERT_EQUALS (reader.request_id(), std::string("WnOBpn1NSHuLIVPc1JAJ9w.."));
  ASSERT_EQUALS (reader.seq_orders().size(), 2U);
  ASSERT_EQUALS (reader.seq_orders().back().imps(), 3U);
}

RUN_TESTS
