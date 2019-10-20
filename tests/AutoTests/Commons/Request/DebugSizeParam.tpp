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


namespace AutoTest
{
  template<typename Request>
  template<class T>
  DebugSizeParam<Request>::DebugSizeParam(
    Request* request,
    const char* name,
    const T& defs,
    bool set_defs): Base(request, name, defs, set_defs)
  {}

  template<typename Request>
  template<class T>
  DebugSizeParam<Request>&
  DebugSizeParam<Request>::operator= (
    const T& val)
  {
    Base::operator=(val);
    return *this;
  }

  template<typename Request>
  DebugSizeParam<Request>::DebugSizeParam(
    Request* request,
    const char* name) : Base(request, name)
  {}

  template<typename Request>
  DebugSizeParam<Request>::DebugSizeParam(
    Request* request,
    const DebugSizeParam& other):
    Base(request, static_cast<const Base&>(other))
  {}

  template<typename Request>
  DebugSizeParam<Request>::~DebugSizeParam() throw()
  {}

  template<typename Request>
  bool DebugSizeParam<Request>::print(
    std::ostream& out,
    const char* prefix,
    const char* eql) const
  {
    out << prefix;
    if (!Base::param_value_.empty())
    {
      out << Base::name_ << eql << Base::str();
    }

    typename Base::ParamsMap::const_iterator it = Base::parameters_.begin();
    for(;it != Base::parameters_.end(); ++it)
    {
      if (it != Base::parameters_.begin() || !Base::param_value_.empty())
      {
        out << static_cast<Request*>(Base::request_)->params_prefix();
      }

      out << "debug.adslot" << it->first << ".size" << eql << it->second->str();
    }
    return true;
  }
}

