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

namespace AdServer
{
  namespace CampaignSvcs
  {
    // CalcParamFillerIntImpl
    template<typename IntType>
    CalculateParamsFiller::CalcParamFillerIntImpl<IntType>::CalcParamFillerIntImpl(
      IntType CTRGenerator::CalculateParams::* field)
        : field_(field)
    { }

    template<typename IntType>
    void
    CalculateParamsFiller::CalcParamFillerIntImpl<IntType>::set_value(
      CTRGenerator::CalculateParams& calc_params,
      const String::SubString& str)
    {
      if(!String::StringManip::str_to_int(str, calc_params.*field_))
      {
        Stream::Error ostr;
        ostr << "invalid feature value '" << str << "'";
        throw CTRGenerator::InvalidConfig(ostr);
      }
    }
  }
}
