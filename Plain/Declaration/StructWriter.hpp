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

#ifndef _STRUCTWRITER_HPP_
#define _STRUCTWRITER_HPP_

#include <list>
#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Stream/MemoryStream.hpp>

#include "BaseWriter.hpp"
#include "StructDescriptor.hpp"

namespace Declaration
{
  class StructWriter:
    public virtual ReferenceCounting::AtomicImpl,
    public BaseWriter
  {
  public:
    class FieldWriter: public ReferenceCounting::AtomicImpl
    {
    public:
      FieldWriter(
        StructDescriptor::Field* field_val,
        BaseWriter* writer_val,
        const MappingSpecifierSet& mapping_specifiers)
        throw();

      const char* name() const throw();
      
      StructDescriptor::Field_var descriptor_field() const throw();

      BaseWriter_var writer() const throw();

      BaseDescriptor_var descriptor() throw();

      const MappingSpecifierSet&
      mapping_specifiers() const throw();

    protected:
      virtual ~FieldWriter() throw()
      {}

    private:
      StructDescriptor::Field_var field_;
      BaseWriter_var writer_;
      const MappingSpecifierSet mapping_specifiers_;
    };

    typedef ReferenceCounting::SmartPtr<FieldWriter>
      FieldWriter_var;

    class FieldWriterList:
      public std::list<FieldWriter_var>,
      public ReferenceCounting::DefaultImpl<>
    {
    protected:
      virtual ~FieldWriterList() throw() {}
    };
    
    typedef ReferenceCounting::SmartPtr<FieldWriterList>
      FieldWriterList_var;

  public:
    StructWriter(
      const char* name_val,
      BaseDescriptor* descriptor_val,
      FieldWriterList* fields_val);

    FieldWriterList_var fields() const;

    virtual BaseDescriptor_var descriptor() throw();

    virtual StructWriter_var as_struct_writer() throw();

    virtual void
    check_mapping_specifiers(
      const Declaration::MappingSpecifierSet& mapping_specifiers)
      throw(InvalidMappingSpecifier);

  protected:
    virtual ~StructWriter() throw() {}
    
  private:
    BaseDescriptor_var descriptor_;
    FieldWriterList_var fields_;
  };

  typedef ReferenceCounting::SmartPtr<StructWriter>
    StructWriter_var;
}

namespace Declaration
{
  // StructWriter::FieldWriter
  inline
  StructWriter::FieldWriter::FieldWriter(
    StructDescriptor::Field* field_val,
    BaseWriter* writer_val,
    const MappingSpecifierSet& mapping_specifiers)
    throw()
    : field_(ReferenceCounting::add_ref(field_val)),
      writer_(ReferenceCounting::add_ref(writer_val)),
      mapping_specifiers_(mapping_specifiers)
  {}

  inline
  const char*
  StructWriter::FieldWriter::name() const throw()
  {
    return field_->name();
  }

  inline
  StructDescriptor::Field_var
  StructWriter::FieldWriter::descriptor_field() const throw()
  {
    return field_;
  }

  inline
  BaseWriter_var
  StructWriter::FieldWriter::writer() const throw()
  {
    return writer_;
  }

  inline
  BaseDescriptor_var
  StructWriter::FieldWriter::descriptor() throw()
  {
    return field_->descriptor();
  }

  inline
  const MappingSpecifierSet&
  StructWriter::FieldWriter::mapping_specifiers() const throw()
  {
    return mapping_specifiers_;
  }

  // StructWriter
  inline
  StructWriter::StructWriter(
    const char* name_val,
    BaseDescriptor* descriptor_val,
    StructWriter::FieldWriterList* fields_val)
    : BaseType(name_val),
      BaseWriter(name_val),
      descriptor_(ReferenceCounting::add_ref(descriptor_val)),
      fields_(ReferenceCounting::add_ref(fields_val))
  {}

  inline
  BaseDescriptor_var
  StructWriter::descriptor() throw()
  {
    return descriptor_;
  }

  inline
  StructWriter::FieldWriterList_var
  StructWriter::fields() const
  {
    return fields_;
  }

  inline
  StructWriter_var
  StructWriter::as_struct_writer() throw()
  {
    return ReferenceCounting::add_ref(this);
  }

  inline
  void
  StructWriter::check_mapping_specifiers(
    const Declaration::MappingSpecifierSet& mapping_specifiers)
    throw(BaseWriter::InvalidMappingSpecifier)
  {
    if(!mapping_specifiers.empty())
    {
      // no allowed specifiers
      Stream::Error ostr;
      ostr << "Struct writer don't allow specifiers";
      throw InvalidMappingSpecifier(ostr);
    }
  }
}

#endif /*_STRUCTWRITER_HPP_*/
