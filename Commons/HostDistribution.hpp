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


#ifndef HOST_DISTRIBUTION_FILE_HPP_
#define HOST_DISTRIBUTION_FILE_HPP_

#include <string>
#include <map>
#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Stream/MemoryStream.hpp>

namespace AdServer
{
namespace Commons
{

  /**
   * Class for finding host by index, defined in configuration file
   * */
  class HostDistributionFile: public ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(InvalidFile, eh::DescriptiveException);
    DECLARE_EXCEPTION(InvalidIndex, InvalidFile);
    DECLARE_EXCEPTION(FileNotFound, InvalidFile);

    typedef std::map<unsigned long, std::string> IndexToHostMap;

    /**
     * @param file configuration file
     * */ 
    HostDistributionFile(const char* file, const char* schema_file)
      throw(FileNotFound, InvalidFile);

    /** 
     * Returns hostname by index value
     * @param index
     * @return host name for index
     * */
    const std::string& get_host_by_index(unsigned long index) const
      throw(InvalidIndex);

    /** 
     * Returns maximum index
     * @return maximum index + 1
     * */
    unsigned long get_index_limit() const throw();

    /** 
     * Returns Map of indexes to host
     * @return Map of indexes
     * */
    const IndexToHostMap& all_indexes() const;

  protected:
    virtual ~HostDistributionFile() throw()
    {}

  private:
    IndexToHostMap host_map_;
  };

  typedef ReferenceCounting::SmartPtr<HostDistributionFile>
    HostDistributionFile_var;
}
}

namespace AdServer
{
namespace Commons
{
  inline
  const std::string& HostDistributionFile::get_host_by_index(
    unsigned long index) const
    throw(InvalidIndex)
  {
    IndexToHostMap::const_iterator fnd = host_map_.find(index);
    if(fnd == host_map_.end())
    {
      Stream::Error err;
      err << "Invalid index " << index;
      throw InvalidIndex(err);
    }
    return fnd->second;
  }

  inline
  unsigned long HostDistributionFile::get_index_limit() const throw()
  {
    return host_map_.rbegin()->first + 1;
  }

  inline
  const HostDistributionFile::IndexToHostMap&
  HostDistributionFile::all_indexes() const
  {
    return host_map_;
  }

}
}

#endif
