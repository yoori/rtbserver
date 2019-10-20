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

#ifndef PLAIN_DECLARATION_BASETEMPLATE_HPP
#define PLAIN_DECLARATION_BASETEMPLATE_HPP

#include "BaseType.hpp"
#include "BaseDescriptor.hpp"

namespace Declaration
{
  class BaseTemplate: public BaseType
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(InvalidParam, Exception);

    BaseTemplate(const char* name, unsigned long args_count)
      throw();

    virtual BaseTemplate_var as_template() throw();

    unsigned long args() const throw();

    CompleteTemplateDescriptor_var
    complete_template_descriptor(
      const BaseDescriptorList& args) const
      throw(InvalidParam);

  protected:
    virtual ~BaseTemplate() throw() {}

    virtual CompleteTemplateDescriptor_var
    create_template_descriptor_(
      const char* name,
      const BaseDescriptorList& args) const
      throw(InvalidParam) = 0;

  private:
    unsigned long args_count_;
  };

  typedef ReferenceCounting::SmartPtr<BaseTemplate>
    BaseTemplate_var;
}

#endif /*PLAIN_DECLARATION_BASETEMPLATE_HPP*/
