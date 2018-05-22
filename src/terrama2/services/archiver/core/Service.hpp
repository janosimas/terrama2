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
  \file terrama2/services/archiver/core/Service.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ARCHIVER_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_ARCHIVER_CORE_SERVICE_HPP__

// TerraMa2
#include "Config.hpp"
#include "../../../core/utility/Service.hpp"
#include "../../../core/Typedef.hpp"
#include "../../../core/Shared.hpp"
#include "Shared.hpp"
#include "Typedef.hpp"
#include "DataManager.hpp"
#include "ArchiverLogger.hpp"

// STL
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace archiver
    {
      namespace core
      {
        /*!
          \brief The %Archiver Service provides thread and time management for Archiver processes.

          This class is used to manage thread sync and timer listening to access
          and achive the data.

          The %Archiver Service has a main thread that will check for new data to archive
          and a threadpool that will be allocated to actively archive the data.
        */
        class TMARCHIVEREXPORT Service : public terrama2::core::Service
        {
            Q_OBJECT

          public:
            Service(std::weak_ptr<terrama2::core::DataManager> dataManager);

            ~Service() = default;
            Service(const Service& other) = delete;
            Service(Service&& other) = default;
            Service& operator=(const Service& other) = delete;
            Service& operator=(Service&& other) = default;

          public slots:

            /*!
              \brief Updates the Archiver.

              calls addArchiver()
            */
            void updateArchiver(ArchiverPtr archiver) noexcept;
            /*!
              \brief Removes the Archiver.

              Rennuning processes will continue until finished.
            */
            void removeArchiver(ArchiverId archiverId) noexcept;

            /*!
             * \brief Receive a jSon and update service information with it
             * \param obj jSon with additional information for service
             */
            virtual void updateAdditionalInfo(const QJsonObject& obj) noexcept override;

          protected:
            virtual terrama2::core::ProcessPtr getProcess(ProcessId processId) override;
            //*! Create a process task and add to taskQueue_
            virtual void prepareTask(const terrama2::core::ExecutionPackage& executionPackage) override;
            /*!
              \brief Callback method to archive data.
            */
            void archive(terrama2::core::ExecutionPackage executionPackage, std::shared_ptr<ArchiverLogger> logger,
                         std::weak_ptr<DataManager> weakDataManager);

            //! Connects signals from DataManager
            void connectDataManager();
        };

      } // end namespace core
    }   // end namespace archiver
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ARCHIVER_CORE_SERVICE_HPP__
