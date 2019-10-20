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

/* $Id: KafkaProducer.hpp 185723 2019-05-13 12:00:57Z jurij_kuznecov $
* @file KafkaProducer.hpp
* @author Artem V. Nikitin (artem_nikitin@ocslab.com)
* Kafka producer implementation
*/
#pragma once

#include <librdkafka/rdkafkacpp.h>
#include <Commons/AtomicInt.hpp>
#include <Commons/DelegateActiveObject.hpp>
#include <Commons/Kafka/LimitedMTQueue.hpp>
#include <xsd/AdServerCommons/AdServerCommons.hpp>

namespace AdServer
{
  namespace Commons
  {
    namespace Kafka
    {

      /**
       * @class StatCounter
       * @brief Simple class to store counter.
       */
      class StatCounter
      {
      public:

        /**
         * @brief Constructor
         */
        StatCounter();

        /**
         * @brief Increment counter
         *
         * @param increment value.
         */
        StatCounter& operator+=(int val);
        

        /**
         * @brief Reduction of int
         */
        operator int() const;
        
        friend
        std::ostream&
        operator<<(
          std::ostream& os,
          const StatCounter& cnt);
        
      private:
        mutable Algs::AtomicUInt prev_;
        Algs::AtomicUInt current_;
      };

      /**
       * @brief Counter to stream
       *
       * @param stream
       * @param counter
       * @return stream
       */
      std::ostream&
      operator<<(
        std::ostream& os,
        const StatCounter& cnt);

      /**
       * @brief Producer
       * RdKafka::Producer wrapper
       */
      class Producer : public Commons::DelegateActiveObject
      {
        DECLARE_EXCEPTION(ProducerError, eh::DescriptiveException);

        typedef std::pair<std::string, std::string> ProducerPair;

        class ProducerHandler;

        /**
         * @brief EventCallback
         * RdKafka::EventCb wrapper
         */
        class EventCallback: public RdKafka::EventCb
        {
        public:
          /**
           * @brief Constructor
           * @param producer handler
           */
          EventCallback(ProducerHandler* handler)
            throw();

          /**
           * @brief Event callback
           * @param kafka event (see RdKafka::Event)
           */
          virtual void
          event_cb(RdKafka::Event& event);
        
        protected:
          ProducerHandler* handler_;
        };

        /**
         * @brief DeliveryReportCallback
         * RdKafka::DeliveryReportCb wrapper
         */
        class DeliveryReportCallback: public RdKafka::DeliveryReportCb
        {
        public:
          /**
           * @brief Constructor
           * @param producer handler
           */
          DeliveryReportCallback(ProducerHandler* handler)
            throw();

          /**
           * @brief Delivery callback
           * @param kafka message (see RdKafka::Message)
           */
          virtual
          void
          dr_cb(
            RdKafka::Message &message);
        
        protected:
          ProducerHandler* handler_;
        };

        /**
         * @brief PartitionCallback
         * RdKafka::PartitionerCb wrapper
         */
        class PartitionCallback: public RdKafka::PartitionerCb
        {
        public:
          /**
           * @brief Partition callback
           * @param kafka topic (see RdKafka::Topic)
           * @param message key
           * @param topic max partition number
           * @return partition number
           */
          virtual int32_t
          partitioner_cb (
            const RdKafka::Topic *topic,
            const std::string *key,
            int32_t partition_cnt,
            void *msg_opaque);
        };

        // 

        /**
         * @class StatsObject
         * @brief Statistic thread
         */
        class StatsObject : public Commons::DelegateActiveObject
        {
        public:
          /**
           * @brief Constructor
           * @param producer object
           * @param active object callback
           */
          StatsObject(
            Producer* owner,
            Generics::ActiveObjectCallback* callback);

          // Statistics
          StatCounter error_overflow;  // input message overflow events
          StatCounter error_exception; // kafka errors
          StatCounter reconnect;       // reconnects to broker
          StatCounter sent;            // sent (into rdkafka internal queue) messages
          StatCounter sent_bytes;      // sent (into rdkafka internal queue) bytes
          volatile sig_atomic_t disconnected; // disconnected flag

          /**
           * @brief Move to disconnected state
           */
          void consider_disconnect(); // 

          /**
           * @brief Move to connected state
           */
          void consider_connect();

        protected:
          
          /**
           * @brief Main work cycle
           */
          void
          work_() throw();

          /**
           * @brief Termination handler
           */
          virtual void
          terminate_() throw();

