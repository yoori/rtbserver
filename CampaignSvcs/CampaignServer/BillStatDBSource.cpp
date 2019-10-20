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

#include <CampaignSvcs/CampaignCommons/CampaignCommons.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>

#include "ExecutionTimeTracer.hpp"
#include "BillStatDBSource.hpp"

namespace Aspect
{
  const char BILL_STAT_DB_SOURCE[] = "BillStatDBSource";
}

namespace AdServer
{
namespace CampaignSvcs
{
  namespace
  {
    typedef std::map<unsigned long, RevenueDecimal> TotalAmountMap;

    template<typename AmountMapType>
    void
    add_day_amount(
      AmountMapType& amounts,
      const Generics::Time& day,
      unsigned long object_id,
      const RevenueDecimal& amount)
      throw()
    {
      auto& obj = amounts[object_id];
      auto ins = obj.day_amounts.insert(std::make_pair(day, amount));
      if(!ins.second)
      {
        ins.first->second += amount;
      }
    }

    void
    add_total_amount(
      TotalAmountMap& amounts,
      unsigned long object_id,
      const RevenueDecimal& total_amount)
      throw()
    {
      auto ins = amounts.insert(std::make_pair(object_id, total_amount));
      if(!ins.second)
      {
        ins.first->second += total_amount;
      }
    }

    template<typename AmountMapType>
    void
    set_prev_day_amount(
      AmountMapType& amounts,
      TotalAmountMap& total_amounts,
      const Generics::Time& before_date)
      throw()
    {
      for(auto it = total_amounts.begin(); it != total_amounts.end(); ++it)
      {
        BillStatSource::Stat::AmountDistribution& res = amounts[it->first];
        RevenueDecimal sum_day_amount = RevenueDecimal::ZERO;
        for(auto day_it = res.day_amounts.begin();
          day_it != res.day_amounts.end(); ++day_it)
        {
          sum_day_amount += day_it->second;
        }
        res.prev_days_amount = it->second - sum_day_amount;
        res.prev_day = before_date;
      }
    }
  }

  BillStatDBSource::BillStatDBSource(
    Logging::Logger* logger,
    Commons::Postgres::Environment* pg_env)
    throw(Exception)
  try
    : logger_(ReferenceCounting::add_ref(logger))
  {
    pg_pool_ = pg_env->create_connection_pool();
  }
  catch(const eh::Exception& ex)
  {
    Stream::Error ostr;
    ostr << "BillStatDBSource::BillStatDBSource(): caught eh::Exception: " <<
      ex.what();
    throw Exception(ostr);
  }

  BillStatDBSource::Stat_var
  BillStatDBSource::update(
    Stat* /*stat*/,
    const Generics::Time& now)
    throw(Exception)
  {
    Stat_var new_stat;

    try
    {
      new_stat = query_db_stats_(now);
    }
    catch(const eh::Exception& ex)
    {
      logger_->sstream(
        Logging::Logger::ERROR,
        Aspect::BILL_STAT_DB_SOURCE,
        "ADS-DB-6000") << __func__ <<
        ": can't receive stats from DB: " << ex.what();
    }

    return new_stat;
  }

