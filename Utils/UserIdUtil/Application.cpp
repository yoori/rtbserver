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

#include <list>
#include <vector>
#include <iterator>
#include <iostream>

#include <Generics/MemBuf.hpp>
#include <Generics/AppUtils.hpp>
#include <Generics/Uuid.hpp>

#include <Commons/Algs.hpp>
#include <Commons/UserInfoManip.hpp>
#include <Commons/IPCrypter.hpp>
#include <Commons/UserInfoManip.hpp>

#include "Application.hpp"

namespace
{
  const char USAGE[] =
    "Usage: UserIdUtil <COMMAND> [OPTIONS]\n"
    "Commands:\n"
    "  uid-to-sspuid\n"
    "  sspuid-to-uid\n"
    "  sign-uid\n"
    "Options:\n"
    "  --source: source id of SSP (empty by default)\n"
    "  --global-key: hex string with global key (empty by default)\n"
    "  --column: column with UID (0 by default)\n"
    "Sample: \n"
    "  echo UID | UserIdUtil uid-to-sspuid --source=openx --global-key=0102\n";
}

Application_::Application_()
  throw()
{}

Application_::~Application_() throw()
{}

void
Application_::main(int& argc, char** argv)
  throw(eh::Exception)
{
  Generics::AppUtils::CheckOption opt_help;
  Generics::AppUtils::StringOption opt_source_id;
  Generics::AppUtils::StringOption opt_global_key;
  Generics::AppUtils::StringOption opt_sep("\t");
  Generics::AppUtils::Option<unsigned long> opt_column(1);
  Generics::AppUtils::StringOption opt_private_key_file;

  Generics::AppUtils::Args args(-1);

  args.add(
    Generics::AppUtils::equal_name("help") ||
    Generics::AppUtils::short_name("h"),
    opt_help);
  args.add(
    Generics::AppUtils::equal_name("source"),
    opt_source_id);
  args.add(
    Generics::AppUtils::equal_name("global-key"),
    opt_global_key);
  args.add(
    Generics::AppUtils::equal_name("column"),
    opt_column);
  args.add(
    Generics::AppUtils::equal_name("private-key-file"),
    opt_private_key_file);
  args.add(
    Generics::AppUtils::equal_name("sep"),
    opt_sep);

  args.parse(argc - 1, argv + 1);

  const Generics::AppUtils::Args::CommandList& commands = args.commands();

  if(commands.empty() || opt_help.enabled() ||
     *commands.begin() == "help")
  {
    std::cout << USAGE << std::endl;
    return;
  }

  std::string command = *commands.begin();

  if(command == "uid-to-sspuid")
  {
    uid_to_sspuid_(
      std::cin,
      std::cout,
      *opt_source_id,
      *opt_global_key,
      *opt_sep,
      *opt_column);
  }
  else if(command == "sspuid-to-uid")
  {
    sspuid_to_uid_(
      std::cin,
      std::cout,
      *opt_source_id,
      *opt_global_key,
      *opt_sep,
      *opt_column);
  }
  else if(command == "sign-uid")
  {
    sign_uid_(
      std::cin,
      std::cout,
      *opt_private_key_file,
      *opt_sep,
      *opt_column);
  }
  else if(command == "help")
  {
    std::cout << USAGE << std::endl;
    return;
  }
  else
  {
    std::cerr << "Unknown command '" << command << "'. "
      "See help for more info." << std::endl;
  }
}

