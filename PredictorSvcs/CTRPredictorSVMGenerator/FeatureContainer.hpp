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

#pragma once

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/Singleton.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <xsd/Predictor/CTRPredictorSVMGeneratorConfig.hpp>
#include <Utils/CTRGenerator/CTRGenerator.hpp>

namespace AdServer
{
  namespace Predictor
  {

    typedef std::map<unsigned long, unsigned long> FeatureColumns;
    typedef std::vector<std::string> FeatureNames;

    /**
     * Features container 
     */
    class FeatureContainer_:
          public ReferenceCounting::AtomicImpl
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      
    public:
      typedef xsd::AdServer::Configuration::SVMGeneratorConfigurationType SVMGeneratorConfig;
      typedef SVMGeneratorConfig::Model_type::Feature_sequence FeatureSeq;
      typedef SVMGeneratorConfig::Model_type::Feature_type FeatureType;
      
    public:
      
      /**
       * @brief Initialize contaner.
       *
       * @param configured feature sequence
       */
      void init(
        const FeatureSeq& feature_seq) throw(eh::Exception);
      
      /**
       * @brief Resolve features.
       *
       * @param feature names
       * @param resolved feature columns
       */
      unsigned long resolve_features(
        const FeatureNames& feature_names,
        FeatureColumns& feature_columns) throw(eh::Exception);

      /**
       * @brief Get feature list.
       *
       * @return feature list
       */
      const AdServer::CampaignSvcs::CTRGenerator::FeatureList&
      features() const;

    protected:

      /**
       * @brief Destructor.
       */
      virtual
      ~FeatureContainer_() throw () = default;

    private:
      AdServer::CampaignSvcs::CTRGenerator::FeatureList features_;
      AdServer::CampaignSvcs::CTR::FeatureNameResolver feature_name_resolver_;
    };

    typedef ReferenceCounting::QualPtr<FeatureContainer_> FeatureContainer_var;  
    typedef Generics::Singleton<FeatureContainer_, FeatureContainer_var>
      FeatureContainer;
  }
}

