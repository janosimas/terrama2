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
  \file terrama2/impl/DataRetrieverHTTP.hpp

  \brief Data Retriever HTTP.

 \author Jean Souza
*/


#ifndef __TERRAMA2_IMPL_DATARETRIEVERHTTP_HPP__
#define __TERRAMA2_IMPL_DATARETRIEVERHTTP_HPP__

// STL
#include <memory>
#include <cassert>

// TerraLib
#include <terralib/core/uri/URI.h>

// TerraMA2
#include "../core/utility/Raii.hpp"
#include "../core/data-access/DataRetriever.hpp"
#include "../core/Shared.hpp"
#include "../core/utility/CurlWrapperHttp.hpp"

// LibCurl
#include <curl/curl.h>

// Qt
#include <QTemporaryDir>

namespace terrama2
{
  namespace core
  {
    /*!
       * \brief The DataRetrieverHTTP class performs the download of
       * occurrences of files, DCP-TOA5, DCP-INPE, GRADES ETA15km.
       *
       * \warning The DataRetrieverHTTP class only performs the download of files, but does not
       * perform the removal of the files after downloading.
    */
    class DataRetrieverHTTP: public DataRetriever
    {
      public:
        /*!
         * \brief DataRetrieverHTTP Constructor:
         * Initializes the Curl and check the URL to download.
         * Initializes scheme information. Ex. "file://".
         * Initializes temporaFolder Folder information where the files will be saved. Ex. "/tmp/".
         * Create the directory where you will download the files. Ex. "/tmp/terrama2-download/".
         * \param dataprovider dataprovider Dataprovider information.
         * \exception DataRetrieverException when HTTP address is invalid.
         * \exception DataRetreiverHTTPException when unknown Error, HTTP address is invalid.
        */
        explicit DataRetrieverHTTP(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperHttp>&& curlwrapper);

        /*!
         * \brief DataRetrieverHTTP Default Destructor.
         *
         */
        virtual ~DataRetrieverHTTP();

        //comments on parent
        virtual bool isRetrivable() const noexcept override;

        /*!
         * \brief retrieveData Retrieving remote data from HTTP servers.
         * \param mask Mask to the data files.
         * \param filter Filter to the data files.
         * \return Returns the absolute path of the folder that contains the files that have been made the download.
         * \exception DataRetrieverException when could not perform the download files.
         * \exception DataRetrieverException when Unknown error, Could not perform the download files.
        */
        virtual std::string retrieveData(const std::string& mask, 
                                         const Filter& filter,
                                         const std::string& timezone,
                                         std::shared_ptr<terrama2::core::FileRemover> remover,
                                         const std::string& temporaryFolder = "",
                                         const std::string& foldersMask = "") override;

        static DataRetrieverPtr make(DataProviderPtr dataProvider);
        static DataRetrieverType dataRetrieverType() { return "HTTP"; }

        std::vector<std::string> listFiles(const std::string& uri);

      private:
        std::unique_ptr<CurlWrapperHttp> curlwrapper_; //!< Curl handler.
    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;//!< Shared pointer to a DataRetriever.
  }
}

#endif //__TERRAMA2_IMPL_DATARETRIEVERHTTP_HPP__
