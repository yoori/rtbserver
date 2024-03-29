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


#include "ReportCommons.hpp"

//Utils

void print_line(std::ostream& out, unsigned short line_lendth)
{
  char stored_fill_char = out.fill();
  out.fill('-');
  out.width(line_lendth);
  out << "" << std::endl;
  out.fill(stored_fill_char);
}

void print_time(std::ostream& out, const Generics::Time& time)
{
  std::ostringstream time_str;
  unsigned long msec = time.tv_usec / 1000;
  time_str << time.tv_sec << ".";
  time_str.fill('0');
  time_str.width(3);
  time_str << msec;
  out << time_str.str();
 }

void print_header(std::ostream& out,
                  const char* header,
                  unsigned short width)
{
  out.width((width + strlen(header))/2 );
  out << std::right << header << std::endl << std::endl; 
}

// Class DumpInterface

std::ostream& DumpInterface::dump(std::ostream& out) const
{
  dump_header(out);
  dump_body(out);
  dump_footer(out);
  return out;
}

std::ostream& DumpInterface::dump_header(std::ostream& out) const
{
  return out;
}

std::ostream& DumpInterface::dump_body(std::ostream& out) const
{
  return out;
}
std::ostream& DumpInterface::dump_footer(std::ostream& out) const
{
  return out;
}


// Class DumpCounter

DumpCounter::DumpCounter(const Counter& counter) :
  counter_(counter)
{ }

DumpCounter::~DumpCounter() throw()
{ }

std::ostream& DumpCounter::dump_body(std::ostream& out) const
{
  out << counter_.diff() << "/" << counter_.get();
  return out;
}

// Class DumpPercentageCounter

DumpPercentageCounter::DumpPercentageCounter(const Counter& counter) :
  counter_(counter)
{ }

DumpPercentageCounter::~DumpPercentageCounter() throw()
{ }

std::ostream& DumpPercentageCounter::dump_body(std::ostream& out) const
{
  std::ostringstream str;
  str.precision(2);
  str << counter_.get() << "(" << std::fixed << counter_.percentage() << "%)";
  out << str.str();
  return out;
}


// Class DumpRangeStats

DumpRangeStats::DumpRangeStats(const RangeStats& stats,
                               bool short_form) :
  stats_(stats),
  short_form_(short_form)
{ }

DumpRangeStats::~DumpRangeStats() throw()
{ }

std::ostream& DumpRangeStats::dump_body(std::ostream& out) const
{
  if (!short_form_)
    out << stats_.get() << "/";
  out << stats_.min() << "/"
      << stats_.max() << "/" << stats_.average().get();
  return out;
}

// Class DumpPerformanceStats

DumpPerformanceStats::DumpPerformanceStats(const PerformanceStatisticsBase& stats,
                                           const char* header) :
  stats_(stats),
  header_(header)
{ }

DumpPerformanceStats::~DumpPerformanceStats() throw()
{ }

std::ostream& DumpPerformanceStats::dump_header(std::ostream& out) const
{
  print_line(out, LINE_LENGTH);
  out << std::endl;
  out.fill (' ');
  out.flags(std::ios::right);
  print_header(out, header_.c_str(), LINE_LENGTH);
  out.width (INDENTION);
  out << "Name";
  out.width (INDENTION);
  out << "Min";
  out.width (INDENTION);
  out << "Max";
  out.width (INDENTION);
  out << "Average";
  out << std::endl;
  print_line(out, LINE_LENGTH);  
  return out;
}

std::ostream& DumpPerformanceStats::dump_body(std::ostream& out) const
{
    out.flags(std::ios::right);
    out.fill (' ');
    for (PerformanceStatisticsBase::ItemList::const_iterator
             it = stats_.items().begin();
         it != stats_.items().end(); ++it)
    {
      out.width (INDENTION);
      out << it->first;
      out.width (INDENTION);
      print_time(out, it->second->min());
      out.width (INDENTION);
      print_time(out, it->second->max());
      out.width (INDENTION);
      print_time(out, it->second->average());
      out << std::endl;
    }
    print_line(out, LINE_LENGTH);
    out << std::endl << std::endl;
    return out;
}