          /**
           * @brief Destructor
           */
          virtual
          ~StatsObject() throw() = default;

        private:
          Producer* owner_;
          typedef Sync::Policy::PosixThread SyncPolicy;
          Generics::Time last_stat_time_;
          SyncPolicy::Mutex lock_;
          Sync::Conditional condition_;
        };

        typedef ReferenceCounting::SmartPtr<StatsObject> StatsObject_var;

        /**
         * @class ProducerHandler
         * @brief Producer handler
         */
        class ProducerHandler
        { 
        public:

          /**
           * @brief Constructor
           * @param producer object
           */
          ProducerHandler(Producer* owner)
            throw(ProducerError);

          /**
           * @brief Produce message pair
           * @param message pair (key, data)
           */
          void
          produce(
            const ProducerPair& msg)
            throw(ProducerError);

          /**
           * @brief Process error
           * @param error context
           * @param error message
           * @param disconnected flag
           */
          void
          process_error(
            const char* context,
            const char* error,
            bool disconnected);
          
          /**
           * @brief Notify that everything looks OK on rdkafka side
           */
          void notify_ok();

          /**
           * @brief Polls the provided kafka handle for events
           * @param timeout_ms the maximum amount of time (in milliseconds)
           *        that the call will block waiting for events
           */
          int poll(
            int timeout_ms = 0);

          int
          flush(int timeout_ms = 0);

          /**
           * @brief Resend message (push into input queue)
           * @param topic name
           * @param key
           * @param data buffer
           * @param data size
           */
          void resend_message(
            const std::string& topic_name,
            const std::string* key,
            void* payload,
            size_t len);

        private:
          Producer* owner_;
          EventCallback event_callback_;
          DeliveryReportCallback delivery_callback_;
          PartitionCallback partition_callback_;
          std::unique_ptr<RdKafka::Conf> producer_conf_;
          std::unique_ptr<RdKafka::Producer> producer_;
          std::unique_ptr<RdKafka::Conf> topic_conf_;
          std::unique_ptr<RdKafka::Topic> topic_;
        };

        typedef ::xsd::AdServer::Configuration::KafkaTopic
           KafkaTopicConfig; 
        
      public:

        /**
         * @brief Constructor
         * @param logger
         * @param active object callback
         * @param kafka producer topic config
         */
        Producer(
          Logging::Logger* logger,
          Generics::ActiveObjectCallback* callback,
          const KafkaTopicConfig& config);

        /**
         * @brief Constructor
         * @param logger
         * @param active object callback
         * @param threads number
         * @param input queue size
         * @param brokers string, sample host1:9092,host2:9093
         * @param topic name
         */
        Producer(
           Logging::Logger* logger,
           Generics::ActiveObjectCallback* callback,
           unsigned long threads_number,
           unsigned long queue_size,
           const char* brokers,
           const char* topic_name);

        /**
         * @brief Push data
         * @param key
         * @param data
         */
        void push_data(
          const std::string& key,
          const std::string& data) throw();
        
        /**
         * @brief Activate objects (start threads)
         */
        virtual void
        activate_object()
          throw (Exception, eh::Exception);

        /**
         * @brief Get errors
         */
        unsigned long
        errors() const;

        /**
         * @brief Get sent messages
         */
        unsigned long
        sent() const;

        /**
         * @brief Get sent bytes
         */
        unsigned long
        sent_bytes() const;
        
      private:

        /**
         * @brief Produce message from input queue
         * @param producer handler
         */
        void
        produce_(
          ProducerHandler& handler);

        /**
         * @brief Process error
         * @param error context
         * @param error message
         */
        void
        process_error_(
          const char* context,
          const char* error);

      protected:
        
        /**
         * @brief Main working cycle.
         */
        void
        work_() throw();
        
        /**
         * @brief Termination.
         */
        virtual void
        terminate_() throw();

        
        /**
         * @brief Destructor
         */
        ~Producer() throw() = default;

      private:
        
        Logging::Logger_var logger_;
        std::string brokers_;
        const std::string topic_name_;
        typedef LimitedMTQueue<ProducerPair> ProducerQueue;
        ProducerQueue messages_;
        
        // Synchronization
        typedef Sync::Policy::PosixThread SyncPolicy;
        SyncPolicy::Mutex reconnect_lock_;
        Sync::Conditional reconnect_cond_;

        // Statistics
        StatsObject_var stats_;
      };

      typedef ReferenceCounting::SmartPtr<Producer> Producer_var;

    }
  }
}

