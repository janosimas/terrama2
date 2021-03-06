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
  \file terrama2/services/analysis/core/python/PythonBindingDCP.hpp

  \brief register dcp analysis binding

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHON_DCP_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHON_DCP_HPP__

// TerraMa2
#include "../Config.hpp"

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace python
        {
          namespace DCP
          {
            /*!
              \brief Registers all functions for monitored object analysis.
            */
            TMANALYSISEXPORT void registerFunctions();

            TMANALYSISEXPORT void registerDCPFunctions();

            /*!
              \brief Registers DCP history functions in the Python interpreter.
            */
            TMANALYSISEXPORT void registerDCPHistoryFunctions();

            /*!
              \brief Registers DCP history interval functions in the Python interpreter.
            */
            TMANALYSISEXPORT void registerDCPHistoryIntervalFunctions();

            /*!
              \brief Registers DCP influence functions in the Python interpreter.
            */
            TMANALYSISEXPORT void registerDCPInfluenceFunctions();

          } /* MonitoredObject */
        } /* python */
      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_PYTHON_DCP_HPP__
