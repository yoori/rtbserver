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

#include <cstdlib>
#include "LoadingProgressCallback.hpp"

namespace
{
  const unsigned int NUMBER_OF_CHUNKS_TO_REPORT = 10000;
}

namespace AdServer
{
namespace ProfilingCommons
{
  LoadingProgressCallback::LoadingProgressCallback(
    LoadingProgressCallbackBase* parent,
    double range) throw()
    : parent_(ReferenceCounting::add_ref(parent)),
      range_(range),
      current_value_(0.0),
      last_reported_chunk_(0)
  {
    if (abs(range_) < 0.000001)
    {
      range_ = 1.0;
    }
  }

  void LoadingProgressCallback::post_progress(double value) throw()
  {
    if (current_value_ > range_)
    {
      return;
    }

    current_value_ += value;
    unsigned int current_chunk = (current_value_ / range_) *
      NUMBER_OF_CHUNKS_TO_REPORT;
    if (current_chunk > last_reported_chunk_)
    {
      if (current_chunk > NUMBER_OF_CHUNKS_TO_REPORT)
      {
        current_chunk = NUMBER_OF_CHUNKS_TO_REPORT;
      }

      double progress = (double)(current_chunk - last_reported_chunk_) /
        NUMBER_OF_CHUNKS_TO_REPORT;

      parent_->post_progress(progress);
      last_reported_chunk_ = current_chunk;
    }
  }

  void LoadingProgressCallback::loading_is_finished() throw()
  {
    if (last_reported_chunk_ >= NUMBER_OF_CHUNKS_TO_REPORT)
    {
      return;
    }

    double progress = (double)(NUMBER_OF_CHUNKS_TO_REPORT - last_reported_chunk_) /
      NUMBER_OF_CHUNKS_TO_REPORT;
    parent_->post_progress(progress);
    last_reported_chunk_ = NUMBER_OF_CHUNKS_TO_REPORT;
  }
}
}
