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

#ifndef CAMPAIGNSERVER_STATSOURCE_HPP
#define CAMPAIGNSERVER_STATSOURCE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  struct StatSource: public virtual ReferenceCounting::Interface
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    struct Stat: public ReferenceCounting::AtomicCopyImpl
    {
      // daily values must be cleaned if day switched
      struct CreativeStat
      {
        CreativeStat() throw();

        CreativeStat& operator+=(const CreativeStat& right) throw();

        long impressions;
        long clicks;
        long actions;
      };

      struct AmountStat
      {
        AmountStat() throw();

        AmountStat& operator+=(const AmountStat& right) throw();

        RevenueDecimal amount; // adv_amount + adv_cmp_amount
        RevenueDecimal comm_amount;
        RevenueDecimal daily_amount; // adv_daily_amount + adv_daily_cmp_amount
        RevenueDecimal daily_comm_amount;
      };

      struct CCGStat: public AmountStat
      {
        typedef std::map<unsigned long, CreativeStat> CreativeStatMap;

        // publisher account id => RevenueDecimal (advertiser amount)
        struct PublisherStat
        {
          PublisherStat() throw();
          PublisherStat& operator+=(const PublisherStat& right) throw();

          RevenueDecimal amount;
          RevenueDecimal daily_amount;
        };

        typedef std::map<unsigned long, PublisherStat> PublisherStatMap;

        struct TagHourStat
        {
          TagHourStat() throw();

          TagHourStat(const RevenueDecimal& isp_pub_amount_val,
            const RevenueDecimal& adv_amount_val,
            const RevenueDecimal& adv_comm_amount_val)
            throw();

          TagHourStat& operator+=(const TagHourStat& right) throw();

          RevenueDecimal isp_pub_amount;
          RevenueDecimal adv_amount;
          RevenueDecimal adv_comm_amount;
        };

        struct TagStat
        {
          TagStat& operator+=(const TagStat& right) throw();

          TagHourStat prev_hour_stat;
          TagHourStat current_hour_stat;
        };

        typedef std::map<unsigned long, TagStat> TagStatMap;

        struct CtrResetStat
        {
          CtrResetStat() throw();

          CtrResetStat& operator+=(const CtrResetStat& right) throw();

          unsigned long impressions;
        };

        typedef std::map<unsigned long, CtrResetStat> CtrResetStatMap;

        CCGStat() throw();

        CCGStat& operator+=(const CCGStat& right) throw();

        long impressions;
        long clicks;
        long actions;

        CreativeStatMap creatives;
        // collect only for max_pub_share campaigns, for minimize memory usage
        PublisherStatMap publisher_amounts;

        RevenueDecimal prev_hour_amount;
        RevenueDecimal prev_hour_comm_amount;
        RevenueDecimal cur_hour_amount;
        RevenueDecimal cur_hour_comm_amount;
        TagStatMap tag_stats;
        CtrResetStatMap ctr_reset_stats;
      };

      // ccg_id => CCGStat
      typedef std::map<unsigned long, CCGStat> CCGStatMap;

      struct CampaignStat: public AmountStat
      {
        CCGStatMap ccgs;

        CampaignStat&
        operator+=(const CampaignStat& right) throw();

        void print(std::ostream& out, const char* offset) const throw();
      };

      typedef std::map<unsigned long, CampaignStat> CampaignStatMap;

      struct Amount
      {
        Amount() throw();
        Amount& operator+=(const Amount& right) throw();

        RevenueDecimal amount;
        RevenueDecimal comm_amount;
      };

      // check_time must be equal (hours)
      void add(const Stat& stat) throw();

      void print(std::ostream& out, const char* offset) const throw();

      typedef std::map<unsigned long, Amount> AccountAmountMap;

      Generics::Time timestamp;
      Generics::Time check_time;
      CampaignStatMap campaign_stats;
      AccountAmountMap account_amounts;

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
      bool& full_synch_required,
      const Generics::Time& now)
      throw(Exception) = 0;

  protected:
    virtual
    ~StatSource() throw ()
    {}
  };

  typedef ReferenceCounting::QualPtr<StatSource>
    StatSource_var;

  typedef ReferenceCounting::FixedPtr<StatSource>
    FStatSource_var;
}
}

