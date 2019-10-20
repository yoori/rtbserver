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

/** $Id$
 * @file AutoTestsXsd.hpp
 * AutoTests configs wrapper
 */
#ifndef __AUTOTESTS_COMMONS_AUTOTESTSXSD_HPP
#define __AUTOTESTS_COMMONS_AUTOTESTSXSD_HPP


#include <xsd/tests/AutoTests/AutoTests.hpp>
#include <xsd/tests/AutoTests/LocalParams.hpp>
#include <eh/Exception.hpp>

// Global config types aliases
typedef xsd::AdServer::Configuration::GeneralParamsType GeneralParamsType;
typedef const GeneralParamsType& Params;
typedef xsd::AdServer::Configuration::ClusterEnum ClusterEnum;
typedef xsd::AdServer::Configuration::ServiceEnum ServiceEnum;

// Local config types aliases
typedef const xsd::tests::AutoTests::LocalParamsType& AllLocals;
typedef const xsd::tests::AutoTests::UnitLocalDataType& Locals;

enum ClusterTypeEnum
{
  CTE_CENTRAL = ClusterEnum::central,
  CTE_REMOTE1 = ClusterEnum::remote_1,
  CTE_REMOTE2  = ClusterEnum::remote_2,
  CTE_PROFILING = ClusterEnum::profiling,
  CTE_ALL_REMOTE = 0xFFFE,
  CTE_ALL = 0xFFFF
};

enum ServiceTypeEnum
{
  STE_FRONTEND = ServiceEnum::AdFrontend,
  STE_CHANNEL_SERVER = ServiceEnum::ChannelServer,
  STE_CHANNEL_CONTROLLER = ServiceEnum::ChannelController,
  STE_CHANNEL_SEARCH_SERVER = ServiceEnum::ChannelSearchServer,
  STE_CAMPAIGN_MANAGER = ServiceEnum::CampaignManager,
  STE_CAMPAIGN_SERVER = ServiceEnum::CampaignServer,
  STE_EXPRESSION_MATCHER = ServiceEnum::ExpressionMatcher,
  STE_REQUEST_INFO_MANAGER = ServiceEnum::RequestInfoManager,
  STE_USER_INFO_MANAGER = ServiceEnum::UserInfoManager,
  STE_USER_INFO_MANAGER_CONTROLLER = ServiceEnum::UserInfoManagerController,
  STE_USER_INFO_EXCHANGER = ServiceEnum::UserInfoExchanger,
};

class GlobalConfig
{
  typedef GeneralParamsType::Cluster_const_iterator ClusterIterator;
  typedef xsd::AdServer::Configuration::ClusterType::Service_const_iterator ServiceIterator;

  public:

  DECLARE_EXCEPTION(ConfigError, eh::DescriptiveException);

  /**
   * @struct Service
   *
   * @brief Service description.
   */
  struct Service
  {
    std::string address;  // service address
    bool remote;          // remote sign

    /**
     * @brief Constructor.
     * @param service address.
     * @param remote sign.
     */
    Service(
      const std::string& address_,
      bool remote_);
  };

  typedef std::vector<Service> ServiceList;
  typedef std::map<ServiceTypeEnum, ServiceList> ServiceDictionary;
  typedef std::map<ClusterTypeEnum, ServiceDictionary> ClusterDictionary;
  typedef std::unique_ptr<class xsd::AdServer::Configuration::GeneralParamsType> ParamsPtr;


  /**
   * @brief Constructor.
   * @param raw global config.
   */
  GlobalConfig(const char* config_path);

  /**
   * @brief Get service.
   * @param cluster.
   * @param service.
   * @param index in cluster.
   * @return service address and remote sign.
   */
  const Service& get_service(
    ClusterTypeEnum cluster,
    ServiceTypeEnum service,
    unsigned long index = 0) const
    throw (eh::Exception);

  /**
   * @brief Get services.
   * @param cluster.
   * @param service.
   * @return services list.
   */
  const ServiceList& get_services(
    ClusterTypeEnum cluster,
    ServiceTypeEnum service) const
    throw (eh::Exception);


  /**
   * @brief Check service.
   * @param cluster.
   * @param service.
   * @param index in cluster.
   * @return true if service exists in global config.
   */
  bool check_service(
   ClusterTypeEnum cluster,
   ServiceTypeEnum service,
   unsigned long index = 0) const;

  /**
   * @brief Get raw config params.
   * @return raw config params.
   */
  Params get_params() const throw();

private:

  void initialize(const char* config_path);

private:
  ParamsPtr params_;
  ClusterDictionary cluster_services_;
  static const ServiceList EMPTY_;
};

/**
 * @class XsdParams
 *
 * @brief Wrapper for autotests config.
 */
class XsdParams
{
public:

  /**
   * @brief Constructor.
   * @param test global config.
   * @param test local config.
   */
  XsdParams(
    const GlobalConfig& config,
    Locals new_locals)
    throw();

  /**
   * @brief Get raw global config params.
   * @return raw global config object.
   */
  Params get_global_params() const throw();

  /**
   * @brief Get local config.
   * @return local config object.
   */
  Locals get_local_params() const throw();

  /**
   * @brief Get global config.
   * @return global config object.
   */
  const GlobalConfig& get_config()  const throw();


private:
  const GlobalConfig& config_;
  Locals locals_;
};

#endif // __AUTOTESTS_COMMONS_AUTOTESTSXSD_HPP
