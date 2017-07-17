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
  \file terrama2/codebase/src/unittest/core/TsDataAccessorFile.cpp

  \brief

  \author Vinicius Campanha
 */

// TerraMA2
#include "TsDataAccessorFile.hpp"
#include "TestDataAccessorFile.hpp"

// Qt
#include <QTemporaryDir>
#include <QTemporaryFile>

// GMock
#include <gtest/gtest.h>

void TsDataAccessorFile::testGetFoldersList()
{
  // Valid folders
  QTemporaryDir dir(QDir::tempPath() + QString::fromStdString("/2016"));

  QTemporaryDir subdir1(dir.path() + QString::fromStdString("/10"));
  QTemporaryDir subdir2(dir.path() + QString::fromStdString("/11"));
  QTemporaryDir subdir3(dir.path() + QString::fromStdString("/12"));

  QTemporaryDir subdir4(subdir1.path() + QString::fromStdString("/03"));
  QTemporaryDir subdir5(subdir3.path() + QString::fromStdString("/11"));

  QTemporaryDir subdir6(subdir4.path() + QString::fromStdString("/final"));
  QTemporaryDir subdir7(subdir4.path() + QString::fromStdString("/final"));

  QTemporaryDir subdir8(subdir5.path() + QString::fromStdString("/final"));

  // Invalid folders

  QTemporaryDir dir2(QDir::tempPath() + QString::fromStdString("/2016"));

  QTemporaryDir subdir9(dir2.path() + QString::fromStdString("/03"));
  QTemporaryDir subdir10(subdir9.path() + QString::fromStdString("/aa"));

  QTemporaryDir dir3(QDir::tempPath() + QString::fromStdString("/2020"));
  QTemporaryDir dir4(QDir::tempPath() + QString::fromStdString("/folder"));

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

  TestDataAccessorFile da(dataProviderPtr, dataSeriesPtr);

  std::vector<std::string> fileList;

  fileList.push_back(QDir::tempPath().toStdString());

  {
    auto foldersList = da.getFoldersList(fileList, "/%YYYY*/%MM*/%DD*/final*");

    if(foldersList.size() < 3)
      QFAIL("Wrong number of folders matched!");
  }

  {
    auto foldersList = da.getFoldersList(fileList, "%YYYY*/%MM*/%DD*/final*");

    if(foldersList.size() < 3)
      QFAIL("Wrong number of folders matched!");
  }

  // empty mask
  {
    auto foldersList = da.getFoldersList(fileList, "");

    if(foldersList.size() != 1)
      QFAIL("Wrong number of folders matched!");
  }

  // considered as empty mask
  {
    auto foldersList = da.getFoldersList(fileList, "/");

    if(foldersList.size() != 1)
      QFAIL("Wrong number of folders matched!");
  }

  // considered as empty mask
  {
    auto foldersList = da.getFoldersList(fileList, "//");

    if(foldersList.size() != 1)
      QFAIL("Wrong number of folders matched!");
  }

  // ignore the extras '/'
  {
    auto foldersList = da.getFoldersList(fileList, "%YYYY*//%MM*/%DD*//final*");

    if(foldersList.size() < 3)
      QFAIL("Wrong number of folders matched!");
  }

  // the folders doesn't match
  {
    auto foldersList = da.getFoldersList(fileList, "%YYYY*/nonexistent/%DD*");

    if(!foldersList.empty())
      QFAIL("Wrong number of folders matched!");
  }
}


void TsDataAccessorFile::testGetFilesList()
{
  // Valid folders
  QTemporaryDir dir(QDir::tempPath() + QString::fromStdString("/2016"));

  QTemporaryDir subdir1(dir.path() + QString::fromStdString("/10"));
  QTemporaryDir subdir2(dir.path() + QString::fromStdString("/11"));
  QTemporaryDir subdir3(dir.path() + QString::fromStdString("/12"));

  QTemporaryDir subdir4(subdir1.path() + QString::fromStdString("/03"));
  QTemporaryDir subdir5(subdir3.path() + QString::fromStdString("/11"));

  QTemporaryDir subdir6(subdir2.path() + QString::fromStdString("/final"));
  QTemporaryDir subdir7(subdir4.path() + QString::fromStdString("/final"));

  QTemporaryDir subdir8(subdir5.path() + QString::fromStdString("/final"));

  // Invalid folders

  QTemporaryDir subdir9(dir.path() + QString::fromStdString("/03"));
  QTemporaryDir subdir10(subdir9.path() + QString::fromStdString("/aa"));

  QTemporaryDir dir3(QDir::tempPath() + QString::fromStdString("/2020"));
  QTemporaryDir dir4(QDir::tempPath() + QString::fromStdString("/folder"));

  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);

  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);

  TestDataAccessorFile da(dataProviderPtr, dataSeriesPtr);

  std::vector<std::string> baseDirList;

  baseDirList.push_back(QDir::tempPath().toStdString());

  QTemporaryFile file1(subdir7.path() + QString::fromStdString("/file_2017_06_01_14:40_XXXXXX.file"));
  QTemporaryFile file2(subdir7.path() + QString::fromStdString("/file_2017_06_01_14:41_XXXXXX.file"));
  QTemporaryFile file3(subdir8.path() + QString::fromStdString("/file_2017_06_02_12:00_XXXXXX.file"));

  file1.open();
  file2.open();
  file3.open();

  {
    std::string fileMask = "file_%YYYY_%MM_%DD_%hh:%mm_*.file";
    std::string foldermask = "/%YYYY*/%MM*/%DD*/final*/";

    terrama2::core::Filter filter;
    std::string timezone = "03";
    auto remover = std::make_shared<terrama2::core::FileRemover>();

    QFileInfoList fileList;

    for(auto& baseDir : baseDirList)
      fileList.append(da.getFilesList(baseDir, fileMask, foldermask, filter, timezone, remover));

    if(fileList.size() < 3)
      QFAIL("Wrong number of folders matched!");
  }

}