namespace AdServer
{
namespace CampaignSvcs
{
  template<typename CollectionType>
  void sum_collections(
    CollectionType& target_coll,
    const CollectionType& source_coll)
  {
    for(typename CollectionType::const_iterator sit =
          source_coll.begin();
        sit != source_coll.end(); ++sit)
    {
      typename CollectionType::iterator tit = target_coll.find(sit->first);
      if(tit == target_coll.end())
      {
        target_coll.insert(*sit);
      }
      else
      {
        tit->second += sit->second;
      }
    }
  }

  // StatSource::Stat::CreativeStat
  inline
  StatSource::Stat::CreativeStat::CreativeStat() throw()
    : impressions(0),
      clicks(0),
      actions(0)
  {}

  inline
  StatSource::Stat::CreativeStat&
  StatSource::Stat::CreativeStat::operator+=(
    const CreativeStat& right) throw()
  {
    impressions += right.impressions;
    clicks += right.clicks;
    actions += right.actions;
    return *this;
  }

  // StatSource::Stat::CCGStat::PublisherStat
  inline
  StatSource::Stat::CCGStat::PublisherStat::PublisherStat()
    throw()
    : amount(RevenueDecimal::ZERO),
      daily_amount(RevenueDecimal::ZERO)
  {}

  inline
  StatSource::Stat::CCGStat::PublisherStat&
  StatSource::Stat::CCGStat::PublisherStat::operator+=(
    const PublisherStat& right) throw()
  {
    amount += right.amount;
    daily_amount += right.daily_amount;
    return *this;
  }

  // StatSource::Stat::AmountStat
  inline
  StatSource::Stat::AmountStat::AmountStat() throw()
    : amount(RevenueDecimal::ZERO),
      comm_amount(RevenueDecimal::ZERO),
      daily_amount(RevenueDecimal::ZERO),
      daily_comm_amount(RevenueDecimal::ZERO)
  {}

  inline
  StatSource::Stat::AmountStat&
  StatSource::Stat::AmountStat::operator+=(const AmountStat& right)
    throw()
  {
    daily_amount += right.daily_amount;
    daily_comm_amount += right.daily_comm_amount;
    amount += right.amount;
    comm_amount += right.comm_amount;
    return *this;
  }

  // StatSource::Stat::CCGStat::TagHourStat
  inline
  StatSource::Stat::CCGStat::
  TagHourStat::TagHourStat()
    throw()
    : isp_pub_amount(RevenueDecimal::ZERO),
      adv_amount(RevenueDecimal::ZERO),
      adv_comm_amount(RevenueDecimal::ZERO)
  {}

  inline
  StatSource::Stat::CCGStat::
  TagHourStat::TagHourStat(
    const RevenueDecimal& isp_pub_amount_val,
    const RevenueDecimal& adv_amount_val,
    const RevenueDecimal& adv_comm_amount_val)
    throw()
    : isp_pub_amount(isp_pub_amount_val),
      adv_amount(adv_amount_val),
      adv_comm_amount(adv_comm_amount_val)
  {}

  inline
  StatSource::Stat::CCGStat::TagHourStat&
  StatSource::Stat::CCGStat::TagHourStat::operator+=(
    const TagHourStat& right) throw()
  {
    isp_pub_amount += right.isp_pub_amount;
    adv_amount += right.adv_amount;
    adv_comm_amount += right.adv_comm_amount;
    return *this;
  }

  // StatSource::Stat::CCGStat::TagStat
  inline
  StatSource::Stat::CCGStat::TagStat&
  StatSource::Stat::CCGStat::TagStat::operator+=(
    const TagStat& stat) throw()
  {
    prev_hour_stat += stat.prev_hour_stat;
    current_hour_stat += stat.current_hour_stat;
    return *this;
  }

