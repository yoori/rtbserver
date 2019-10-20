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

#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <Generics/Uncopyable.hpp>
#include <pthread.h>

class Barrier: private Generics::Uncopyable
{
public:
  Barrier(unsigned int count) throw();

  ~Barrier() throw();

  bool wait() throw();

private:
  pthread_barrier_t barrier_;
};

inline
Barrier::Barrier(unsigned int count) throw()
{
  pthread_barrier_init(&barrier_, NULL, count);
}

inline
Barrier::~Barrier() throw()
{
  pthread_barrier_destroy(&barrier_);
}

inline
bool
Barrier::wait() throw()
{
  int res = pthread_barrier_wait(&barrier_);
  return (res == PTHREAD_BARRIER_SERIAL_THREAD);
}

#endif  /*BARRIER_HPP*/
