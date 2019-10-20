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

#ifndef CTRGENERATOR_CTRGENERATOR_HPP
#define CTRGENERATOR_CTRGENERATOR_HPP

#include <CampaignSvcs/CampaignManager/CTRProvider.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  class CTRGenerator
  {
  public:
    DECLARE_EXCEPTION(InvalidConfig, eh::DescriptiveException);

    struct Feature
    {
      std::string name;
      CTR::BasicFeatureSet basic_features;
    };

    typedef std::list<Feature> FeatureList;

    typedef std::set<uint32_t> IdSet;

    struct CalculateParams
    {
      CalculateParams();

      uint32_t publisher_id;
      uint32_t site_id;
      uint32_t tag_id;
      std::string etag;
      std::string domain;
      uint32_t referer_hash;
      uint32_t advertiser_id;
      uint32_t campaign_id;
      uint32_t ccg_id;
      uint32_t size_type_id;
      uint32_t size_id;
      unsigned char hour;
      unsigned char wd;
      uint32_t isp_id;
      uint32_t colo_id;
      uint32_t device_id;
      uint32_t creative_id;
      uint32_t cc_id;
      uint32_t campaign_freq;
      uint32_t campaign_freq_log;
      IdSet channels;
      IdSet geo_channels;
      IdSet content_categories;
      IdSet visual_categories;
      int32_t tag_visibility;
      int32_t tag_predicted_viewability;
    };

    struct Calculation
    {
      std::map<unsigned long, unsigned long> hashes;
    };

    typedef std::map<unsigned long, std::string>
      FeatureDictionary;

    class FeatureHashCalculator: public ReferenceCounting::AtomicImpl
    {
    public:
      virtual void
      eval(
        CTR::Murmur32v3Adapter& hash_adapter,
        CTRGenerator::Calculation& index_calculation,
        const CalculateParams& calc_params)
        throw() = 0;

      virtual void
      fill_dictionary(
        CTR::Murmur32v3Adapter& hash_adapter,
        FeatureDictionary& dictionary,
        const FeatureDictionary& global_dictionary,
        const CalculateParams& calc_params)
        throw() = 0;

    protected:
      virtual
      ~FeatureHashCalculator() throw () = default;
    };

    typedef ReferenceCounting::SmartPtr<FeatureHashCalculator>
      FeatureHashCalculator_var;

  public:
    CTRGenerator(const FeatureList& features, bool xgb_model);

    void
    calculate(
      Calculation& calculation,
      const CalculateParams& calc_params);

    void
    fill_dictionary(
      FeatureDictionary& global_dictionary,
      const CalculateParams& calc_params);

  protected:
    struct FeatureHolder
    {
      unsigned long hash_seed;
      FeatureHashCalculator_var feature_calculator;
    };

    typedef std::list<FeatureHolder> FeatureHolderList;

  protected:
    std::size_t
    eval_feature_hash_seed_(const Feature& feature)
      throw();

    FeatureHashCalculator_var
    create_final_feature_hash_calculator_(
      CTR::BasicFeature basic_feature);

    FeatureHashCalculator_var
    create_delegate_feature_hash_calculator_(
      CTR::BasicFeature basic_feature,
      FeatureHashCalculator* next_calculator);

  protected:
    FeatureHolderList feature_holders_;
    bool push_hour_;
    bool push_week_day_;
    bool push_campaign_freq_;
    bool push_campaign_freq_log_;
  };
}
}

#endif /*CTRGENERATOR_CTRGENERATOR_HPP*/
