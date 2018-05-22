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
  \file terrama2/services/archiver/core/Service.cpp

  \brief

  \author Jano Simas
*/

#include "Service.hpp"
#include "Archiver.hpp"
#include "ArchiverLogger.hpp"

#include "../../../core/Shared.hpp"

#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/Filter.hpp"

#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-access/DataStorager.hpp"

#include "../../../core/utility/Timer.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/DataStoragerFactory.hpp"
#include "../../../core/utility/ServiceManager.hpp"
#include "../../../core/utility/FileRemover.hpp"

#include "../../../core/Exception.hpp"

terrama2::services::archiver::core::Service::Service(std::weak_ptr<terrama2::core::DataManager> dataManager)
  : terrama2::core::Service(dataManager)
{
  connectDataManager();
}

void terrama2::services::archiver::core::Service::prepareTask(const terrama2::core::ExecutionPackage& executionPackage)
{
  try
  {
    auto dataManager = std::static_pointer_cast<terrama2::services::archiver::core::DataManager>(dataManager_.lock());
    auto archiverLogger = std::static_pointer_cast<ArchiverLogger>(logger_->clone());
    assert(archiverLogger);
    // taskQueue_.emplace(std::bind(&terrama2::services::archiver::core::Service::collect, this, executionPackage, archiverLogger, dataManager));
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

void terrama2::services::archiver::core::Service::archive(terrama2::core::ExecutionPackage executionPackage,
                                                           std::shared_ptr<ArchiverLogger> logger,
                                                           std::weak_ptr<DataManager> weakDataManager)
{
  auto dataManager = weakDataManager.lock();
  if(!dataManager.get())
  {
    TERRAMA2_LOG_ERROR() << tr("Unable to access DataManager");
    notifyWaitQueue(executionPackage.processId);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
    return;
  }

  // error message to be logged
  QString errMsg;
  try
  {
    //////////////////////////////////////////////////////////
    //  aquiring metadata
    auto lock = dataManager->getLock();

    auto archiverPtr = dataManager->findArchiver(executionPackage.processId);

    // input data
    auto dataSeries = dataManager->findDataSeries(archiverPtr->dataSeriesId);
    auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

    TERRAMA2_LOG_DEBUG() << tr("Starting arquiving process for data series '%1'").arg(dataSeries->name.c_str());

    // dataManager no longer in use
    lock.unlock();

    /////////////////////////////////////////////////////////////////////////
    //  recovering data

    auto processingStartTime = terrama2::core::TimeUtils::nowUTC();


    auto remover = std::make_shared<terrama2::core::FileRemover>();
    auto dataAccessor = terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries);

    auto status = ArchiverLogger::ProcessLogger::Status::START;

    //
    //
    //
    //
    //
    //

    TERRAMA2_LOG_INFO() << tr("Archiver %1 finished successfully.").arg(QString::fromStdString(dataSeries->name));

    auto processingEndTime = terrama2::core::TimeUtils::nowUTC();

    // log processing time
    logger->setStartProcessingTime(processingStartTime, executionPackage.registerId);
    logger->setEndProcessingTime(processingEndTime, executionPackage.registerId);

    // logger->result(status, lastDateTime, executionPackage.registerId);

    QJsonObject jsonAnswer;
    // only execute linked automatic process if finished successfully
    jsonAnswer.insert(terrama2::core::ReturnTags::AUTOMATIC, status == ArchiverLogger::ProcessLogger::Status::DONE);

    auto success = (status == ArchiverLogger::ProcessLogger::Status::DONE || status == ArchiverLogger::ProcessLogger::Status::WARNING);
    // notify process finished
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, success, jsonAnswer);
    notifyWaitQueue(executionPackage.processId);
    return;
  }
  catch(const terrama2::core::NoDataException& e)
  {
    TERRAMA2_LOG_INFO() << tr("Archiver %1 finished but there was no data available.").arg(executionPackage.processId);

    std::string errMsg = boost::get_error_info<terrama2::ErrorDescription>(e)->toStdString();
    if(executionPackage.registerId != 0)
    {
      logger->log(ArchiverLogger::MessageType::WARNING_MESSAGE, errMsg, executionPackage.registerId);
      logger->result(ArchiverLogger::Status::WARNING, nullptr, executionPackage.registerId);
    }

    QJsonObject jsonAnswer;
    jsonAnswer.insert(terrama2::core::ReturnTags::AUTOMATIC, false);
    sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, true, jsonAnswer);
    notifyWaitQueue(executionPackage.processId);
    return;
  }
  catch(const terrama2::core::LogException& e)
  {
    errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const terrama2::Exception& e)
  {
    errMsg = *boost::get_error_info<terrama2::ErrorDescription>(e);
  }
  catch(const boost::exception& e)
  {
    errMsg = QString::fromStdString(boost::diagnostic_information(e));
  }
  catch(const std::exception& e)
  {
    errMsg = e.what();
  }
  catch(...)
  {
    errMsg = tr("Unknown error.");
  }

  if(executionPackage.registerId != 0)
  {
    logger->log(ArchiverLogger::MessageType::ERROR_MESSAGE, errMsg.toStdString(), executionPackage.registerId);
    logger->result(ArchiverLogger::Status::ERROR, nullptr, executionPackage.registerId);
  }

  TERRAMA2_LOG_ERROR() << errMsg;
  TERRAMA2_LOG_INFO() << tr("Archiver %1 finished with error(s).").arg(executionPackage.processId);

  // if arrived here, always error
  sendProcessFinishedSignal(executionPackage.processId, executionPackage.executionDate, false);
  notifyWaitQueue(executionPackage.processId);
}

