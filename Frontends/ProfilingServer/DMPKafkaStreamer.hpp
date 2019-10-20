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

#include <array>

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include <Frontends/ProfilingServer/DMPProfilingInfo.hpp>
#include <Commons/Kafka/KafkaProducer.hpp>
#include <xsd/Frontends/ProfilingServerConfig.hpp>
#include <Commons/ExternalUserIdUtils.hpp>
#include <Commons/UserInfoManip.hpp>

namespace AdServer
{
  namespace Profiling
  {
    class DMPKafkaStreamer: public ReferenceCounting::AtomicImpl
    {
    public:

      DECLARE_EXCEPTION(ProducerNotFound, eh::DescriptiveException);
      
      enum ProducerType
      {
        PT_UID_GENERATOR,
        PT_CLICK_STREAM,
        PT_GEO
      };
      
      typedef ::xsd::AdServer::Configuration::KafkaTopic
        KafkaTopicConfig;
      typedef ::xsd::AdServer::Configuration::KafkaDMPProfilingStorageType
        KafkaStorageConfig;
      typedef ::xsd::cxx::tree::optional<KafkaTopicConfig>
        KafkaTopicConfig_optional;
      
      typedef const KafkaTopicConfig_optional& (
        KafkaStorageConfig::* GetTopicConfigFunc)() const;

      struct KafkaProducerDescriptor
      {
        ProducerType producer_type;
        GetTopicConfigFunc topic_func;
      };

      static const KafkaProducerDescriptor PRODUCER_DESCRIPTORS[];

      
      typedef std::map<ProducerType, Commons::Kafka::Producer_var> ProducerTable;

      void register_producers(
        Logging::Logger* logger,
        Generics::ActiveObjectCallback* callback,
        Generics::CompositeActiveObject* owner,
        const KafkaStorageConfig& config);

      void 
      write_dmp_profiling_info(
        const DMPProfilingInfoReader& dmp_profiling_info,
        const AdServer::Commons::ExternalUserIdArray& bind_user_ids,
        const AdServer::Commons::UserId& user_id,
        const Generics::Time& now);

      unsigned long
      sent_messages() const;

      unsigned long
      sent_bytes() const;

      unsigned long
      errors() const;

    protected:
      virtual
      ~DMPKafkaStreamer() throw () = default;

    private:
      ProducerTable producers_;
    };

    typedef ReferenceCounting::SmartPtr<DMPKafkaStreamer>
    DMPKafkaStreamer_var;
  }
}
    
        
