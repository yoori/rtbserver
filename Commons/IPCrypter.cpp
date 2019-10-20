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

#include <Generics/CRC.hpp>
#include <Generics/Rand.hpp>
#include <String/StringManip.hpp>
#include <Stream/MemoryStream.hpp>

#include "IPCrypter.hpp"

namespace AdServer
{
namespace Commons
{
  //
  // AesEncryptKey
  //
  AesEncryptKey::AesEncryptKey() throw()
  {}

  AesEncryptKey::AesEncryptKey(const unsigned char *key, unsigned long size)
    throw()
  {
    AES_set_encrypt_key(key, size*8, &key_);
  }

  AesEncryptKey::~AesEncryptKey() throw ()
  {
    OPENSSL_cleanse(&key_, sizeof(key_));
  }

  void
  AesEncryptKey::encrypt(void* out, const void* in) const
    throw ()
  {
    AES_encrypt(
      static_cast<const unsigned char*>(in),
      static_cast<unsigned char*>(out),
      &key_);
  }

  //
  // AesDecryptKey
  //
  AesDecryptKey::AesDecryptKey() throw()
  {}

  AesDecryptKey::AesDecryptKey(const unsigned char *key, unsigned long size)
    throw()
  {
    AES_set_decrypt_key(key, size*8, &key_);
  }

  AesDecryptKey::~AesDecryptKey() throw ()
  {
    OPENSSL_cleanse(&key_, sizeof(key_));
  }

  void
  AesDecryptKey::decrypt(void* out, const void* in) const
    throw ()
  {
    AES_decrypt(
      static_cast<const unsigned char*>(in),
      static_cast<unsigned char*>(out),
      &key_);
  }

  // IPCrypter
  IPCrypter::IPCrypter(const String::SubString& key)
    throw(InvalidKey)
    : encrypt_key_(make_key_<AesEncryptKey>(key)),
      decrypt_key_(make_key_<AesDecryptKey>(key))
  {}

  void
  IPCrypter::encrypt(
    std::string& encrypted_ip,
    const char* ip)
    throw(InvalidParams)
  {
    static const char* FUN = "IPCrypter::encrypt()";

    uint32_t ip_address_buf[4];
    uint32_t ip_address;
    if (inet_pton(AF_INET, ip, ip_address_buf) != 0)
    {
      ip_address = ip_address_buf[0];      
    }
    else if(inet_pton(AF_INET6, ip, ip_address_buf) != 0)
    {
      ip_address = ip_address_buf[3];
    }
    else
    {
      Stream::Error ostr;
      ostr << FUN << ": incorrect ip value '" << ip << "'";
      throw InvalidParams(ostr);
    }

    uint32_t encrypted_buf[AES_BLOCK_SIZE / 4 + 1];
    uint32_t buf[AES_BLOCK_SIZE / 4 + 1];
    *buf = ip_address;
    for(uint32_t* salt = buf + 1;
        salt < buf + AES_BLOCK_SIZE / 4 - 1; ++salt)
    {
      *salt = Generics::safe_rand();
    }
    *(buf + AES_BLOCK_SIZE / 4 - 1) = Generics::CRC::quick(
      0, buf, (AES_BLOCK_SIZE / 4 - 1) * 4);

    encrypt_key_.encrypt(encrypted_buf, buf);

    String::StringManip::base64mod_encode(encrypted_ip,
      encrypted_buf, AES_BLOCK_SIZE, false);
  }

  void
  IPCrypter::decrypt(
    std::string& ip,
    const String::SubString& encrypted_ip)
    throw(InvalidParams)
  {
    static const char* FUN = "IPCrypter::decrypt()";

    std::string encrypted_buf;

    try
    {
      String::StringManip::base64mod_decode(
        encrypted_buf, encrypted_ip, false);
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": can't do base64 decode: " << ex.what();
      throw InvalidParams(ostr);
    }

    if(encrypted_buf.size() < AES_BLOCK_SIZE)
    {
      Stream::Error ostr;
      ostr << FUN << ": incorrect size of encrypted buffer " <<
        encrypted_buf.size() << " instead " << AES_BLOCK_SIZE;
      throw InvalidParams(ostr);
    }

    uint32_t buf[AES_BLOCK_SIZE / 4 + 1];
    decrypt_key_.decrypt(buf, encrypted_buf.data());

    // check CRC
    if(Generics::CRC::quick(0, buf, (AES_BLOCK_SIZE / 4 - 1) * 4) !=
       *(buf + 3))
    {
      Stream::Error ostr;
      ostr << FUN << ": incorrect CRC sum";
      throw InvalidParams(ostr);
    }

    char str_ip_holder[INET_ADDRSTRLEN];
    if(inet_ntop(AF_INET, buf, str_ip_holder, sizeof(str_ip_holder)))
    {
      ip = str_ip_holder;
    }
    else
    {
      // EAFNOSUPPORT, ENOSPC unexpected here
      assert(0);
    }
  }

  template<typename KeyType>
  KeyType
  IPCrypter::make_key_(const String::SubString& base64_key)
    throw(InvalidKey)
  {
    static const char* FUN = "IPCrypter::make_key_()";

    std::string key;
    try
    {
      String::StringManip::base64mod_decode(
        key, base64_key, false);
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": invalid key '" << base64_key <<
        "', base64 decode failed: " << ex.what();
      throw InvalidKey(ostr);
    }

    if(key.size() < AES_BLOCK_SIZE)
    {
      Stream::Error ostr;
      ostr << FUN << ": invalid key '" << base64_key <<
        "', size " << key.size() << " less then minimal size " << AES_BLOCK_SIZE;
      throw InvalidKey(ostr);
    }

    return KeyType(
      reinterpret_cast<const unsigned char*>(key.c_str()), AES_BLOCK_SIZE);
  }
}
}
