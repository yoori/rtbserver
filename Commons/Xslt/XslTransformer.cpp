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

// @file Xslt/XslTransformer.cpp

#include "XslTransformer.hpp"

#include "LibxsltTransformer.hpp"

namespace AdServer
{

  //
  // XslTransformer class implementation
  //

  XslTransformer::XslTransformer(XsltEngine /*engine*/) throw ()
    : pimpl_(
      static_cast<XslTransformerBase*>(new LibxslTransformer))
  {
  }

  XslTransformer::XslTransformer(std::istream& xsl,
    const char* base_path,
    XsltEngine /*engine*/)
    throw (Exception)
    : pimpl_(static_cast<XslTransformerBase*>(
        new LibxslTransformer(xsl, base_path)))
  {
  }

  XslTransformer::XslTransformer(const char* xsl_file,
    const char* base_path,
    XsltEngine /*engine*/)
    throw (FileNotExists, Exception)
    : pimpl_(static_cast<XslTransformerBase*>(
        new LibxslTransformer(xsl_file, base_path)))
  {
  }

  void
  XslTransformer::open(const char* xsl_file,
    const char* base_path)
    throw (FileNotExists, Exception)
  {
    pimpl_->open(xsl_file, base_path);
  }

  void
  XslTransformer::transform(std::istream& input,
    std::ostream& output,
    const XslParameters* parameters)
    throw (Exception)
  {
    pimpl_->transform(input, output, parameters);
  }

  void
  XslTransformer::register_external_fun(
    const char* fun_namespace,
    const char* fun_name,
    XslFunction* fun) throw (Exception)
  {
    pimpl_->register_external_fun(fun_namespace, fun_name, fun);
  }

} // namespace AdServer
