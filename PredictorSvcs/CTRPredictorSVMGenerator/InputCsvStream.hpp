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

#include "CampaignFolder.hpp"
#include "FileGuard.hpp"
#include "FeatureContainer.hpp"
#include "OutputLibSvmStream.hpp"

namespace AdServer
{
  namespace Predictor
  {

    /**
     * Input PRImpression csv stream
     */
    class InputCsvStream
    {

      DECLARE_EXCEPTION(ParseError, eh::DescriptiveException);
      
    public:

      /**
       * @brief Constructor.
       *
       * @param file object
       * @param logger
       */
      InputCsvStream(
        const FileObject& file,
        Logging::Logger* logger);

      /**
       * @brief Parse csv header.
       *
       * @param[out]  feature column list
       * @return label column index
       */
      unsigned long
      parse_header_(
        std::istream& stream,
        FeatureColumns& feature_columns);
      
      /**
       * @brief Process input stream.
       *
       * @param libsvm output streams
       */
      bool process(
        OutputLibSvmStreamList& lib_svms);

      /**
       * @brief Destructor.
       */
      virtual ~InputCsvStream()
        throw();

    private:
      FileObject file_;
      Logging::Logger_var logger_;
    };

  }
}