void terrama2::services::archiver::core::Service::connectDataManager()
{
  auto dataManager = std::static_pointer_cast<terrama2::services::archiver::core::DataManager>(dataManager_.lock());
  connect(dataManager.get(), &terrama2::services::archiver::core::DataManager::archiverAdded, this,
          &terrama2::services::archiver::core::Service::addProcessToSchedule);
  connect(dataManager.get(), &terrama2::services::archiver::core::DataManager::archiverRemoved, this,
          &terrama2::services::archiver::core::Service::removeArchiver);
  connect(dataManager.get(), &terrama2::services::archiver::core::DataManager::archiverUpdated, this,
          &terrama2::services::archiver::core::Service::updateArchiver);
}

void terrama2::services::archiver::core::Service::removeArchiver(ArchiverId archiverId) noexcept
{
  try
  {
    std::lock_guard<std::mutex> lock(mutex_);


    TERRAMA2_LOG_INFO() << tr("Removing archiver %1.").arg(archiverId);

    auto it = timers_.find(archiverId);
    if(it != timers_.end())
    {
      auto timer = timers_.at(archiverId);
      timer->disconnect();
      timers_.erase(archiverId);
    }

    // remove from queue
    processQueue_.erase(std::remove_if(processQueue_.begin(), processQueue_.end(),
                                       [archiverId](const terrama2::core::ExecutionPackage& executionPackage)
                                       { return archiverId == executionPackage.processId; }), processQueue_.end());

    waitQueue_.erase(archiverId);


    TERRAMA2_LOG_INFO() << tr("Archiver %1 removed successfully.").arg(archiverId);
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
    TERRAMA2_LOG_INFO() << tr("Could not remove archiver: %1.").arg(archiverId);
  }
  catch(const boost::exception& e)
  {
    TERRAMA2_LOG_ERROR() << boost::get_error_info<terrama2::ErrorDescription>(e);
    TERRAMA2_LOG_INFO() << tr("Could not remove archiver: %1.").arg(archiverId);
  }
  catch(...)
  {
    TERRAMA2_LOG_ERROR() << tr("Unknown error");
    TERRAMA2_LOG_INFO() << tr("Could not remove archiver: %1.").arg(archiverId);
  }
}

void terrama2::services::archiver::core::Service::updateArchiver(ArchiverPtr archiver) noexcept
{
  removeArchiver(archiver->id);
  addProcessToSchedule(archiver);
}

void terrama2::services::archiver::core::Service::updateAdditionalInfo(const QJsonObject& /*obj*/) noexcept
{

}

terrama2::core::ProcessPtr terrama2::services::archiver::core::Service::getProcess(ProcessId processId)
{
  auto dataManager = std::static_pointer_cast<terrama2::services::archiver::core::DataManager>(dataManager_.lock());
  return dataManager->findArchiver(processId);
}