  // StatSource::Stat::CCGStat
  inline
  StatSource::Stat::CCGStat::CtrResetStat::CtrResetStat()
    throw()
    : impressions(0)
  {}

  inline
  StatSource::Stat::CCGStat::CtrResetStat&
  StatSource::Stat::CCGStat::CtrResetStat::operator+=(
    const CtrResetStat& stat) throw()
  {
    impressions += stat.impressions;
    return *this;
  }

  // StatSource::Stat::CCGStat
  inline
  StatSource::Stat::CCGStat::CCGStat() throw()
    : impressions(0),
      clicks(0),
      actions(0),
      prev_hour_amount(RevenueDecimal::ZERO),
      prev_hour_comm_amount(RevenueDecimal::ZERO),
      cur_hour_amount(RevenueDecimal::ZERO),
      cur_hour_comm_amount(RevenueDecimal::ZERO)
  {}

  inline
  StatSource::Stat::CCGStat&
  StatSource::Stat::CCGStat::operator+=(
    const CCGStat& right)
    throw()
  {
    this->AmountStat::operator+=(right);

    impressions += right.impressions;
    clicks += right.clicks;
    actions += right.actions;

    sum_collections(creatives, right.creatives);

    prev_hour_amount += right.prev_hour_amount;
    prev_hour_comm_amount += right.prev_hour_comm_amount;
    cur_hour_amount += right.cur_hour_amount;
    cur_hour_comm_amount += right.cur_hour_comm_amount;
    sum_collections(publisher_amounts, right.publisher_amounts);
    sum_collections(tag_stats, right.tag_stats);
    sum_collections(ctr_reset_stats, right.ctr_reset_stats);

    return *this;
  }

  // StatSource::Stat::Amount
  inline
  StatSource::Stat::Amount::Amount() throw()
    : amount(RevenueDecimal::ZERO),
      comm_amount(RevenueDecimal::ZERO)
  {}

  inline
  StatSource::Stat::Amount&
  StatSource::Stat::Amount::operator+=(const Amount& right) throw()
  {
    amount += right.amount;
    comm_amount += right.comm_amount;
    return *this;
  }

  // StatSource::Stat::CampaignStat
  inline
  StatSource::Stat::CampaignStat&
  StatSource::Stat::CampaignStat::operator+=(
    const CampaignStat& right)
    throw()
  {
    AmountStat::operator+=(right);
    sum_collections(ccgs, right.ccgs);

    return *this;
  }

