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
  \file src/terrama2/services/archiver/core/JSonUtils.cpp

  \brief

  \author Jano Simas
*/

#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::services::archiver::core::ArchiverPtr terrama2::services::archiver::core::fromArchiverJson(QJsonObject json, terrama2::core::DataManager* dataManager)
{
  if(json["class"].toString() != "Archiver")
  {
    QString errMsg = QObject::tr("Invalid Archiver JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
      && json.contains("project_id")
      && json.contains("service_instance_id")
      && json.contains("active")
      && json.contains("schedule")
      && json.contains("dataSeries_id")
      && json.contains("time_filter")))
  {
    QString errMsg = QObject::tr("Invalid Archiver JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  auto archiver = std::make_shared<terrama2::services::archiver::core::Archiver>();

  archiver->id = static_cast<uint32_t>(json["id"].toInt());
  archiver->projectId = static_cast<uint32_t>(json["project_id"].toInt());
  archiver->serviceInstanceId = static_cast<uint32_t>(json["service_instance_id"].toInt());
  archiver->active = json["active"].toBool();

  if(json.contains("schedule") && !json["schedule"].isNull())
    archiver->schedule = terrama2::core::fromScheduleJson(json["schedule"].toObject());

  archiver->timeFilter = json["time_filter"].toString().toStdString();
  archiver->dataSeriesId =  static_cast<uint32_t>(json["dataSeries_id"].toInt());

  if(json.contains("schedule") && !json["schedule"].isNull()) {
    auto output = json["schedule"].toObject();
    archiver->output.format = output["format"].toString().toStdString();
    archiver->output.uri = te::core::URI(output["uri"].toString().toStdString());
  }

  return archiver;
}

QJsonObject terrama2::services::archiver::core::toJson(ArchiverPtr archiver)
{
  QJsonObject obj;
  assert(0);

  return obj;
}
