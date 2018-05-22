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

#ifndef __TERRAMA2_SERVICES_ARCHIVER_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_ARCHIVER_CORE_DATAMANAGER_HPP__

// TerraMa2
#include "Config.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../core/Typedef.hpp"
#include "../core/Shared.hpp"

namespace terrama2
{
  namespace services
  {
    namespace archiver
    {
      namespace core
      {
        /*!
          \brief Manages metadata of terrama2::core::DataProvider, terrama2::core::DataSeries and Archiver.

          Extends terrama2::core::DataManager by managing Archiver.

          \sa terrama2::core::DataManager
        */
        class TMARCHIVEREXPORT DataManager : public terrama2::core::DataManager
        {
            Q_OBJECT

          public:
            //! Default constructor
            DataManager() = default;

            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Archiver to be added.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "archivers"
            */
            virtual void addJSon(const QJsonObject& obj) override;
            /*!
              \brief Parsers the QJsonObject for terrama2::core::DataProvider, terrama2::core::DataSeries and Archiver to be removed.

              The valid tags are:
                - "dataproviders"
                - "dataseries"
                - "archivers"
            */
            virtual void removeJSon(const QJsonObject& obj) override;

            //! Default destructor
            virtual ~DataManager() = default;
            DataManager(const DataManager& other) = delete;
            DataManager(DataManager&& other) = delete;
            DataManager& operator=(const DataManager& other) = delete;
            DataManager& operator=(DataManager&& other) = delete;

            using terrama2::core::DataManager::add;
            using terrama2::core::DataManager::update;

            /*!
              \brief Register a Archiver in the manager.

              At end it will emit archiverAdded(ArchiverPtr) signal.

              \param archiver The Archiver to be registered into the manager.

              \pre The Archiver must not have a terrama2::core::InvalidId.
              \pre A Archiver with the same name must not be already in the manager.

              \exception terrama2::InvalidArgumentException If it is not possible to add the Archiver.

              \note Thread-safe.
            */
            virtual void add(ArchiverPtr archiver);
            /*!
              \brief Update a given Archiver.

              Emits archiverUpdated() signal if the Archiver is updated successfully.

              \param dataseries     Archiver to update.
              \param shallowSave If true it will update only the dataseries attributes.

              \pre The Archiver must not have a terrama2::core::InvalidId.
              \pre The Archiver must exist in the DataManager.

              \exception terrama2::InvalidArgumentException If it is not possible to update the Archiver.

              \note Thread-safe.
            */
            virtual void update(ArchiverPtr archiver);
            /*!
              \brief Removes the Archiver with the given id.

              Emits archiverRemoved() signal if the DataSeries is removed successfully.

              \param id ID of the Archiver to remove.

              \exception terrama2::InvalidArgumentException If it is not possible to remove the Archiver.

              \note Thread-safe.
            */
            virtual void removeArchiver(ArchiverId archiverId);
            /*!
              \brief Retrieves the Archiver with the given ArchiverId.

              \param id The Archiver ArchiverId.

              \return DataProviderPtr A smart pointer to the Archiver

              \exception terrama2::InvalidArgumentException If some error occur when trying to find the Archiver.

              \note Thread-safe.
            */
            virtual ArchiverPtr findArchiver(ArchiverId id) const;

            //! Verify if the Archiver already exists in the DataManager
            virtual bool hasArchiver(ArchiverId id) const;

          signals:
            //! Signal to notify that a Archiver has been added.
            void archiverAdded(ArchiverPtr);
            //! Signal to notify that a Archiver has been updated.
            void archiverUpdated(ArchiverPtr);
            //! Signal to notify that a Archiver has been removed.
            void archiverRemoved(ArchiverId);

          protected:
            std::map<ArchiverId, ArchiverPtr> archivers_;//!< A map from ArchiverId to Archiver.
        };
      } // end namespace core
    }   // end namespace archiver
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_ARCHIVER_CORE_DATAMANAGER_HPP__