  inline
  void
  StatSource::Stat::CampaignStat::print(
    std::ostream& out, const char* offset) const throw()
  {
    out << offset <<
      "am = " << amount <<
      ", c_am = " << comm_amount <<
      ", d_am = " << daily_amount <<
      ", dc_am = " << daily_comm_amount << std::endl;

    out << offset << "ccgs: " << std::endl;

    for(CCGStatMap::const_iterator cs_it = ccgs.begin();
        cs_it != ccgs.end(); ++cs_it)
    {
      out << offset << cs_it->first <<
        ": imps = " << cs_it->second.impressions <<
        ", clicks = " << cs_it->second.clicks <<
        ", actions = " << cs_it->second.actions <<
        ", am = " << cs_it->second.amount <<
        ", c_am = " << cs_it->second.comm_amount <<
        ", d_am = " << cs_it->second.daily_amount <<
        ", dc_am = " << cs_it->second.daily_comm_amount <<
        ", ph_am = " << cs_it->second.prev_hour_amount <<
        ", phc_am = " << cs_it->second.prev_hour_comm_amount <<
        ", ch_am = " << cs_it->second.cur_hour_amount <<
        ", chc_am = " << cs_it->second.cur_hour_comm_amount << std::endl <<
        offset << "    creatives:" << std::endl;
      for(CCGStat::CreativeStatMap::const_iterator cr_it =
            cs_it->second.creatives.begin();
          cr_it != cs_it->second.creatives.end(); ++cr_it)
      {
        out << offset << "      " << cr_it->first <<
          ": imps = " << cr_it->second.impressions <<
          ", clicks = " << cr_it->second.clicks <<
          ", actions = " << cr_it->second.actions << std::endl;
      }
      out << offset << "    publisher_amounts:" << std::endl;
      for(CCGStat::PublisherStatMap::const_iterator p_it =
            cs_it->second.publisher_amounts.begin();
          p_it != cs_it->second.publisher_amounts.end(); ++p_it)
      {
        out << offset << "      " << p_it->first <<
          ": amount = " << p_it->second.amount <<
          ", daily_amount = " << p_it->second.daily_amount <<
          std::endl;
      }
      out << offset << "    tag_stats:" << std::endl;
      for(CCGStat::TagStatMap::const_iterator t_it =
            cs_it->second.tag_stats.begin();
          t_it != cs_it->second.tag_stats.end(); ++t_it)
      {
        out << offset << "      " << t_it->first <<
          ": ( ip_am = " << t_it->second.current_hour_stat.isp_pub_amount <<
          ", am = " << t_it->second.current_hour_stat.adv_amount <<
          ", c_am = " << t_it->second.current_hour_stat.adv_comm_amount <<
          " ) ( ip_am = " << t_it->second.prev_hour_stat.isp_pub_amount <<
          ", am = " << t_it->second.prev_hour_stat.adv_amount <<
          ", c_am = " << t_it->second.prev_hour_stat.adv_comm_amount <<
          " )" << std::endl;
      }
    }
  }

  // StatSource::Stat
  inline
  void
  StatSource::Stat::add(const Stat& stat) throw()
  {
    {
      Generics::ExtendedTime left_check_time =
        check_time.get_gm_time();
      Generics::ExtendedTime right_check_time =
        stat.check_time.get_gm_time();

      assert(Generics::ExtendedTime(
        right_check_time.tm_year + 1900,
        right_check_time.tm_mon + 1,
        right_check_time.tm_mday,
        right_check_time.tm_hour,
        0,
        0,
        0) == Generics::ExtendedTime(
          left_check_time.tm_year + 1900,
          left_check_time.tm_mon + 1,
          left_check_time.tm_mday,
          left_check_time.tm_hour,
          0,
          0,
          0));
    }

    timestamp = stat.timestamp;
    sum_collections(account_amounts, stat.account_amounts);
    sum_collections(campaign_stats, stat.campaign_stats);
  }

  inline
  void
  StatSource::Stat::print(std::ostream& out, const char* offset) const
    throw()
  {
    try
    {
      out << offset << "timestamp: " << timestamp.get_gm_time() << std::endl <<
        offset << "check_time: " << check_time.get_gm_time() << std::endl <<
        offset << "account_amounts: " << std::endl;
    }
    catch(const eh::Exception& e)
    {
      out << offset << "timestamp: " << timestamp.get_gm_time() << std::endl <<
        offset << "check_time: " << check_time.get_gm_time() << std::endl <<
        offset << "account_amounts: " << std::endl;
    }
    for(AccountAmountMap::const_iterator acc_it = account_amounts.begin();
        acc_it != account_amounts.end(); ++acc_it)
    {
      out << offset << "  " << acc_it->first <<
        ": amount = " << acc_it->second.amount <<
        ", comm_amount = " << acc_it->second.comm_amount << std::endl;
    }
    out << offset << "campaign_stats: " << std::endl;
    for(CampaignStatMap::const_iterator cmp_it = campaign_stats.begin();
        cmp_it != campaign_stats.end(); ++cmp_it)
    {
      out << offset << "  " << cmp_it->first << ": ";
      cmp_it->second.print(out, (std::string(offset) + "  ").c_str());
    }
  }
}
}

#endif /*CAMPAIGNSERVER_STATSOURCE_HPP*/
