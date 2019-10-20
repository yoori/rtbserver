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

#ifndef CAMPAIGNSERVER_BILLSTATSOURCE_HPP
#define CAMPAIGNSERVER_BILLSTATSOURCE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  struct BillStatSource: public virtual ReferenceCounting::Interface
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    struct Stat: public ReferenceCounting::AtomicCopyImpl
    {
    public:
      typedef std::map<Generics::Time, RevenueDecimal>
        DayToAmountMap;

      struct AmountDistribution
      {
        AmountDistribution()
          : prev_days_amount(RevenueDecimal::ZERO),
            prev_day(Generics::Time::ZERO)
        {}

        void
        print(std::ostream& out, const char* offset) const
          throw();

        RevenueDecimal prev_days_amount;
        Generics::Time prev_day;
        DayToAmountMap day_amounts;
      };

      struct Account: public AmountDistribution
      {};

      typedef std::map<unsigned long, Account> AccountMap;

      struct Campaign: public AmountDistribution
      {};

      typedef std::map<unsigned long, Campaign> CampaignMap;

      struct CCG: public AmountDistribution
      {};

      typedef std::map<unsigned long, CCG> CCGMap;

    public:
      void
      print(std::ostream& out, const char* offset) const throw();

    public:
      Generics::Time timestamp;
      AccountMap accounts;
      CampaignMap campaigns;
      CCGMap ccgs;

    protected:
      virtual
      ~Stat() throw ()
      {}
    };

    typedef ReferenceCounting::QualPtr<Stat>
      Stat_var;

    typedef ReferenceCounting::ConstPtr<Stat>
      CStat_var;

    virtual Stat_var
    update(
      Stat* stat,
      const Generics::Time& now)
      throw(Exception) = 0;

  protected:
    virtual
    ~BillStatSource() throw ()
    {}
  };

  typedef ReferenceCounting::QualPtr<BillStatSource>
    BillStatSource_var;

  typedef ReferenceCounting::FixedPtr<BillStatSource>
    FBillStatSource_var;
}
}

#endif /*CAMPAIGNSERVER_BILLSTATSOURCE_HPP*/
