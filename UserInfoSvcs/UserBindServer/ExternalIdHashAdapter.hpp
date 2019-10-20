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

#ifndef EXTERNAL_ID_HASH_ADAPTER_HPP_
#define EXTERNAL_ID_HASH_ADAPTER_HPP_

namespace AdServer
{
namespace UserInfoSvcs
{
  class ExternalIdHashAdapter
  {
  public:
    class EncodingSelector;

  public:
    ExternalIdHashAdapter() throw();

    ExternalIdHashAdapter(const String::SubString& text, size_t hash)
      throw();

    ExternalIdHashAdapter(ExternalIdHashAdapter&& init) throw();

    ExternalIdHashAdapter(const ExternalIdHashAdapter& init) throw();

    ExternalIdHashAdapter&
    operator=(const ExternalIdHashAdapter& init) throw();

    ExternalIdHashAdapter&
    operator=(ExternalIdHashAdapter&& init) throw();

    ~ExternalIdHashAdapter() throw();

    bool
    operator==(const ExternalIdHashAdapter& right) const throw();

    size_t
    hash() const throw();

    std::string
    text() const throw();

    unsigned char
    encoder_id() const throw();

  protected:
    void
    free_buf_() throw();

  protected:
    void* data_;
  };  
};
};

namespace AdServer
{
namespace UserInfoSvcs
{
  // StringDefHashAdapter
  inline
  ExternalIdHashAdapter::
  ExternalIdHashAdapter() throw()
    : data_(0)
  {}

  inline
  ExternalIdHashAdapter::
  ExternalIdHashAdapter(ExternalIdHashAdapter&& init)
    throw()
  {
    data_ = init.data_;
    init.data_ = 0;
  }

  inline
  size_t
  ExternalIdHashAdapter::
  hash() const
    throw()
  {
    return data_ ? *static_cast<uint32_t*>(data_) : 0;
  }
};
};

#endif /*EXTERNAL_ID_HASH_ADAPTER_HPP_*/
