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


#include <assert.h>
#include <Stream/MemoryStream.hpp>
#include "Constraint.hpp"

// BaseConstraint class

BaseConstraint::BaseConstraint(const char* name_,
                       const char* description_) :
  name(name_),
  description(description_),
  error_("")
{}

BaseConstraint::~BaseConstraint() throw()
{}

const std::string& BaseConstraint::error() const
{
  return error_;
}

// ConstraintElementClass

ConstraintElement::ConstraintElement(const char* name_,
                                     const char* description_,
                                     const unsigned long sampling_size,
                                     const unsigned long threshold_value) : 
    BaseConstraint(name_, description_),
    sampling_size_(sampling_size),
    threshold_value_(threshold_value), // threshold value in percentage
    sampling_begin_idx_(0),
    sampling_end_idx_(0),
    current_denominator_(sampling_size),
    error_detected_(false)
{
  PercentagePair pair;
  pair.numerator   = 0;
  pair.denominator = 0;
  stats_.assign(sampling_size_, pair);
}

ConstraintElement::~ConstraintElement() throw()
{}


void ConstraintElement::push(unsigned long numerator,
                             unsigned long denominator) throw(InvalidSequence)
{
  if (!error_detected_)
    {
      if (numerator < stats_[sampling_end_idx_].numerator ||
          denominator < stats_[sampling_end_idx_].denominator)
        {
          Stream::Error ostr;
          ostr << "ConstraintElement: invalid sequence order (" <<
            numerator << ", " << denominator << ") after (" <<
            stats_[sampling_end_idx_].numerator << ", " <<
            stats_[sampling_end_idx_].denominator << ")";
          throw InvalidSequence(ostr);
        }
      
      // Resize sampling window
      unsigned long index = sampling_begin_idx_;
      while (denominator - stats_[index].denominator > sampling_size_ &&
             index !=  sampling_end_idx_)
        {
          index = _incr_index(index);
        }
      if ( index != sampling_begin_idx_ )
        {
          sampling_begin_idx_ = _dec_index(index);
        }
      
      current_denominator_ =
        (denominator - stats_[sampling_begin_idx_].denominator) > sampling_size_ ?
        denominator - stats_[sampling_begin_idx_].denominator : sampling_size_;
      
      // Add element
      sampling_end_idx_ = _incr_index(sampling_end_idx_);
      stats_[sampling_end_idx_].numerator   = numerator;
      stats_[sampling_end_idx_].denominator = denominator;
      
      detect_error();
    }
}

void ConstraintElement::detect_error()
{
  long current_numerator =
    stats_[sampling_end_idx_].numerator - stats_[sampling_begin_idx_].numerator;
  assert(current_numerator >= 0);  
  if (static_cast<unsigned long>(current_numerator) * 100 >= threshold_value_ * current_denominator_)
    {
      std::ostringstream ostr;
      ostr << "Constraint '" << name << "' (" << description <<
        ") failed, because number of events (" << current_numerator <<
        ") more than allowable limit (" << threshold_value_ * current_denominator_ / 100 <<
        ")";
      error_ = ostr.str();
      error_detected_ = true;
    }
}

bool ConstraintElement::check()
{
  return !error_detected_;
}

unsigned long ConstraintElement::_dec_index(unsigned long index)
{
  return (index + 1) % sampling_size_;
}
unsigned long ConstraintElement::_incr_index(unsigned long index)
{
  return index ? index-1 : sampling_size_ - 1;
}


// ConstraintsContainer class

ConstraintsContainer::ConstraintsContainer(const char* name_,
                                           const char* description_) :
  BaseConstraint(name_, description_)
{}

ConstraintsContainer::~ConstraintsContainer() throw()
{}

void ConstraintsContainer::register_constraint(Constraint_var& constraint)
{
  constraints_.push_back(constraint);
}

bool ConstraintsContainer::check()
{
  ConstraintsList::iterator it = constraints_.begin();
  for(; it != constraints_.end(); ++it)
    {
      if (!(*it)->check())
        {
          error_ = (*it)->error();
          return false;
        }
    }
  return true;
}
