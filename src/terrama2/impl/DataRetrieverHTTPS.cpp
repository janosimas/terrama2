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
  \file terrama2/impl/DataRetrieverHTTPS.cpp

  \brief Data Retriever HTTPS.

  \author Jean Souza
*/

// TerraMA2
#include "DataRetrieverHTTPS.hpp"

terrama2::core::DataRetrieverHTTPS::DataRetrieverHTTPS(DataProviderPtr dataprovider, std::unique_ptr<CurlWrapperHttp>&& curlwrapper)
  : DataRetrieverHTTP(dataprovider, std::move(curlwrapper))
{
}

terrama2::core::DataRetrieverHTTPS::~DataRetrieverHTTPS()
{
}

terrama2::core::DataRetrieverPtr terrama2::core::DataRetrieverHTTPS::make(DataProviderPtr dataProvider)
{
  std::unique_ptr<CurlWrapperHttp> curlwrapper(new CurlWrapperHttp());
  return std::make_shared<DataRetrieverHTTPS>(dataProvider, std::move(curlwrapper));
}
