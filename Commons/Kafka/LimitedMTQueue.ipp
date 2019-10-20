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

/* $Id$
* @file LimitedMTQueue.ipp
* @author Artem V. Nikitin (artem_nikitin@ocslab.com)
* Limited thread safe queue implementation 
*/

namespace AdServer
{
  namespace Commons
  {
    template<class T>
    LimitedMTQueue<T>::LimitedMTQueue(size_t max_size)
      : max_size_(max_size),
        queue_size_(0),
        closed_(false)
    {}

    template<class T>
    LimitedMTQueue<T>::~LimitedMTQueue()
    {}

    template<class T>
    template <class... Args>
    bool
    LimitedMTQueue<T>::try_emplace(Args&&... args)
    {
      bool signal;

      std::list<T> queue;
      queue.emplace_back(std::forward<Args>(args)...);

      {
        SyncPolicy::WriteGuard lock(lock_);

        if(max_size_ > 0 && queue_size_ >= max_size_)
        {
          return false;
        }

        signal = queue_.empty();
        queue_.splice(queue_.begin(), queue);
        ++queue_size_; // queue.size() == 1
      }

      if(signal)
      {
        available_.broadcast();
      }

      return true;
    }

    template<class T>
    bool
    LimitedMTQueue<T>::try_push(const T& val)
    {
      bool signal;

      std::list<T> queue;
      queue.push_back(val);

      {
        SyncPolicy::WriteGuard lock(lock_);

        if(max_size_ > 0 && queue_size_ >= max_size_)
        {
          return false;
        }

        signal = queue_.empty();
        queue_.splice(queue_.begin(), queue);
        ++queue_size_; // queue_.size() == 1
      }

      if(signal)
      {
        available_.broadcast();
      }

      return true;
    }

    template<class T>
    template <class... Args>
    void
    LimitedMTQueue<T>::emplace(Args&&... args)
    {
      std::list<T> queue;
      queue.emplace_back(std::forward<Args>(args)...);

      {
        SyncPolicy::WriteGuard lock(lock_);
      
        if(max_size_ > 0)
        {
          while(queue_size_ >= max_size_)
          {
            removed_.wait(lock_);
          }
        }

        signal = queue_.empty();
        queue_.splice(queue_.begin(), queue);
        ++queue_size_; // queue_.size() == 1
      }

      if(signal)
      {
        available_.broadcast();
      }
    }

    template<class T>
    void
    LimitedMTQueue<T>::push(const T& val)
    {
      bool signal_available;
      std::list<T> queue;
      queue.push_back(val);

      {
        SyncPolicy::WriteGuard lock(lock_);
      
        if(max_size_ > 0)
        {
          while(queue_size_ >= max_size_)
          {
            removed_.wait(lock_);
          }
        }

        // assert(queue_size_ < max_size_)
        signal_available = queue_.empty();
        queue_.splice(queue_.begin(), queue);
        ++queue_size_; // queue_.size() == 1
      }

      if(signal_available)
      {
        available_.broadcast();
      }
    }

    template<class T>
    void
    LimitedMTQueue<T>::peek(T& val) const
    {
      SyncPolicy::WriteGuard lock(lock_);
      
      while(queue_.empty())
      {
        available_.wait(lock_);
        check_close();
      }
  
      val = queue_.front();
    }

    template<class T>
    void
    LimitedMTQueue<T>::pop(T& val)
    {
      bool signal_removed;
      std::list<T> queue;

      {
        SyncPolicy::WriteGuard lock(lock_);
      
        while(queue_.empty())
        {
          available_.wait(lock_);
          check_close();
        }

        queue.splice(queue.end(), queue_, queue_.begin());
        signal_removed = queue_.empty() || (queue_size_ >= max_size_);
        --queue_size_;
      }

      if(signal_removed)
      {
        removed_.broadcast();
      }

      val = std::move(queue.front());
    }

    template<class T>
    bool
    LimitedMTQueue<T>::pop(T& val, const Generics::Time& timeout)
    {
      bool signal_removed;
      std::list<T> queue;

      {
        SyncPolicy::WriteGuard lock(lock_);

        while(queue_.empty())
        {
          if(!available_.timed_wait(lock_, &timeout, true))
          {
            return false;
          }

          check_close();
        }

        queue.splice(queue.end(), queue_, queue_.begin());
        signal_removed = queue_.empty() || (queue_size_ >= max_size_);
        --queue_size_;
      }

      if(signal_removed)
      {
        removed_.broadcast();
      }

      val = std::move(queue.front());

      return true;
    }

    template<class T>
    void LimitedMTQueue<T>::pop()
    {
      T destroy_value;
      pop(destroy_value);
    } 
    
    template<class T>
    void
    LimitedMTQueue<T>::pop_all(std::list<T>& vals, const Generics::Time& timeout)
    {
      vals.clear();

      {
        SyncPolicy::WriteGuard lock(lock_);

        while(queue_.empty())
        {
          if(!available_.timed_wait(lock_, &timeout, true))
          {
            return;
          }

          check_close();
        }

        vals.splice(vals.end(), queue_, queue_.begin(), queue_.end());
        queue_size_ = 0;
      }

      removed_.broadcast();
    }

    template<class T>
    size_t
    LimitedMTQueue<T>::size() const
    {
      SyncPolicy::ReadGuard lock(lock_);
      return queue_size_;
    }
    
    template<class T>
    void
    LimitedMTQueue<T>::wait_empty()
    {
      SyncPolicy::ReadGuard lock(lock_);
      while(!queue_.empty())
      {
        removed_.wait(lock_);
      }
    }

    template<class T>
    void
    LimitedMTQueue<T>::close()
    {
      {
        SyncPolicy::WriteGuard lock(lock_);
        closed_ = true;
      }

      removed_.broadcast();
      available_.broadcast();
    }

    template<class T>
    void
    LimitedMTQueue<T>::check_close() throw(AlreadyClosed)
    {
      if(closed_)
      {
        throw AlreadyClosed("Queue is inactive");
      }
    }    
  }
}
