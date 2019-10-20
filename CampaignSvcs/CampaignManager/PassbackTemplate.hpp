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

#ifndef CAMPAIGNMANAGER_PASSBACK_TEMPLATE_HPP
#define CAMPAIGNMANAGER_PASSBACK_TEMPLATE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Time.hpp>
#include <Sync/SyncPolicy.hpp>
#include <String/TextTemplate.hpp>
#include <Generics/GnuHashTable.hpp>

#include <Commons/LockMap.hpp>

#include "CreativeTemplateArgs.hpp"

namespace AdServer
{
  namespace CampaignSvcs
  {
    struct PassbackTemplate: public ReferenceCounting::AtomicImpl
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      DECLARE_EXCEPTION(FileNotExists, Exception);
      DECLARE_EXCEPTION(InvalidParams, Exception);
      DECLARE_EXCEPTION(InvalidTemplate, Exception);
      DECLARE_EXCEPTION(ImplementationException, Exception);

      virtual void
      instantiate(
        std::ostream& ostr,
        const TokenValueMap& tokens)
        throw(InvalidParams,
          InvalidTemplate,
          ImplementationException) = 0;

    protected:
      virtual ~PassbackTemplate() throw() {};
    };

    typedef ReferenceCounting::QualPtr<PassbackTemplate>
      PassbackTemplate_var;

    class PassbackTemplateMap
    {
    public:
      PassbackTemplate_var
      get(const Generics::StringHashAdapter& key)
        throw(PassbackTemplate::FileNotExists,
          PassbackTemplate::InvalidTemplate);

    protected:
      typedef Sync::Policy::PosixThread SyncPolicy;

      struct TemplateHolder: public ReferenceCounting::AtomicImpl
      {
        SyncPolicy::Mutex lock;
        Generics::Time last_update;
        PassbackTemplate_var passback_template;
      };

      typedef ReferenceCounting::SmartPtr<TemplateHolder> TemplateHolder_var;

      typedef AdServer::Commons::StrictLockMap<
        Generics::StringHashAdapter,
        Sync::Policy::PosixThread,
        Commons::Hash2Args>
        OpenLockMap;

      typedef Generics::GnuHashTable<
        Generics::StringHashAdapter,
        TemplateHolder_var>
        KeyMap;

      SyncPolicy::Mutex templates_lock_;
      KeyMap templates_;
      OpenLockMap open_locks_;
    };
  }
}

#endif /*CAMPAIGNMANAGER_PASSBACK_TEMPLATE_HPP*/
