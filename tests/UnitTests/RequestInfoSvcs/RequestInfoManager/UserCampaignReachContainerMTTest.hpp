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

/**
 * @file UserCampaignReachContainerMTTest.hpp
 */
#ifndef USER_CAMPAIGN_REACH_CONTAINER_MT_TEST_HPP_INCLUDED
#define USER_CAMPAIGN_REACH_CONTAINER_MT_TEST_HPP_INCLUDED

#include <list>

#include <Sync/PosixLock.hpp>
#include <RequestInfoSvcs/RequestInfoManager/UserCampaignReachContainer.hpp>


typedef std::list<
  AdServer::RequestInfoSvcs::CampaignReachProcessor::ReachInfo> ReachInfoList;

struct CollReachTestProcessor :
  public AdServer::RequestInfoSvcs::CampaignReachProcessor,
  public virtual ReferenceCounting::AtomicImpl
{
  typedef AdServer::RequestInfoSvcs::CampaignReachProcessor::Exception
    Exception;

  void
  process_reach(
    const AdServer::RequestInfoSvcs::CampaignReachProcessor::ReachInfo&
      reach_info)
    throw (Exception)
  {
    Sync::PosixGuard lock(mutex_);
    result_.push_back(reach_info);
  }

  const ReachInfoList&
  result() const
  {
    return result_;
  }

  void
  clear()
  {
    result_.clear();
  }

protected:
  virtual ~CollReachTestProcessor() throw()
  {}

private:
  Sync::PosixMutex mutex_;
  ReachInfoList result_;
};

struct TestIt
{
  AdServer::RequestInfoSvcs::UserCampaignReachContainer* reach_container;
  CollReachTestProcessor* reach_processor;
};

/**
 * Multithreading test
 */
bool
multi_thread_test(const TestIt* test_it) throw ();


#endif // USER_CAMPAIGN_REACH_CONTAINER_MT_TEST_HPP_INCLUDED