void
Application_::uid_to_sspuid_(
  std::istream& in,
  std::ostream& out,
  const String::SubString& source_id,
  const String::SubString& global_key,
  const String::SubString& sep,
  unsigned long column)
  throw(InvalidArgument, eh::Exception)
{
  Generics::ArrayByte ssp_uid_encrypt_key_buf;
  ssp_uid_encrypt_key_buf.reset(16);
  ::memset(ssp_uid_encrypt_key_buf.get(), 0, 16);

  if(!global_key.empty())
  {
    Generics::ArrayByte ssp_uid_encrypt_global_key_buf;
    unsigned long ssp_uid_encrypt_global_key_size =
      String::StringManip::hex_decode(global_key,
        ssp_uid_encrypt_global_key_buf);

    ::memcpy(
      ssp_uid_encrypt_key_buf.get(),
      ssp_uid_encrypt_global_key_buf.get(),
      std::min(ssp_uid_encrypt_global_key_size, 16ul));
  }

  uint32_t source_seed = Generics::CRC::quick(
    0, source_id.data(), source_id.length());
  for(unsigned long i = 0; i < 4; ++i)
  {
    *(reinterpret_cast<uint32_t*>(
      ssp_uid_encrypt_key_buf.get()) + i) ^= source_seed;
  }

  AdServer::Commons::AesEncryptKey ssp_uid_encrypt_key(
    ssp_uid_encrypt_key_buf.get(), 16);

  unsigned long line_i = 0;
  while(!in.eof())
  {
    std::string line;
    std::getline(in, line);
    if(line_i != 0)
    {
      out << std::endl;
    }

    if(line.empty())
    {
      continue;
    }
    String::SubString line_ss(line);
    String::SubString::SizeType prefix_end = 0;
    unsigned long cur_column = column - 1;
    while(cur_column > 0)
    {
      String::SubString::SizeType pos = line_ss.find(sep, prefix_end);
      if(pos == String::SubString::NPOS)
      {
        Stream::Error ostr;
        ostr << "Column #" << column << " not present in line: '" << line << "'";
        throw InvalidArgument(ostr);
      }
      prefix_end = pos + 1;
      --cur_column;
    }
    String::SubString::SizeType suffix_begin = line_ss.find(sep, prefix_end);

    String::SubString user_id_ss = String::SubString(
      line_ss.begin() + prefix_end,
      suffix_begin == String::SubString::NPOS ?
        line_ss.end() : line_ss.begin() + suffix_begin);
    try
    {
      AdServer::Commons::UserId user_id(user_id_ss, user_id_ss.size() == 24);
      out << line_ss.substr(0, prefix_end) <<
        (source_id.empty() ? user_id.to_string() : uid_to_sspuid_(user_id, ssp_uid_encrypt_key)) <<
        (suffix_begin != String::SubString::NPOS ?
          line_ss.substr(suffix_begin) : String::SubString());
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << "line #" << line_i << " incorrect user id '" <<
        user_id_ss << "': " << ex.what();
      throw InvalidArgument(ostr);
    }

    ++line_i;
  }
}

void
Application_::sspuid_to_uid_(
  std::istream& in,
  std::ostream& out,
  const String::SubString& source_id,
  const String::SubString& global_key,
  const String::SubString& sep,
  unsigned long column)
  throw(InvalidArgument, eh::Exception)
{
  Generics::ArrayByte ssp_uid_encrypt_key_buf;
  ssp_uid_encrypt_key_buf.reset(16);
  ::memset(ssp_uid_encrypt_key_buf.get(), 0, 16);

  if(!global_key.empty())
  {
    Generics::ArrayByte ssp_uid_encrypt_global_key_buf;
    unsigned long ssp_uid_encrypt_global_key_size =
      String::StringManip::hex_decode(global_key,
        ssp_uid_encrypt_global_key_buf);

    ::memcpy(
      ssp_uid_encrypt_key_buf.get(),
      ssp_uid_encrypt_global_key_buf.get(),
      std::min(ssp_uid_encrypt_global_key_size, 16ul));
  }

  uint32_t source_seed = Generics::CRC::quick(
    0, source_id.data(), source_id.length());
  for(unsigned long i = 0; i < 4; ++i)
  {
    *(reinterpret_cast<uint32_t*>(
      ssp_uid_encrypt_key_buf.get()) + i) ^= source_seed;
  }

  AdServer::Commons::AesDecryptKey ssp_uid_decrypt_key(
    ssp_uid_encrypt_key_buf.get(), 16);

  unsigned long line_i = 0;
  while(!in.eof())
  {
    std::string line;
    std::getline(in, line);
    if(line_i != 0)
    {
      out << std::endl;
    }

    if(line.empty())
    {
      continue;
    }
    String::SubString line_ss(line);
    String::SubString::SizeType prefix_end = 0;
    unsigned long cur_column = column - 1;
    while(cur_column > 0)
    {
      String::SubString::SizeType pos = line_ss.find(sep, prefix_end);
      if(pos == String::SubString::NPOS)
      {
        Stream::Error ostr;
        ostr << "Column #" << column << " not present in line: '" << line << "'";
        throw InvalidArgument(ostr);
      }
      prefix_end = pos + 1;
      --cur_column;
    }
    String::SubString::SizeType suffix_begin = line_ss.find(sep, prefix_end);

    String::SubString user_id_ss = String::SubString(
      line_ss.begin() + prefix_end,
      suffix_begin == String::SubString::NPOS ?
        line_ss.end() : line_ss.begin() + suffix_begin);

    try
    {
      AdServer::Commons::UserId ssp_uid;
      uint8_t ssp_uid_marker;

      if (user_id_ss.length() == 22)
      {
        std::string ssp_id_buf;
        String::StringManip::base64mod_decode(
          ssp_id_buf, user_id_ss, false, &ssp_uid_marker);
        ssp_uid = Generics::Uuid(ssp_id_buf.begin(), ssp_id_buf.end());
      }
      else
      {
        assert(0);
        /*
        Generics::SignedUuid signed_ssp_uid(user_id_ss.str().c_str());
        ssp_uid = signed_ssp_uid.uuid();
        ssp_uid_marker = signed_ssp_uid.data();
        */
      }

      Generics::Uuid res;
      ssp_uid_decrypt_key.decrypt(&*res.begin(), &*ssp_uid.begin());

      out << line_ss.substr(0, prefix_end) <<
        (source_id.empty() ? ssp_uid.to_string() : res.to_string()) <<
        (suffix_begin != String::SubString::NPOS ?
          line_ss.substr(suffix_begin) : String::SubString());
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << "line #" << line_i << " incorrect user id '" <<
        user_id_ss << "': " << ex.what();
      throw InvalidArgument(ostr);
    }

    ++line_i;
  }
}

