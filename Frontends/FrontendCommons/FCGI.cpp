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

#include "FCGI.hpp"
#include <sstream>
#include <String/StringManip.hpp>

namespace
{
  const std::string REQUEST_URI("REQUEST_URI");
  const std::string REQUEST_METHOD("REQUEST_METHOD");
  const std::string QUERY_STRING("QUERY_STRING");
  const std::string HTTPS("HTTPS");
  const std::string REMOTE_ADDR("REMOTE_ADDR");

  const std::string GET("GET");
  const std::string POST("POST");
  const std::string HEAD("HEAD");

  std::string PASS_REMOTE_ADDR(".remotehost");
  std::string PASS_SECURE("secure");
  std::string PASS_SECURE_VALUE("1");

  const String::SubString HEADER_SEP(": ");
  const String::SubString CONTENT_TYPE_AND_SEP("Content-Type: ");
  const String::SubString SET_COOKIE_AND_SEP("Set-Cookie: ");
  const String::SubString STATUS_AND_SEP("Status: ");
  const String::SubString CONTENT_LENGTH_AND_SEP("Content-Length: ");
}

namespace FCGI
{

InputStream::InputStream() throw () :
  pos_(0)
{}

InputStream::InputStream(const String::SubString& buf) throw () :
  buf_(buf),
  pos_(0)
{}

void
InputStream::set_buf(const String::SubString& buf) throw ()
{
  buf_ = buf;
  pos_ = 0;
}

Stream::BinaryInputStream&
InputStream::read(char_type* s, streamsize n) throw (eh::Exception)
{
  if (n > buf_.size() - pos_)
  {
    n = buf_.size() - pos_;
    if (n == 0)
    {
      setstate(std::ios_base::eofbit | std::ios_base::failbit);
      gcount_ = 0;
      return *this;
    }
  }
  memcpy(s, buf_.data() + pos_, n);
  pos_ += n;
  gcount_ = n;
  return *this;
}

void
InputStream::has_body(bool /*val*/) throw ()
{
}


OutputStream::OutputStream(HttpResponse* owner) throw ()
  : owner_(owner)
{}

Stream::BinaryOutputStream&
OutputStream::write(const char_type* s, streamsize n)
  throw (eh::Exception)
{
  ssize_t res = owner_->write(String::SubString(s, n));

  if (res < static_cast<ssize_t>(n) && n > 0)
  {
    setstate(std::ios_base::badbit | std::ios_base::failbit);
  }
  else if (n > 0)
  {
    setstate(std::ios_base::eofbit);
  }

  return *this;
}


void
HttpRequest::parse_params(
  const String::SubString& str,
  HTTP::ParamList& params)
  throw(String::StringManip::InvalidFormatException, eh::Exception)
{
  String::StringManip::SplitAmp tokenizer(str);
  String::SubString token;
  while (tokenizer.get_token(token))
  {
    String::SubString enc_name;
    String::SubString enc_value;
    String::SubString::SizeType pos = token.find('=');
    if (pos == String::SubString::NPOS)
    {
      enc_name = token;
    }
    else
    {
      enc_name = token.substr(0, pos);
      enc_value = token.substr(pos + 1);
    }

    try
    {
      HTTP::Param param;

      String::StringManip::mime_url_decode(enc_name, param.name);
      String::StringManip::mime_url_decode(enc_value, param.value);

      params.push_back(std::move(param));
    }
    catch (const String::StringManip::InvalidFormatException&)
    {
    }
  }
}

ParseRes
HttpRequest::parse(char* buf, size_t size)
{
  tinyfcgi::const_message m(buf, size);

  uint16_t id = 0;
  bool need_more = true;

  auto stdin_i = m.end();

  for(auto i = m.begin(); i != m.end(); ++i)
  {
    const tinyfcgi::header& h = *i;

    if (!h.valid())
    {
      return PARSE_INVALID_HEADER; // invalid header
    }

    if (id == 0)
    {
      if (h.type != FCGI_BEGIN_REQUEST)
      {
        return PARSE_BEGIN_REQUEST_EXPECTED; // begin request expected
      }
      id = h.id();
    }
    else
    {
      if (id != h.id())
      {
        return PARSE_INVALID_ID; // all headers should have same id
      }
    }

    if (h.type == FCGI_STDIN)
    {
      if (h.size() == 0)
      {
        need_more = false;
        break;
      }

      if (stdin_i == m.end())
      {
        stdin_i = i;
      }
      else
      {
        return PARSE_FRAGMENTED_STDIN; // fragmented stdin not supported
      }

      auto next_i = i;
      ++next_i;
      if (next_i.valid() && next_i->valid() &&
        next_i->type == FCGI_STDIN && next_i->size() > 0)
      {
        tinyfcgi::header* mh = (tinyfcgi::header*)&h;
        mh->merge_next();
        continue;
      }
    }
  }

  if(need_more)
  {
    return PARSE_NEED_MORE;
  }

  if (stdin_i.valid())
  {
    body_ = stdin_i->str();
  }

  input_stream_.set_buf(body_);
  for(auto i = m.begin(); i != m.end(); ++i)
  {
    const tinyfcgi::header& h = *i;
    if (h.type == FCGI_PARAMS)
    {
      tinyfcgi::const_params params(h.str());
      for(auto i = params.begin(); i != params.end(); ++i)
      {
        String::SubString name, value;
        i->read(name, value);

        if(name == QUERY_STRING)
        {
          query_string_ = value;
        }
        else if(name == REQUEST_URI)
        {
          uri_ = value;
        }
        else if(name == REQUEST_METHOD)
        {
          if(value == GET)
          {
            method_ = RM_GET;
          }
          else if(value == HEAD)
          {
            method_ = RM_GET;
            header_only_ = true;
          }
          else if(value == POST)
          {
            method_ = RM_POST;
          }
        }
        else if(name == HTTPS)
        {
          secure_ = true;
          headers_.push_back(HTTP::SubHeader(PASS_SECURE, PASS_SECURE_VALUE));
        }
        else if(name == REMOTE_ADDR)
        {
          headers_.push_back(HTTP::SubHeader(PASS_REMOTE_ADDR, value));
        }
        else if(name.size() > 5 && name.compare(0, 5, "HTTP_") == 0)
        {
          headers_.push_back(HTTP::SubHeader(name.substr(5), value));
        }
      }
    }
  }
  return PARSE_OK;
}

namespace {
  std::string CRLF("\r\n");
  std::string STATUS_200("OK");
  std::string STATUS_204("No Content");
  std::string STATUS_301("Moved Permanently");
  std::string STATUS_302("Found");
  std::string STATUS_303("See Other");
  std::string STATUS_307("Temporary Redirect");
  std::string STATUS_400("Bad Request");
  std::string STATUS_403("Forbidden");
  std::string STATUS_404("Not Found");
  std::string STATUS_500("Internal Server Error");
  const size_t STATUS_MSG_SIZE = 256;
}

HttpResponse::HttpResponse(uint16_t id, char* buf, size_t capacity) throw ()
  : status_msg_(id, buf, STATUS_MSG_SIZE),
    headers_msg_(id, buf + STATUS_MSG_SIZE, capacity / 2 - STATUS_MSG_SIZE),
    end_msg_(id, buf + capacity / 2, capacity / 2),
    output_stream_(this),
    body_size_(0),
    cookie_installed_(false)
{}

void
HttpResponse::add_header(
  const String::SubString& name,
  const String::SubString& value)
  throw (eh::Exception)
{
  headers_msg_.append(FCGI_STDOUT, String::SubString(name))
    .append(FCGI_STDOUT, HEADER_SEP)
    .append(FCGI_STDOUT, value)
    .append(FCGI_STDOUT, CRLF);
}

void
HttpResponse::set_content_type(const String::SubString& value)
  throw (eh::Exception)
{
  headers_msg_.append(FCGI_STDOUT, CONTENT_TYPE_AND_SEP)
    .append(FCGI_STDOUT, value)
    .append(FCGI_STDOUT, CRLF);
}

void
HttpResponse::add_cookie(const char* value)
  throw (eh::Exception)
{
  headers_msg_.append(FCGI_STDOUT, SET_COOKIE_AND_SEP)
    .append(FCGI_STDOUT, String::SubString(value))
    .append(FCGI_STDOUT, CRLF);

  cookie_installed_ = true;
}

OutputStream&
HttpResponse::get_output_stream() throw ()
{
  return output_stream_;
}

ssize_t
HttpResponse::write(const String::SubString& str) throw ()
{
  size_t saved_size = 0;

  while(str.size() > saved_size)
  {
    if(body_messages_.empty() ||
      body_messages_.back()->msg.size() >= body_messages_.back()->msg.capacity())
    {
      body_messages_.push_back(
        std::unique_ptr<MessageHolder>(new MessageHolder(status_msg_.id())));
    }

    tinyfcgi::message& target_body_msg = body_messages_.back()->msg;

    size_t msg_size = std::min(
      str.size() - saved_size,
      target_body_msg.capacity() - target_body_msg.size());

    target_body_msg.append(FCGI_STDOUT, str.substr(saved_size, msg_size));

    assert(target_body_msg);

    saved_size += msg_size;
  }

  body_size_ += str.size();

  return str.size();
}

size_t
HttpResponse::end_response(
  std::vector<String::SubString>& res,
  int status)
  throw ()
{
  char buf[256];
  int n = snprintf(buf, sizeof(buf), "%d ", status);
  String::SubString status_text;

  switch(status)
  {
    case 200: status_text = STATUS_200; break;
    case 204: status_text = STATUS_204; break;
    case 301: status_text = STATUS_301; break;
    case 302: status_text = STATUS_302; break;
    case 303: status_text = STATUS_303; break;
    case 307: status_text = STATUS_307; break;
    case 400: status_text = STATUS_400; break;
    case 403: status_text = STATUS_403; break;
    case 404: status_text = STATUS_404; break;
    case 500: status_text = STATUS_500; break;
    default: status_text = String::SubString(buf, n);
  }

  status_msg_.append(FCGI_STDOUT, STATUS_AND_SEP)
    .append(FCGI_STDOUT, String::SubString(buf, n))
    .append(FCGI_STDOUT, status_text)
    .append(FCGI_STDOUT, CRLF)
    .clear_padding();

  n = snprintf(buf, sizeof(buf), "%d", (int)body_size_);

  headers_msg_.append(FCGI_STDOUT, CONTENT_LENGTH_AND_SEP)
    .append(FCGI_STDOUT, String::SubString(buf, n))
    .append(FCGI_STDOUT, CRLF)
    .append(FCGI_STDOUT, CRLF) // terminate headers
    .clear_padding();

  end_msg_.end_request(0, FCGI_REQUEST_COMPLETE);

  res.reserve(3 + body_messages_.size());

  res.push_back(status_msg_.str());
  res.push_back(headers_msg_.str());

  for(auto it = body_messages_.begin(); it != body_messages_.end(); ++it)
  {
    res.push_back((*it)->msg.str());
  }

  res.push_back(end_msg_.str());

  size_t res_size = 0;
  for(auto res_it = res.begin(); res_it != res.end(); ++res_it)
  {
    res_size += res_it->size();
  }

  return res_size;
}
}