  BillStatDBSource::Stat_var
  BillStatDBSource::query_db_stats_(const Generics::Time& now)
    throw(Exception)
  {
    static const char* FUN = "BillStatDBSource::query_db_stats_()";

    ExecutionTimeTracer db_stats_timer(
      FUN,
      Aspect::BILL_STAT_DB_SOURCE,
      logger_);

    Commons::Postgres::Connection_var connection;

    try
    {
      connection = pg_pool_->get_connection();
    }
    catch (const Commons::Postgres::NotActive&)
    {
      return Stat_var();
    }
    catch (const Commons::Postgres::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": Commons::Postgres::Exception: " << e.what();
      throw Exception(ostr);
    }

    const Generics::Time now_date = now.get_gm_time().get_date();
    const Generics::Time trunc_date = now_date - Generics::Time::ONE_DAY * 60;

    Stat_var result(new Stat());
    result->timestamp = now;

    try
    {
      ExecutionTimeTracer db_stats_timer(FUN,
        Aspect::BILL_STAT_DB_SOURCE,
        logger_,
        "amount stats");

      enum
      {
        POS_ADV_DATE = 1,
        POS_ACCOUNT_ID,
        POS_CAMPAIGN_ID,
        POS_CCG_ID,
        POS_ADV_AMOUNT,
        POS_ADV_COMM_AMOUNT,
        POS_ACCOUNT_FLAGS
      };

      Commons::Postgres::Statement_var stmt = new Commons::Postgres::Statement(
        "SELECT "
          "adv_sdate,"
          "coalesce(case when not (at.flags & x'2'::int)::bool then "
            "ag.account_id end, ac.account_id) adv_account_id,"
          "s.campaign_id,"
          "s.ccg_id,"
          "s.adv_amount adv_amount, "
          "s.adv_comm_amount adv_comm_amount, "
          "at.flags flags "
        "FROM ("
          "SELECT sdate adv_sdate,"
            "campaign_id,"
            "ccg_id,"
            "SUM(adv_amount) adv_amount, "
            "SUM(adv_comm_amount) adv_comm_amount "
          "FROM adserver.snapshot_daily_cmp d "
          "GROUP BY sdate, campaign_id, ccg_id "
          "UNION ALL "
          "SELECT NULL adv_sdate,"
            "campaign_id,"
            "ccg_id,"
            "SUM(adv_amount) adv_amount, "
            "SUM(adv_comm_amount) adv_comm_amount "
          "FROM adserver.snapshot_total s "
          "GROUP BY campaign_id, ccg_id "
          ") s "
          "JOIN Campaign ON Campaign.campaign_id = s.campaign_id "
          "JOIN Account ac ON ac.account_id = campaign.account_id "
          "LEFT JOIN Account ag ON ag.account_id = ac.agency_account_id "
          "JOIN AccountType at on at.account_type_id = "
            "coalesce(ag.account_type_id, ac.account_type_id) "
        "WHERE (s.adv_amount <> 0 OR s.adv_comm_amount <> 0) AND "
        "Campaign.status <> 'D' AND "
        "ac.status <> 'D' AND "
        "COALESCE(ag.status, 'N') <> 'D' "
        "ORDER BY adv_sdate ASC NULLS LAST");

      TotalAmountMap account_total_amounts;
      TotalAmountMap campaign_total_amounts;
      TotalAmountMap ccg_total_amounts;

      Commons::Postgres::ResultSet_var rs = connection->execute_statement(stmt);

      while(rs->next())
      {
        const RevenueDecimal amount = rs->get_decimal<RevenueDecimal>(POS_ADV_AMOUNT);
        const RevenueDecimal comm_amount = rs->get_decimal<RevenueDecimal>(POS_ADV_COMM_AMOUNT);
        const unsigned long account_flags = rs->get_number<unsigned long>(POS_ACCOUNT_FLAGS);

        const RevenueDecimal account_amount =
          (account_flags & AccountTypeFlags::INVOICE_COMMISION ? amount + comm_amount : amount);
        const RevenueDecimal ccg_amount =
          (account_flags & AccountTypeFlags::GROSS ? amount + comm_amount : amount);

        if(amount != RevenueDecimal::ZERO ||
           comm_amount != RevenueDecimal::ZERO)
        {
          Generics::Time day = !rs->is_null(POS_ADV_DATE) ?
            rs->get_date(POS_ADV_DATE) :
            Generics::Time::ZERO;

          if(day > trunc_date)
          {
            add_day_amount(
              result->accounts,
              day,
              rs->get_number<unsigned long>(POS_ACCOUNT_ID),
              account_amount);

            add_day_amount(
              result->campaigns,
              day,
              rs->get_number<unsigned long>(POS_CAMPAIGN_ID),
              ccg_amount);

            add_day_amount(
              result->ccgs,
              day,
              rs->get_number<unsigned long>(POS_CCG_ID),
              ccg_amount);
          }
          else
          {
            add_total_amount(
              account_total_amounts,
              rs->get_number<unsigned long>(POS_ACCOUNT_ID),
              account_amount);

            add_total_amount(
              campaign_total_amounts,
              rs->get_number<unsigned long>(POS_CAMPAIGN_ID),
              ccg_amount);

            add_total_amount(
              ccg_total_amounts,
              rs->get_number<unsigned long>(POS_CCG_ID),
              ccg_amount);
          }
        }
      }

      // fill truncated by date amounts
      set_prev_day_amount(
        result->accounts,
        account_total_amounts,
        trunc_date);

      set_prev_day_amount(
        result->campaigns,
        campaign_total_amounts,
        trunc_date);

      set_prev_day_amount(
        result->ccgs,
        ccg_total_amounts,
        trunc_date);
    }
    catch(const eh::Exception& ex)
    {
      pg_pool_->bad_connection(connection);
      Stream::Error ostr;
      ostr << FUN << ": Can't query account stats, eh::Exception: " <<
        ex.what();
      throw Exception(ostr);
    }

    return result;
  }
}
}
