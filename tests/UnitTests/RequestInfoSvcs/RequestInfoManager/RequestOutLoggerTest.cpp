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

// @file RequestInfoSvcs/RequestOutLoggerTest.cpp

#include <Generics/AppUtils.hpp>
#include <Generics/Uuid.hpp>
#include <Generics/DirSelector.hpp>
#include <RequestInfoSvcs/RequestInfoManager/RequestOutLogger.hpp>

using namespace AdServer::RequestInfoSvcs;

namespace
{
  const char DEFAULT_ROOT_PATH[] = "./";
  Generics::AppUtils::Option<std::string> root_path(DEFAULT_ROOT_PATH);

  const char TEST_FOLDER[] = "/RequestOutLoggerTestDir/";

  const char USAGE[] =
    "RequestOutLoggerTest [OPTIONS]\n"
    "OPTIONS:\n"
    "  -p, --path : path to folder with temporary files.\n"
    "  -h, --help : show this message.\n";
}


int
request_out_logger_test(const char* tmp_dir)
{
  AdServer::LogProcessing::LogFlushTraits log_traits;
  log_traits.out_dir = tmp_dir;
  RequestOutLogger_var request_out_logger(
    new RequestOutLogger(
      0,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      log_traits,
      &log_traits,
      &log_traits,
      &log_traits,
      &log_traits,
      AdServer::Commons::LogReferrer::LR_PATH,
      1));

  Generics::Uuid uuid = Generics::Uuid::create_random_based();
  AdServer::Commons::RequestId req_id(uuid.to_string().c_str());
  RequestInfo request_info(req_id);
  request_info.user_status = 'I';
  request_info.pub_floor_cost = RevenueDecimal::ZERO;
  request_info.pub_bid_cost = RevenueDecimal::ZERO;
  request_info.adv_revenue.currency_rate = RevenueDecimal(1);
  request_info.pub_revenue.currency_rate = RevenueDecimal(1);
  request_info.isp_revenue.currency_rate = RevenueDecimal(1);

  request_out_logger->process_request(
    request_info,
    RequestActionProcessor::ProcessingState());
  request_out_logger->flush_if_required();

  request_out_logger->process_impression(
    request_info,
    ImpressionInfo(),
    RequestActionProcessor::ProcessingState());
  request_out_logger->flush_if_required();

  request_out_logger->process_click(
    request_info,
    RequestActionProcessor::ProcessingState());
  request_out_logger->flush_if_required();

  request_out_logger->process_action(request_info);
  request_out_logger->flush_if_required();

  return 0;
}

bool
init(int& argc, char**& argv) throw (eh::Exception)
{
  using namespace Generics::AppUtils;
  Args args;
  CheckOption opt_help;

  args.add(
    equal_name("path") ||
    short_name("p"),
    root_path);
  args.add(
    equal_name("help") ||
    short_name("h"),
    opt_help);

  args.parse(argc - 1, argv + 1);

  if (opt_help.enabled())
  {
    std::cout << USAGE << std::endl;
    return false;
  }
  return true;
}

int
main(int argc, char* argv[]) throw ()
{
  try
  {
    if (!init(argc, argv))
    {
      return 0;
    }
    system(("rm -r " + *root_path + TEST_FOLDER +
      " 2>/dev/null ; mkdir -p " + *root_path + TEST_FOLDER).c_str());
    return request_out_logger_test(
      (*root_path + TEST_FOLDER).c_str());
  }
  catch (const eh::Exception& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  return -1;
}
