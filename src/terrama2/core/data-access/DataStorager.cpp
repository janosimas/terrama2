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
#include "../utility/FilterUtils.hpp"
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

std::shared_ptr< te::dt::TimeInstantTZ > terrama2::core::DataStorager::copy(DataSetPtr inputDataSet,
                                                                            std::string inputUriStr,
                                                                            DataSetPtr outputDataSet,
                                                                            const Filter& filter,
                                                                            std::shared_ptr<terrama2::core::FileRemover> remover) const
{
  if(!outputDataSet)
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  std::string mask = getMask(inputDataSet);
  if(mask.empty())
  {
    QString errMsg = QObject::tr("Empty mask for output grid.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  std::string timezone = getTimezone(inputDataSet);

  std::string outputURIStr = dataProvider_->uri;
  try
  {
    std::string folder = terrama2::core::getFolderMask(outputDataSet, nullptr);
    if (!folder.empty())
      outputURIStr += "/" + folder;
  }
  catch(...)
  {
    // nothing to be done
  }

  te::core::URI inputUri(inputUriStr);
  if(inputUri.scheme() != "file")
  {
    QString errMsg = QObject::tr("Wrong scheme.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  te::core::URI outputUri(outputURIStr);
  if(outputUri.scheme() != "file")
  {
    QString errMsg = QObject::tr("Wrong scheme.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }

  std::string outputPath = outputUri.path();
  std::string inputPath = inputUri.path();
  QFileInfoList tempFileInfoList = terrama2::core::getDataFileInfoList(inputPath,
                                                                       mask,
                                                                       timezone,
                                                                       filter,
                                                                       remover);

  boost::local_time::local_date_time noTime(boost::local_time::not_a_date_time);
  std::shared_ptr< te::dt::TimeInstantTZ > fileTimestamp;

  for(const auto& fileInfo : tempFileInfoList)
  {
    std::shared_ptr< te::dt::TimeInstantTZ > thisFileTimestamp = std::make_shared<te::dt::TimeInstantTZ>(noTime);
    // Verify if the file name matches the mask
    terrama2::core::isValidDataSetName(mask, filter, timezone, fileInfo.fileName().toStdString(), thisFileTimestamp);

    boost::filesystem::path file_folder(outputPath);
    boost::filesystem::create_directories(file_folder);
    boost::filesystem::path tempFile(inputPath+"/"+fileInfo.fileName().toStdString());
    boost::filesystem::path outputFileFile(outputPath+"/"+fileInfo.fileName().toStdString());
    boost::filesystem::copy_file(tempFile, outputFileFile);

    if(!fileTimestamp || *fileTimestamp < *thisFileTimestamp)
      fileTimestamp = thisFileTimestamp;
  }

  return fileTimestamp;
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