std::string
Application_::uid_to_sspuid_(
  const AdServer::Commons::UserId& user_id,
  const AdServer::Commons::AesEncryptKey& encrypt_key)
  throw()
{
  const uint32_t CURRENT_SSP_USER_ID_MARKER = 1;

  AdServer::Commons::UserId ssp_user_id;
  encrypt_key.encrypt(&*ssp_user_id.begin(), &*user_id.begin());
  
  std::string ssp_user_id_str;
  String::StringManip::base64mod_encode(
    ssp_user_id_str,
    ssp_user_id.begin(),
    ssp_user_id.size(),
    false,
    CURRENT_SSP_USER_ID_MARKER);
  return ssp_user_id_str;
}

void
Application_::sign_uid_(
  std::istream& in,
  std::ostream& out,
  const String::SubString& private_key_file,
  const String::SubString& sep,
  unsigned long column)
  throw(InvalidArgument, eh::Exception)
{
  std::unique_ptr<Generics::SignedUuidGenerator> uid_signer(
    new Generics::SignedUuidGenerator(private_key_file.str().c_str()));

  unsigned long line_i = 0;
  while(!in.eof())
  {
    std::string line;
    std::getline(in, line);

    if(line.empty())
    {
      continue;
    }

    String::SubString line_ss(line);
    String::SubString::SizeType prefix_end = 0;
    unsigned long cur_column = column - 1;
    while(cur_column > 0)
    {
      String::SubString::SizeType pos = line_ss.find(sep, prefix_end);
      if(pos == String::SubString::NPOS)
      {
        Stream::Error ostr;
        ostr << "Column #" << column << " not present in line: '" << line << "'";
        throw InvalidArgument(ostr);
      }
      prefix_end = pos + 1;
      --cur_column;
    }

    String::SubString::SizeType suffix_begin = line_ss.find(sep, prefix_end);

    String::SubString user_id_ss = String::SubString(
      line_ss.begin() + prefix_end,
      suffix_begin == String::SubString::NPOS ?
        line_ss.end() : line_ss.begin() + suffix_begin);

    try
    {
      AdServer::Commons::UserId user_id(user_id_ss);
      Generics::SignedUuid signed_user_id = uid_signer->sign(user_id);
      out << line_ss.substr(0, prefix_end) <<
        signed_user_id.str() <<
        (suffix_begin != String::SubString::NPOS ?
          line_ss.substr(suffix_begin) : String::SubString()) <<
        std::endl;
    }
    catch(const eh::Exception& ex)
    {
      std::cerr << "line #" << line_i << " incorrect user id '" <<
        user_id_ss << "': " << ex.what() << std::endl;

      out << line_ss.substr(0, prefix_end) <<
        (suffix_begin != String::SubString::NPOS ?
          line_ss.substr(suffix_begin) : String::SubString()) <<
        std::endl;
    }

    ++line_i;
  }
}

int main(int argc, char** argv)
{
  try
  {
    Application::instance().main(argc, argv);
  }
  catch(const Application_::InvalidArgument& ex)
  {
    std::cerr << "Caught InvalidArgument: " << ex.what() << std::endl;
    return -1;
  }
  catch(const eh::Exception& ex)
  {
    std::cerr << "Caught eh::Exception: " << ex.what() << std::endl;
    return -1;
  }

  return 0;
}


