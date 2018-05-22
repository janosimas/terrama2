/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/services/archiver/core/Archiver.hpp

  \brief Model class for the archiver configuration.

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ARCHIVER_CORE_ARCHIVER_HPP__
#define __TERRAMA2_SERVICES_ARCHIVER_CORE_ARCHIVER_HPP__

#include "../../../core/data-model/Process.hpp"
#include "../../../core/Typedef.hpp"
#include "../core/Typedef.hpp"
#include "../core/Shared.hpp"

// STL
#include <string>

#include <terralib/core/uri/URI.h>

namespace terrama2
{
  namespace services
  {
    namespace archiver
    {
      namespace core
      {
        struct OutputFormat
        {
          std::string format;
          te::core::URI uri;

          operator bool() const { return !format.empty(); }
        };

        /*!
          \brief The Archiver groups the information to acquire data from a source server and store at a destination.
        */
        struct Archiver : public terrama2::core::Process
        {
          DataSeriesId dataSeriesId =  terrama2::core::InvalidId();//!< DataSeries source of the data.
          std::string timeFilter;
          OutputFormat output;
        };

      } // end namespace core
    }   // end namespace archiver
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ARCHIVER_CORE_ARCHIVER_HPP__
