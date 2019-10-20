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

#include <Utils/CTRGenerator/CalculateParamsFilter.hpp>
#include <String/AsciiStringManip.hpp>
#include "FeatureContainer.hpp"

namespace AdServer
{
  namespace Predictor
  {
    void FeatureContainer_::init(
      const FeatureSeq& feature_seq) throw(eh::Exception)
    {
      // Configure model
      for(FeatureSeq::const_iterator feature_it = feature_seq.begin();
          feature_it != feature_seq.end(); ++feature_it)
      {
        AdServer::CampaignSvcs::CTRGenerator::Feature result_feature;
        
        for(FeatureType::BasicFeature_sequence::
              const_iterator basic_feature_it =
              feature_it->BasicFeature().begin();
            basic_feature_it != feature_it->BasicFeature().end();
            ++basic_feature_it)
        {
          AdServer::CampaignSvcs::CTR::BasicFeature basic_feature;
          if(!feature_name_resolver_.basic_feature_by_name(
               basic_feature, basic_feature_it->name()))
          {
            Stream::Error ostr;
            ostr << "Invalid basic feature name: '" << basic_feature_it->name() << "'";
            throw Exception(ostr);
          }
          
          result_feature.basic_features.insert(basic_feature);
        }
        
        features_.push_back(result_feature);
      }
    }

    unsigned long FeatureContainer_::resolve_features(
      const FeatureNames& feature_names,
      FeatureColumns& feature_columns) throw(eh::Exception)
    {
      unsigned long column_i = 0;
      unsigned long label_index = 0;
      for(auto it = feature_names.begin(); it != feature_names.end(); ++it, ++column_i)
      {
        if(*it == "label" || *it == "Label")
        {
          label_index = column_i;
        }
        else if(*it == "timestamp" || *it == "Timestamp")
        {
          feature_columns[
            AdServer::CampaignSvcs::CalculateParamsFiller::BF_TIMESTAMP] =
            column_i;
        }
        else if(*it == "link" || *it == "Link")
        {
          feature_columns[
            AdServer::CampaignSvcs::CalculateParamsFiller::BF_LINK] = column_i;
        }
        else if(!it->empty() && (*it)[0] != '#')
        {
          std::string name_lower(*it);
          String::AsciiStringManip::to_lower(name_lower);
          
          AdServer::CampaignSvcs::CTR::BasicFeature basic_feature;
          if(!feature_name_resolver_.basic_feature_by_name(basic_feature, name_lower))
          {
            Stream::Error ostr;
            ostr << "Invalid basic feature name: '" << *it << "'";
            throw Exception(ostr);            
          }
          feature_columns[basic_feature] = column_i;
        }
      }
      return label_index;
    }

    const AdServer::CampaignSvcs::CTRGenerator::FeatureList&
    FeatureContainer_::features() const
    {
      return features_;
    }
  }
}
