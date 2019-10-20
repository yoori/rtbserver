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


#pragma once

#include <memory>
#include <iostream>
#include <vector>

#include <Stream/BinaryStream.hpp>
#include <String/SubString.hpp>
#include <String/StringManip.hpp>

#include <HTTP/HttpMisc.hpp>

namespace tinyfcgi
{
  typedef String::SubString string_ref;
}

#include "tinyfcgi/tinyfcgi.hpp"

namespace FCGI
{
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  class InputStream: public Stream::BinaryInputStream
  {
  public:
    InputStream() throw ();

    InputStream(const String::SubString& buf) throw ();

    void
    set_buf(const String::SubString& buf) throw ();

    virtual Stream::BinaryInputStream&
    read(char_type* s, streamsize n) throw (eh::Exception);

    void
    has_body(bool val) throw ();

  private:
    String::SubString buf_;
    mutable size_t pos_;
  };

  class HttpResponse;

  class OutputStream: public Stream::BinaryOutputStream
  {
  public:
    OutputStream(HttpResponse* owner) throw ();

    virtual
    Stream::BinaryOutputStream&
    write(const char_type* s, streamsize n) throw (eh::Exception);

  private:
    HttpResponse* owner_;
  };

  enum ParseRes
  {
    PARSE_OK,
    PARSE_NEED_MORE,
    PARSE_INVALID_HEADER,
    PARSE_BEGIN_REQUEST_EXPECTED,
    PARSE_INVALID_ID,
    PARSE_FRAGMENTED_STDIN
  };

  class HttpRequest
  {
  public:
    enum Method
    {
      RM_GET,
      RM_POST
    };

    class Exception : public FCGI::Exception
    {
    public:
      template <typename T>
      explicit
      Exception(const T& description, int error_code = 0)
        throw ();

      virtual
      ~Exception() throw ();

      int
      error_code() const throw ();

    protected:
      Exception() throw ();

    protected:
      int error_code_;
    };

  public:
    static void
    parse_params(const String::SubString& str, HTTP::ParamList& params)
      throw (String::StringManip::InvalidFormatException, eh::Exception);

  public:
    HttpRequest() throw ()
      : method_(RM_GET),
        header_only_(false),
        secure_(false)
    {}

    Method
    method() const throw () { return method_; }

    const String::SubString&
    uri() const throw () { return uri_; }

    const String::SubString&
    args() const throw () { return query_string_; }

    const HTTP::ParamList&
    params() const throw () { return params_; }

    const HTTP::SubHeaderList&
    headers() const throw () { return headers_; }

    const String::SubString&
    body() const throw () { return body_; }

    InputStream&
    get_input_stream() const throw () { return input_stream_; }

    bool
    secure() const throw() { return secure_; }

    const String::SubString&
    server_name() const throw() { return server_name_; }

    void
    set_params(HTTP::ParamList&& params) throw () { params_ = std::move(params); }

    /**
     * HEAD request, as opposed to GET
     * @return true if HEAD request
     */
    bool
    header_only() const throw () { return header_only_; }

  public:
    ParseRes
    parse(char* buf, size_t size);

  private:
    Method method_;
    String::SubString body_;
    String::SubString uri_;
    String::SubString server_name_;
    String::SubString query_string_;
    HTTP::ParamList params_;
    HTTP::SubHeaderList headers_;
    mutable InputStream input_stream_;
    bool header_only_;
    bool secure_;
  };

  class HttpResponse
  {
  public:
    HttpResponse(uint16_t id, char* buf, size_t capacity) throw ();

    void
    add_header(
      const String::SubString& name,
      const String::SubString& value)
      throw (eh::Exception);

    void
    set_content_type(const String::SubString& value)
      throw (eh::Exception);

    void
    add_cookie(const char* value)
      throw (eh::Exception);

    OutputStream&
    get_output_stream() throw ();

    ssize_t
    write(const String::SubString& str) throw ();

    size_t
    end_response(
      std::vector<String::SubString>& res,
      int status) throw ();

    bool
    cookie_installed() const throw();

  private:
    struct MessageHolder
    {
      MessageHolder(uint16_t id)
        : buf(MAX_SIZE),
          msg(id, buf.data(), MAX_SIZE)
      {};

      static const size_t MAX_SIZE = 64 * 1024 - 1;
      std::vector<char> buf;
      tinyfcgi::message msg;
    };

  private:
    tinyfcgi::message status_msg_;
    tinyfcgi::message headers_msg_;
    std::vector<std::unique_ptr<MessageHolder> > body_messages_;
    tinyfcgi::message end_msg_;
    OutputStream output_stream_;
    size_t body_size_;
    bool cookie_installed_;
  };
}

namespace FCGI
{
  inline bool
  HttpResponse::cookie_installed() const throw()
  {
    return cookie_installed_;
  }
}
