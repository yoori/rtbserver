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

#include <vector>
#include <iostream>

#include <Generics/Singleton.hpp>
#include <String/StringManip.hpp>
#include <String/AsciiStringManip.hpp>

#include "SimpleFixedAllocator.hpp"
#include "ExternalIdHashAdapter.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  namespace
  {
    const String::AsciiStringManip::CharCategory DECIMAL_NUMBER("0-9");
    const String::AsciiStringManip::CharCategory HEX_LOWALPHA_NUMBER("0-9a-f");
    const String::AsciiStringManip::CharCategory HEX_UPALPHA_NUMBER("0-9A-F");

    const String::AsciiStringManip::CharCategory BASE64MOD_CHARS("0-9a-zA-Z_-");
    const String::AsciiStringManip::CharCategory PADDING_DOT_CHARS(".");

    const String::AsciiStringManip::CharCategory BASE64STD_CHARS("0-9a-zA-Z+/");
    const String::AsciiStringManip::CharCategory PADDING_EQ_CHARS("=");

    const char HEX_LOW_DIGITS[17] = "0123456789abcdef";
    const char HEX_UP_DIGITS[17] = "0123456789ABCDEF";

    std::string
    hex_low_encode(
      const unsigned char* data,
      size_t size,
      bool skip_leading_zeroes) throw (eh::Exception)
    {
      if(!size)
      {
        return std::string();
      }

      if(skip_leading_zeroes)
      {
        while(!*data)
        {
          data++;
          if(!--size)
          {
            return std::string(1, '0');
          }
        }
      }

      std::string result;
      result.reserve(size * 2);

      if(skip_leading_zeroes && !((*data) & 0xF0))
      {
        result.push_back(HEX_LOW_DIGITS[*data]);
        data++;
        size--;
      }

      for(; size--; data++)
      {
        char buf[2] =
          {
            HEX_LOW_DIGITS[(*data) >> 4],
            HEX_LOW_DIGITS[(*data) & 0xF]
          };
        result.append(buf, 2);
      }

      return result;
    }

    std::string
    hex_up_encode(
      const unsigned char* data,
      size_t size,
      bool skip_leading_zeroes) throw (eh::Exception)
    {
      if(!size)
      {
        return std::string();
      }

      if(skip_leading_zeroes)
      {
        while(!*data)
        {
          data++;
          if(!--size)
          {
            return std::string(1, '0');
          }
        }
      }

      std::string result;
      result.reserve(size * 2);

      if(skip_leading_zeroes && !((*data) & 0xF0))
      {
        result.push_back(HEX_UP_DIGITS[*data]);
        data++;
        size--;
      }

      for(; size--; data++)
      {
        char buf[2] =
          {
            HEX_UP_DIGITS[(*data) >> 4],
            HEX_UP_DIGITS[(*data) & 0xF]
          };
        result.append(buf, 2);
      }

      return result;
    }

    /*
    void
    base64_decode(
      std::string& dst,
      const String::SubString& src,
      bool padding = true)
      throw (eh::Exception)
    {
      if(!size)
      {
        return std::string();
      }

      if(skip_leading_zeroes)
      {
        while(!*data)
        {
          data++;
          if(!--size)
          {
            return std::string(1, '0');
          }
        }
      }

      std::string result;
      result.reserve(size * 2);

      if(skip_leading_zeroes && !((*data) & 0xF0))
      {
        result.push_back(HEX_LOW_DIGITS[*data]);
        data++;
        size--;
      }

      for(; size--; data++)
      {
        char buf[2] =
          {
            HEX_LOW_DIGITS[(*data) >> 4],
            HEX_LOW_DIGITS[(*data) & 0xF]
          };
        result.append(buf, 2);
      }

      return result;
    }
    */

    typedef Generics::Singleton<
      SimpleDistribAllocator,
      Generics::Helper::AutoPtr<SimpleDistribAllocator>,
      Generics::AtExitDestroying::DP_LOUD_COUNTER + 1>
      ExternalIdKeyAllocator;
  };

  class ExternalIdHashAdapter::EncodingSelector:
    public ReferenceCounting::AtomicImpl
  {
  public:
    struct Encoder: public ReferenceCounting::AtomicImpl
    {
      Encoder(unsigned char id_val)
        : id_(id_val)
      {}

      unsigned char
      id() const
      {
        return id_;
      }

      virtual bool
      can_encode(const String::SubString& text) const = 0;

      virtual unsigned long
      encode_size(const String::SubString& text) const = 0;

      virtual void
      encode(void* buf, const String::SubString& text) const = 0;

      virtual unsigned long
      encoded_size_by_buf(const void* buf) const = 0;

      virtual std::string
      decode(const void* buf) const = 0;

    protected:
      unsigned char id_;
    };

    typedef ReferenceCounting::ConstPtr<Encoder>
      CEncoder_var;

  public:
    EncodingSelector() throw();

    const Encoder*
    select_encoder(const String::SubString& text) throw();

    const Encoder*
    get_encoder(unsigned char index);

  protected:
    virtual
    ~EncodingSelector() throw()
    {}

  protected:
    std::vector<CEncoder_var> all_encoders_;
    std::vector<CEncoder_var> encoders_;
  };

  typedef ReferenceCounting::SmartPtr<ExternalIdHashAdapter::EncodingSelector>
    EncodingSelector_var;

  typedef Generics::Singleton<
    ExternalIdHashAdapter::EncodingSelector,
    EncodingSelector_var,
    Generics::AtExitDestroying::DP_LOUD_COUNTER>
    EncodingSelectorSingleton;

  // ExternalIdHashAdapter
  const unsigned int EXT_HASH_BUF_PRESPACE = 4;

  ExternalIdHashAdapter::
  ExternalIdHashAdapter(const String::SubString& text, size_t hash)
    throw()
  {
    const EncodingSelector::Encoder* encoder =
      EncodingSelectorSingleton::instance().select_encoder(text);

    unsigned long encode_size = encoder->encode_size(text);
    data_ = ExternalIdKeyAllocator::instance().alloc(EXT_HASH_BUF_PRESPACE + 1 + encode_size);
    *static_cast<uint32_t*>(data_) = hash;
    *(static_cast<unsigned char*>(data_) + EXT_HASH_BUF_PRESPACE) = encoder->id();

    encoder->encode(
      static_cast<unsigned char*>(data_) + EXT_HASH_BUF_PRESPACE + 1,
      text);
  }

  ExternalIdHashAdapter::
  ExternalIdHashAdapter(const ExternalIdHashAdapter& init)
    throw()
  {
    if(init.data_)
    {
      const EncodingSelector::Encoder* encoder =
        EncodingSelectorSingleton::instance().get_encoder(
          *(static_cast<unsigned char*>(init.data_) + EXT_HASH_BUF_PRESPACE));

      int ssize = encoder->encoded_size_by_buf(
        static_cast<char*>(init.data_) + EXT_HASH_BUF_PRESPACE + 1);
      data_ = ExternalIdKeyAllocator::instance().alloc(EXT_HASH_BUF_PRESPACE + 1 + ssize);
      ::memcpy(data_, init.data_, EXT_HASH_BUF_PRESPACE + 1 + ssize);
    }
    else
    {
      data_ = 0;
    }
  }

  ExternalIdHashAdapter::
  ~ExternalIdHashAdapter() throw()
  {
    free_buf_();
  }

  ExternalIdHashAdapter&
  ExternalIdHashAdapter::
  operator=(ExternalIdHashAdapter&& init)
    throw()
  {
    free_buf_();

    data_ = init.data_;
    init.data_ = 0;

    return *this;
  }

  void
  ExternalIdHashAdapter::free_buf_() throw()
  {
    if(data_)
    {
      const EncodingSelector::Encoder* encoder =
        EncodingSelectorSingleton::instance().get_encoder(
          *(static_cast<unsigned char*>(data_) + EXT_HASH_BUF_PRESPACE));

      int ssize = encoder->encoded_size_by_buf(
        static_cast<char*>(data_) + EXT_HASH_BUF_PRESPACE + 1);

      ExternalIdKeyAllocator::instance().dealloc(
        data_,
        EXT_HASH_BUF_PRESPACE + 1 + ssize);

      data_ = 0;
    }
  }

  bool
  ExternalIdHashAdapter::
  operator==(const ExternalIdHashAdapter& right) const
    throw()
  {
    if(::memcmp(data_, right.data_, EXT_HASH_BUF_PRESPACE + 1) != 0)
    {
      return false;
    }

    const EncodingSelector::Encoder* encoder =
      EncodingSelectorSingleton::instance().get_encoder(
        *(static_cast<unsigned char*>(data_) + EXT_HASH_BUF_PRESPACE));

    unsigned long left_data_size = encoder->encoded_size_by_buf(
      static_cast<char*>(data_) + EXT_HASH_BUF_PRESPACE + 1);
    unsigned long right_data_size = encoder->encoded_size_by_buf(
      static_cast<char*>(right.data_) + EXT_HASH_BUF_PRESPACE + 1);
    return left_data_size == right_data_size &&
      ::memcmp(
        static_cast<char*>(data_) + EXT_HASH_BUF_PRESPACE + 1,
        static_cast<char*>(right.data_) + EXT_HASH_BUF_PRESPACE + 1,
        left_data_size) == 0;
  }

  std::string
  ExternalIdHashAdapter::text() const
    throw()
  {
    if(data_)
    {
      const EncodingSelector::Encoder* encoder =
        EncodingSelectorSingleton::instance().get_encoder(
          *(static_cast<unsigned char*>(data_) + EXT_HASH_BUF_PRESPACE));

      return encoder->decode(
        static_cast<char*>(data_) + EXT_HASH_BUF_PRESPACE + 1);
    }
    else
    {
      return std::string();
    }
  }

  unsigned char
  ExternalIdHashAdapter::encoder_id() const
    throw()
  {
    return *(static_cast<unsigned char*>(data_) + EXT_HASH_BUF_PRESPACE);
  }

  // Encoder's
  namespace Encoders
  {
    class DefaultEncoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      DefaultEncoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id)
      {}

      virtual bool
      can_encode(const String::SubString&) const
      {
        return true;
      }

      virtual unsigned long
      encode_size(const String::SubString& text) const
      {
        return text.size() + 4;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        *static_cast<uint32_t*>(buf) = text.size();
        ::memcpy(static_cast<unsigned char*>(buf) + 4,
          text.data(),
          text.size());
      }

      virtual unsigned long
      encoded_size_by_buf(const void* buf) const
      {
        return *static_cast<const uint32_t*>(buf) + 4;
      }

      virtual std::string
      decode(const void* buf) const
      {
        return std::string(
          static_cast<const char*>(buf) + 4,
          *static_cast<const uint32_t*>(buf));
      }

    protected:
      virtual ~DefaultEncoder() throw()
      {}
    };

    // SizeLess256Encoder
    class SizeLess256Encoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      SizeLess256Encoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id)
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        return text.size() < 256;
      }

      virtual unsigned long
      encode_size(const String::SubString& text) const
      {
        assert(text.size() < 256);
        return text.size() + 1;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() < 256);
        *static_cast<unsigned char*>(buf) = text.size();
        ::memcpy(static_cast<unsigned char*>(buf) + 1,
          text.data(),
          text.size());
      }

      virtual unsigned long
      encoded_size_by_buf(const void* buf) const
      {
        return *static_cast<const unsigned char*>(buf) + 1;
      }

      virtual std::string
      decode(const void* buf) const
      {
        return std::string(
          static_cast<const char*>(buf) + 1,
          *static_cast<const unsigned char*>(buf));
      }

    protected:
      virtual ~SizeLess256Encoder() throw()
      {}
    };

    // h8_h4_h4_h4_h12_Encoder
    class h8_h4_h4_h4_h12_Encoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      h8_h4_h4_h4_h12_Encoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id),
          minus_("-")
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        if(text.size() == 36 && text[8] == '-' && text[13] == '-' &&
          text[18] == '-' && text[23] == '-')
        {
          if((HEX_LOWALPHA_NUMBER.find_nonowned(text.begin(), text.begin() + 8) ==
              text.begin() + 8) &&
            (HEX_LOWALPHA_NUMBER.find_nonowned(text.begin() + 9, text.begin() + 13) ==
              text.begin() + 13) &&
            (HEX_LOWALPHA_NUMBER.find_nonowned(text.begin() + 14, text.begin() + 18) ==
              text.begin() + 18) &&
            (HEX_LOWALPHA_NUMBER.find_nonowned(text.begin() + 19, text.begin() + 23) ==
              text.begin() + 23) &&
            (HEX_LOWALPHA_NUMBER.find_nonowned(text.begin() + 24, text.begin() + 36) ==
              text.begin() + 36))
          {
            return true;
          }
        }

        return false;
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return 16;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() == 36);
        char* cbuf = static_cast<char*>(buf);
        String::AsciiStringManip::hex_to_buf(text.substr(0, 8), cbuf + 0);
        String::AsciiStringManip::hex_to_buf(text.substr(9, 4), cbuf + 4);
        String::AsciiStringManip::hex_to_buf(text.substr(14, 4), cbuf + 6);
        String::AsciiStringManip::hex_to_buf(text.substr(19, 4), cbuf + 8);
        String::AsciiStringManip::hex_to_buf(text.substr(24, 12), cbuf + 10);
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return 16;
      }

      virtual std::string
      decode(const void* buf) const
      {
        const unsigned char* cbuf = static_cast<const unsigned char*>(buf);
        return hex_low_encode(cbuf, 4, false) + minus_ +
          hex_low_encode(cbuf + 4, 2, false) + minus_ +
          hex_low_encode(cbuf + 6, 2, false) + minus_ +
          hex_low_encode(cbuf + 8, 2, false) + minus_ +
          hex_low_encode(cbuf + 10, 6, false);
      }

    protected:
      virtual ~h8_h4_h4_h4_h12_Encoder() throw()
      {}

    protected:
      const std::string minus_;
    };

    // H8_H4_H4_H4_H12_Encoder
    class H8_H4_H4_H4_H12_Encoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      H8_H4_H4_H4_H12_Encoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id),
          minus_("-")
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        if(text.size() == 36 && text[8] == '-' && text[13] == '-' &&
          text[18] == '-' && text[23] == '-')
        {
          if((HEX_UPALPHA_NUMBER.find_nonowned(text.begin(), text.begin() + 8) ==
              text.begin() + 8) &&
            (HEX_UPALPHA_NUMBER.find_nonowned(text.begin() + 9, text.begin() + 13) ==
              text.begin() + 13) &&
            (HEX_UPALPHA_NUMBER.find_nonowned(text.begin() + 14, text.begin() + 18) ==
              text.begin() + 18) &&
            (HEX_UPALPHA_NUMBER.find_nonowned(text.begin() + 19, text.begin() + 23) ==
              text.begin() + 23) &&
            (HEX_UPALPHA_NUMBER.find_nonowned(text.begin() + 24, text.begin() + 36) ==
              text.begin() + 36))
          {
            return true;
          }
        }

        return false;
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return 16;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() == 36);
        char* cbuf = static_cast<char*>(buf);
        String::AsciiStringManip::hex_to_buf(text.substr(0, 8), cbuf + 0);
        String::AsciiStringManip::hex_to_buf(text.substr(9, 4), cbuf + 4);
        String::AsciiStringManip::hex_to_buf(text.substr(14, 4), cbuf + 6);
        String::AsciiStringManip::hex_to_buf(text.substr(19, 4), cbuf + 8);
        String::AsciiStringManip::hex_to_buf(text.substr(24, 12), cbuf + 10);
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return 16;
      }

      virtual std::string
      decode(const void* buf) const
      {
        const unsigned char* cbuf = static_cast<const unsigned char*>(buf);
        return hex_up_encode(cbuf, 4, false) + minus_ +
          hex_up_encode(cbuf + 4, 2, false) + minus_ +
          hex_up_encode(cbuf + 6, 2, false) + minus_ +
          hex_up_encode(cbuf + 8, 2, false) + minus_ +
          hex_up_encode(cbuf + 10, 6, false);
      }

    protected:
      virtual ~H8_H4_H4_H4_H12_Encoder() throw()
      {}

    protected:
      const std::string minus_;
    };

    // hX_Encoder
    class hX_Encoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      hX_Encoder(
        unsigned char id,
        unsigned long x,
        const String::SubString& prefix = String::SubString())
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id),
          len_(x),
          prefix_(prefix.str())
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        return text.size() == prefix_.size() + len_ &&
          text.compare(0, prefix_.size(), prefix_.data(), prefix_.size()) == 0 &&
          HEX_LOWALPHA_NUMBER.find_nonowned(text.begin() + prefix_.size(), text.end()) == text.end();
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return (len_ + 1) / 2;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() == len_ + prefix_.size());
        char* cbuf = static_cast<char*>(buf);
        String::AsciiStringManip::hex_to_buf(
          text.substr(prefix_.size()), cbuf);
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return (len_ + 1) / 2;
      }

      virtual std::string
      decode(const void* buf) const
      {
        const unsigned char* cbuf = static_cast<const unsigned char*>(buf);
        return prefix_ + hex_low_encode(cbuf, (len_ + 1) / 2, false);
      }

    protected:
      virtual ~hX_Encoder() throw()
      {}

    protected:
      const unsigned long len_;
      const std::string prefix_;
    };

    // HX_Encoder
    class HX_Encoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      HX_Encoder(
        unsigned char id,
        unsigned long x,
        const String::SubString& prefix = String::SubString())
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id),
          len_(x),
          prefix_(prefix.str())
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        return text.size() == prefix_.size() + len_ &&
          text.compare(0, prefix_.size(), prefix_.data(), prefix_.size()) == 0 &&
          HEX_UPALPHA_NUMBER.find_nonowned(text.begin() + prefix_.size(), text.end()) == text.end();
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return (len_ + 1) / 2;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() == len_ + prefix_.size());
        char* cbuf = static_cast<char*>(buf);
        String::AsciiStringManip::hex_to_buf(
          text.substr(prefix_.size()), cbuf);
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return (len_ + 1) / 2;
      }

      virtual std::string
      decode(const void* buf) const
      {
        const unsigned char* cbuf = static_cast<const unsigned char*>(buf);
        return prefix_ + String::StringManip::hex_encode(
          cbuf, (len_ + 1) / 2, false);
      }

    protected:
      virtual ~HX_Encoder() throw()
      {}

    protected:
      const unsigned long len_;
      const std::string prefix_;
    };

    // CAESE_B22_Encoder
    class CAESE_B22_Encoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      CAESE_B22_Encoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id),
          prefix_("CAESE")
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        return text.size() == 27 &&
          text.compare(0, 5, "CAESE", 5) == 0 &&
          BASE64MOD_CHARS.find_nonowned(text.begin() + 5, text.end()) == text.end();
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return 17;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() == 27);
        std::string d;
        String::StringManip::base64mod_decode(d, text.substr(5, 20), false);
        assert(d.size() == 15);
        ::memcpy(buf, d.data(), d.size());
        char* cbuf = static_cast<char*>(buf);
        cbuf[15] = text[25];
        cbuf[16] = text[26];        
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return 17;
      }

      virtual std::string
      decode(const void* buf) const
      {
        std::string d;
        d.reserve(17);
        String::StringManip::base64mod_encode(d, buf, 15, false);
        const char* cbuf = static_cast<const char*>(buf);
        d.push_back(cbuf[15]);
        d.push_back(cbuf[16]);
        return prefix_ + d;
      }

    protected:
      virtual ~CAESE_B22_Encoder() throw()
      {}

    protected:
      const std::string prefix_;
    };

    void
    base64mod_encode_wrap(
      std::string& dest,
      const void* src,
      size_t size,
      bool padding)
    {
      String::StringManip::base64mod_encode(dest, src, size, padding);
    }

    template<
      void (*encode_fun)(std::string&, const void*, size_t, bool),
      void (*decode_fun)(std::string&, const String::SubString&, bool, uint8_t*)>
    class BasicBase64Encoder: public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      // data_len : size of data encoded in string
      // must be defined on encoder level for force holding of size in data block
      // actual only if string have padding
      BasicBase64Encoder(
        unsigned char id,
        const String::AsciiStringManip::CharCategory& base64_chars,
        const String::AsciiStringManip::CharCategory& padding_chars,
        unsigned long len,
        bool padding,
        const String::SubString& prefix = String::SubString(),
        unsigned long data_len = 0,
        const String::SubString& add_suffix = String::SubString())
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id),
          base64_chars_(base64_chars),
          padding_chars_(padding_chars),
          len_(len),
          padding_(padding),
          prefix_(prefix.str()),
          data_len_(data_len ? data_len : (len * 3 + 3) / 4),
          add_suffix_(add_suffix.str())
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        // 
        if(text.size() == len_ + prefix_.size() &&
          text.compare(0, prefix_.size(), prefix_.data(), prefix_.size()) == 0)
        {
          String::SubString base64_str = text.substr(prefix_.size());

          auto pad_it = base64_chars_.find_nonowned(base64_str.begin(), base64_str.end());
          auto it = padding_ ? padding_chars_.find_nonowned(pad_it, base64_str.end()) : pad_it;

          if(it != base64_str.end())
          {
            return false;
          }

          std::string d;
          try
          {
            if(add_suffix_.empty())
            {
              decode_fun(d, base64_str, padding_, 0);
            }
            else
            {
              decode_fun(d, base64_str.str() + add_suffix_, padding_, 0);
            }
          }
          catch(const eh::Exception&)
          {
            return false;
          }

          return d.size() == data_len_;
        }

        return false;
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return data_len_;
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return data_len_;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        assert(text.size() == len_ + prefix_.size());
        std::string d;
        if(add_suffix_.empty())
        {
          decode_fun(d, text.substr(prefix_.size()), padding_, 0);
        }
        else
        {
          decode_fun(d, text.substr(prefix_.size()).str() + add_suffix_, padding_, 0);
        }

        //std::cerr << "d.size() = " << d.size() << ", padding_ = " << (padding_ ? "true" : "false") << std::endl;
        assert(d.size() == data_len_);
        ::memcpy(buf, d.data(), d.size());
      }

      virtual std::string
      decode(const void* buf) const
      {
        std::string d;
        encode_fun(
          d,
          static_cast<const unsigned char*>(buf),
          data_len_,
          padding_);

        if(!add_suffix_.empty())
        {
          assert(d.compare(d.size() - add_suffix_.size(), add_suffix_.size(), add_suffix_) == 0);
          d.resize(d.size() - add_suffix_.size());
        }

        return prefix_ + d;
      }

    protected:
      virtual ~BasicBase64Encoder() throw()
      {}

    protected:
      const String::AsciiStringManip::CharCategory& base64_chars_;
      const String::AsciiStringManip::CharCategory& padding_chars_;
      const unsigned long len_;
      const bool padding_;
      const std::string prefix_;
      const unsigned long data_len_;
      const std::string add_suffix_;
    };

    // BMX_Encoder
    class BMX_Encoder: public BasicBase64Encoder<
      base64mod_encode_wrap,
      String::StringManip::base64mod_decode>
    {
    public:
      // data_len : size of data encoded in string
      // must be defined on encoder level for force holding of size in data block
      // actual only if string have padding
      BMX_Encoder(
        unsigned char id,
        unsigned long len,
        bool padding,
        const String::SubString& prefix = String::SubString(),
        unsigned long data_len = 0,
        const String::SubString& add_suffix = String::SubString())
        : BasicBase64Encoder<base64mod_encode_wrap, String::StringManip::base64mod_decode>(
            id,
            BASE64MOD_CHARS,
            PADDING_DOT_CHARS,
            len,
            padding,
            prefix,
            data_len,
            add_suffix)
      {}

    protected:
      virtual ~BMX_Encoder() throw()
      {}
    };

    // BSX_Encoder
    class BSX_Encoder: public BasicBase64Encoder<
      String::StringManip::base64_encode,
      String::StringManip::base64mod_decode> // base64mod_decode allow to decode standard base64
    {
    public:
      // data_len : size of data encoded in string
      // must be defined on encoder level for force holding of size in data block
      // actual only if string have padding
      BSX_Encoder(
        unsigned char id,
        unsigned long len,
        bool padding,
        const String::SubString& prefix = String::SubString(),
        unsigned long data_len = 0,
        const String::SubString& add_suffix = String::SubString())
        : BasicBase64Encoder<String::StringManip::base64_encode, String::StringManip::base64mod_decode>(
            id,
            BASE64STD_CHARS,
            PADDING_EQ_CHARS,
            len,
            padding,
            prefix,
            data_len,
            add_suffix)
      {}

    protected:
      virtual ~BSX_Encoder() throw()
      {}
    };

    // UIntEncoder
    class UIntEncoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      UIntEncoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id)
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        return !text.empty() &&
          text.size() <= 9 &&
          *text.begin() != '0' &&
          DECIMAL_NUMBER.find_nonowned(text.begin(), text.end()) == text.end();
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return 4;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        uint32_t val;
        if(!String::StringManip::str_to_int(text, val))
        {
          assert(0);
        }

        *static_cast<uint32_t*>(buf) = val;
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return 4;
      }

      virtual std::string
      decode(const void* buf) const
      {
        char res_str[std::numeric_limits<uint32_t>::digits10 + 3];
        if(!String::StringManip::int_to_str(*static_cast<const uint32_t*>(buf), res_str, sizeof(res_str)))
        {
          assert(0);
        }

        return std::string(res_str);
      }

    protected:
      virtual ~UIntEncoder() throw()
      {}
    };

    // DoubleUIntEncoder
    class DoubleUIntEncoder:
      public ExternalIdHashAdapter::EncodingSelector::Encoder
    {
    public:
      DoubleUIntEncoder(unsigned char id)
        : ExternalIdHashAdapter::EncodingSelector::Encoder(id)
      {}

      virtual bool
      can_encode(const String::SubString& text) const
      {
        // can be extended if use 18446744073709551615 as max
        return !text.empty() &&
          text.size() > 9 &&
          text.size() < 20 &&
          *text.begin() != '0' &&
          DECIMAL_NUMBER.find_nonowned(text.begin(), text.end()) == text.end();
      }

      virtual unsigned long
      encode_size(const String::SubString&) const
      {
        return 8;
      }

      virtual void
      encode(void* buf, const String::SubString& text) const
      {
        uint64_t val;
        if(!String::StringManip::str_to_int(text, val))
        {
          assert(0);
        }

        *static_cast<uint64_t*>(buf) = val;
      }

      virtual unsigned long
      encoded_size_by_buf(const void*) const
      {
        return 8;
      }

      virtual std::string
      decode(const void* buf) const
      {
        char res_str[std::numeric_limits<uint64_t>::digits10 + 3];
        if(!String::StringManip::int_to_str(*static_cast<const uint64_t*>(buf), res_str, sizeof(res_str)))
        {
          assert(0);
        }

        return std::string(res_str);
      }

    protected:
      virtual ~DoubleUIntEncoder() throw()
      {}
    };
  };

  ExternalIdHashAdapter::EncodingSelector::
  EncodingSelector() throw()
  {
    unsigned long encoder_id = 0;

    all_encoders_.push_back(new Encoders::DefaultEncoder(++encoder_id));
    all_encoders_.push_back(new Encoders::SizeLess256Encoder(++encoder_id));

    // base64 encoders
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 8, false)); // relap, otm
    all_encoders_.push_back(new Encoders::BSX_Encoder(++encoder_id, 12, true, String::SubString(), 7));
    all_encoders_.push_back(new Encoders::BSX_Encoder(++encoder_id, 12, true, String::SubString(), 8));
    all_encoders_.push_back(new Encoders::BSX_Encoder(++encoder_id, 12, false));
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 12, false)); // otm, mgid
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 15, false, String::SubString(), 0, String::SubString("A")));
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 20, false)); // rtw(subset)
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 20, false, String::SubString("aid"))); // otm(aid..)
    all_encoders_.push_back(new Encoders::BSX_Encoder(++encoder_id, 24, true, String::SubString(), 16));
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 24, true, String::SubString(), 16)); // c
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 22, false, String::SubString(), 16));
    all_encoders_.push_back(new Encoders::BMX_Encoder(++encoder_id, 22, false, String::SubString(), 17)); // rtw
    all_encoders_.push_back(new Encoders::CAESE_B22_Encoder(++encoder_id)); // google
    all_encoders_.push_back(new Encoders::BSX_Encoder(++encoder_id, 44, true, String::SubString(), 32)); // convertise

    // hex encoders
    all_encoders_.push_back(new Encoders::hX_Encoder(++encoder_id, 24)); // rtw(subset)
    all_encoders_.push_back(new Encoders::hX_Encoder(++encoder_id, 32)); // gpm, admixer, hyper, admedia, convertise
    all_encoders_.push_back(new Encoders::hX_Encoder(++encoder_id, 32, String::SubString("aid"))); // otm(aid..)
    all_encoders_.push_back(new Encoders::hX_Encoder(++encoder_id, 40)); // videonow, convertise
    all_encoders_.push_back(new Encoders::hX_Encoder(++encoder_id, 64)); // bln, decenterads
    all_encoders_.push_back(new Encoders::hX_Encoder(++encoder_id, 96)); // buz, moevideo, smartyads

    all_encoders_.push_back(new Encoders::HX_Encoder(++encoder_id, 32)); // sape, ina
    all_encoders_.push_back(new Encoders::HX_Encoder(++encoder_id, 40));

    all_encoders_.push_back(new Encoders::h8_h4_h4_h4_h12_Encoder(++encoder_id)); // ifa, rpb, rambler
    all_encoders_.push_back(new Encoders::H8_H4_H4_H4_H12_Encoder(++encoder_id)); // mobfox
      // advark, btw, buz

    all_encoders_.push_back(new Encoders::DoubleUIntEncoder(++encoder_id)); //
    all_encoders_.push_back(new Encoders::UIntEncoder(++encoder_id)); // pprofit
    // TODO yandex/

    encoders_.resize(256);
    for(auto enc_it = all_encoders_.begin(); enc_it != all_encoders_.end(); ++enc_it)
    {
      encoders_[(*enc_it)->id()] = *enc_it;
    }

    /*
    {
      const String::SubString str("RBaYqGxyTK6gomazq5K2SA");
      const Encoder* encoder = select_encoder(str);
      std::vector<unsigned char> buf(encoder->encode_size(str));
      encoder->encode(&buf[0], str);
      std::string str_res = encoder->decode(&buf[0]);

      std::string dest;
      String::StringManip::base64mod_decode(dest, str, false, 0);
      std::cerr << "ENC RES2(" << static_cast<unsigned int>(encoder->id()) << "): <" << str_res <<
        "> (D=" << dest.size() << ")" << std::endl;
    }
    */
  };

  const ExternalIdHashAdapter::EncodingSelector::Encoder*
  ExternalIdHashAdapter::EncodingSelector::
  select_encoder(const String::SubString& text) throw()
  {
    for(auto encoder_it = all_encoders_.rbegin(); encoder_it != all_encoders_.rend(); ++encoder_it)
    {
      if((*encoder_it)->can_encode(text))
      {
        /*
        if((*encoder_it)->id() == 2)
        {
          Stream::Error ostr;
          ostr << text << std::endl;
          std::cerr << ostr.str();
        }
        */

        return (*encoder_it);
      }
    }

    // unreachable, because encoders_.begin()->can_encode return true
    return *all_encoders_.begin();
  };

  const ExternalIdHashAdapter::EncodingSelector::Encoder*
  ExternalIdHashAdapter::EncodingSelector::
  get_encoder(unsigned char index)
  {
    const ExternalIdHashAdapter::EncodingSelector::Encoder* ret =
      encoders_[index];
    assert(ret);
    return ret;
  };  
};
};
