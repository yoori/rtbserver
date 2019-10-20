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

#ifndef USERBINDSERVER_SIMPLEFIXEDALLOCATOR_HPP_
#define USERBINDSERVER_SIMPLEFIXEDALLOCATOR_HPP_

#include <memory>
#include <vector>
#include <list>

#include <Sync/SyncPolicy.hpp>
#include <Generics/Singleton.hpp>

namespace AdServer
{
namespace UserInfoSvcs
{
  struct SimpleFixedAllocator
  {
  public:
    SimpleFixedAllocator(unsigned long alloc_size) throw();

    void*
    alloc() throw();

    void
    dealloc(void*) throw();

  protected:
    typedef std::vector<unsigned char> Buf;
    typedef std::list<Buf> BufList;

    typedef Sync::Policy::PosixSpinThread SyncPolicy;

  protected:
    void
    alloc_new_buf_(Buf& new_buf) throw();

  protected:
    const unsigned long alloc_size_;
    const unsigned long buf_size_;
    const unsigned long last_element_offset_in_buf_;

    mutable SyncPolicy::Mutex lock_;
    BufList buffers_;
    void* first_free_;
  };

  struct SimpleDistribAllocator
  {
  public:
    SimpleDistribAllocator(
      unsigned long min_alloc_size = sizeof(void*),
      unsigned long max_alloc_size = 257) throw();

    void*
    alloc(unsigned long size) throw();

    void
    dealloc(void* buf, unsigned long size) throw();

  protected:
    typedef std::vector<std::unique_ptr<SimpleFixedAllocator> > AllocArray;

  protected:
    const unsigned long min_alloc_size_;
    const unsigned long max_alloc_size_;
    AllocArray allocators_;
  };
}
}

#include "SimpleFixedAllocator.tpp"

#endif /*USERBINDSERVER_SIMPLEFIXEDALLOCATOR_HPP_*/
