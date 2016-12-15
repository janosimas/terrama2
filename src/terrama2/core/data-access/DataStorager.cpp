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
  \file terrama2/core/data-access/DataStorager.cpp

  \brief

  \author Jano Simas
  \author Vinicius Campanha
 */

//TerraMA2
#include "DataStorager.hpp"
#include "../utility/Logger.hpp"
#include "../utility/Utils.hpp"
#include "../data-model/DataProvider.hpp"
#include "../Exception.hpp"
#include "../../Config.hpp"

#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/core/uri/URI.h>

#include <boost/filesystem.hpp>

terrama2::core::DataStorager::DataStorager(DataProviderPtr outputDataProvider)
        : dataProvider_(outputDataProvider)
{
  if(!dataProvider_.get())
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataStorager::getMask(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("mask");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined mask in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataStorager::getTimezone(DataSetPtr dataSet, bool logError) const
{
  try
  {
    return dataSet->format.at("timezone");
  }
  catch(...)
  {
    QString errMsg = QObject::tr("Undefined timezone in dataset: %1.").arg(dataSet->id);
    if(logError)
      TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}
