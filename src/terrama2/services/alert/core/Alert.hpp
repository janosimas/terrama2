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
  \file terrama2/services/alert/core/Alert.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_ALERT_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_ALERT_HPP__

// TerraMA2
#include "Typedef.hpp"
#include "../../../core/data-model/Risk.hpp"
#include "../../../core/data-model/Schedule.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/data-model/Process.hpp"

//STL
#include <unordered_map>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        const std::string COMPARISON_PROPERTY_NAME = "comparison";
        //! Additional dataseries for alert process.
        struct AdditionalData
        {
          DataSeriesId dataSeriesId = terrama2::core::InvalidId();//!< Identifier of the additional dataseries
          DataSetId dataSetId = terrama2::core::InvalidId();//!< Identifier of the dataset referred
          std::string referrerAttribute;
          std::string referredAttribute;
          std::vector<std::string> attributes;//!< attributes that will be copied to the alert
          std::map<std::string, std::string> alias;//!< attributes alias
        };

        //! Notification rules and targets .
        struct Notification
        {
          uint32_t notifyOnRiskLevel = 0; //!< Targets should be notified if the greatest risk level is greater then notifyOnRiskLevel.
          bool notifyOnChange = true; //!< Targets should be notified if the risk level changed in any valeu.
          std::string includeReport; //!< Include report on notification.
          bool simplifiedReport = true; //!< Include simplified or complete report.
          std::vector<std::string> targets; //!< List of targets that should be notified.
        };

        /*!
         \brief Struct with information for an Alert
        */
        struct Alert : public terrama2::core::Process
        {
          AlertId id = 0; //!< Alert identifier
          ProjectId projectId = 0; //!< Project identifier.
          bool active = true;//!< Flag if the alert is active.
          std::string name; //!< Name of the alert.
          std::string description; //!< Short description of the purpose of the alert.
          DataSeriesId dataSeriesId = 0; //!< The DataSeries that will be used for risk analysis.
          ServiceInstanceId serviceInstanceId; //!< Identifier of the service instance that should run the alert.

          std::string riskAttribute;//!< Attribute of the DataSeries that will be used for risk analysis.
          LegendId riskId;//!< Risk rule of the alert
          terrama2::core::Filter filter;//!< Information on how input data should be filtered before the alert is created.

          std::vector<AdditionalData> additionalDataVector;//!< Vector of additional DataSeries and attributes that should be included in the result.

          std::unordered_map<std::string, std::string> reportMetadata;//!< Metadata used to create a report.

          std::vector<Notification> notifications;
        };
      } /* core */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_CORE_ALERT_HPP__
