#include "AlertLoggerMock.hpp"
#include <terrama2/core/Shared.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/impl/Utils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/data-model/DataProvider.hpp>
#include <terrama2/core/data-model/DataSeries.hpp>
#include <terrama2/core/data-model/DataSetDcp.hpp>

#include <terrama2/services/alert/core/Shared.hpp>
#include <terrama2/services/alert/core/DataManager.hpp>
#include <terrama2/services/alert/core/Alert.hpp>
#include <terrama2/services/alert/core/AlertLogger.hpp>
#include <terrama2/services/alert/core/Report.hpp>
#include <terrama2/services/alert/core/AlertExecutor.hpp>
#include <terrama2/services/alert/impl/Utils.hpp>
#include <terrama2/services/alert/core/Service.hpp>


#include <iostream>

//QT
#include <QUrl>
#include <QtGui>
#include <QTimer>

using ::testing::_;

using namespace terrama2::services::alert::core;


terrama2::core::DataProviderPtr inputDataProvider()
{
  //DataProvider information
  terrama2::core::DataProvider* dataProvider = new terrama2::core::DataProvider();
  terrama2::core::DataProviderPtr dataProviderPtr(dataProvider);
  dataProvider->name = "Local folder";
  dataProvider->uri = "file://"+ TERRAMA2_DATA_DIR+"/";
  dataProvider->intent = terrama2::core::DataProviderIntent::COLLECTOR_INTENT;
  dataProvider->dataProviderType = "POSTGIS";
  dataProvider->active = true;
  dataProvider->id = 1;

  return dataProviderPtr;
}

terrama2::core::DataSeriesPtr inputDataSeries()
{

  auto& semanticsManager = terrama2::core::SemanticsManager::getInstance();

  //DataSeries information
  terrama2::core::DataSeries* dataSeries = new terrama2::core::DataSeries();
  terrama2::core::DataSeriesPtr dataSeriesPtr(dataSeries);
  dataSeries->name = "Max Temperature";
  dataSeries->semantics = semanticsManager.getSemantics("GRID-gdal");
  dataSeries->id = 1;
  dataSeries->dataProviderId = 1;

  //DataSet information
  terrama2::core::DataSetDcp* dataSet = new terrama2::core::DataSetDcp();
  dataSet->active = true;
  dataSet->format.emplace("mask", "tmmax_obs_ams_1km_%YYYY%MM%DD.tif");
  dataSet->format.emplace("folder", "temperatura");


  dataSeries->datasetList.emplace_back(dataSet);

  return dataSeriesPtr;
}

terrama2::core::LegendPtr newLegend()
{
  auto legend = std::make_shared<terrama2::core::Risk>();
  legend->name = "Temperature levels";
  legend->id = 1;

  terrama2::core::RiskLevel level1;
  level1.level = 0;
  level1.value = 0;
  level1.name = "low";
  legend->riskLevels.push_back(level1);

  terrama2::core::RiskLevel level2;
  level2.level = 1;
  level2.value = 3;
  level2.name = "medium";
  legend->riskLevels.push_back(level2);

  terrama2::core::RiskLevel level3;
  level3.level = 2;
  level3.value = 21;
  level3.name = "high";
  legend->riskLevels.push_back(level3);

  return legend;
}

terrama2::services::alert::core::AlertPtr newAlert()
{
  auto alert = std::make_shared<terrama2::services::alert::core::Alert>();

  alert->id = 1;
  alert->projectId = 1;
  alert->riskAttribute = "0";
  alert->dataSeriesId = 1;
  alert->active = true;
  alert->name = "Example alert";
  alert->serviceInstanceId = 1;

  alert->riskId = 1;

  std::unordered_map<std::string, std::string> reportMetadata;

  reportMetadata[terrama2::services::alert::core::ReportTags::TITLE] = "GRID RISK EXAMPLE REPORT";
  reportMetadata[terrama2::services::alert::core::ReportTags::ABSTRACT] = "NumericRisk example.";
  reportMetadata[terrama2::services::alert::core::ReportTags::AUTHOR] = "TerraMA2";
  reportMetadata[terrama2::services::alert::core::ReportTags::CONTACT] = "TerraMA2 developers.";
  reportMetadata[terrama2::services::alert::core::ReportTags::COPYRIGHT] = "copyright information...";
  reportMetadata[terrama2::services::alert::core::ReportTags::DESCRIPTION] = "Example generated report...";
  reportMetadata[terrama2::services::alert::core::ReportTags::DOCUMENT_URI] = "/" + TERRAMA2_DATA_DIR + "/GridRisk.pdf";

  alert->reportMetadata = reportMetadata;

  terrama2::core::Filter filter;
  filter.lastValues = std::make_shared<size_t>(6);

  alert->filter = filter;

  Notification notification;
  notification.targets = {"vmimeteste@gmail.com"};
  notification.includeReport = "PDF";
  alert->notifications = { notification };

  return alert;
}


int main(int argc, char* argv[])
{
  QGuiApplication a(argc, argv);

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  terrama2::core::TerraMA2Init terramaRaii("example", 0);
  terrama2::core::registerFactories();
  terrama2::services::alert::core::registerFactories();

  auto dataManager = std::make_shared<terrama2::services::alert::core::DataManager>();

  dataManager->add(inputDataProvider());
  dataManager->add(inputDataSeries());
  auto alert = newAlert();
  auto legend = newLegend();
  dataManager->add(alert);
  dataManager->add(legend);

  auto logger = std::make_shared<AlertLoggerMock>();
  ::testing::DefaultValue<RegisterId>::Set(1);
  EXPECT_CALL(*logger.get(), setConnectionInfo(_)).Times(::testing::AtLeast(1));
  EXPECT_CALL(*logger.get(), start(_)).WillRepeatedly(::testing::Return(1));
  EXPECT_CALL(*logger.get(), result(_, _, _));

  logger->setConnectionInfo(te::core::URI());

  QJsonObject additionalIfo;
  additionalIfo.insert("email_server", QString("smtp://vmimeteste@gmail.com:a1a2a3a4@smtp.gmail.com:587"));

  terrama2::core::ServiceManager::getInstance().setInstanceId(1);

  terrama2::services::alert::core::Service service(dataManager);

  service.setLogger(logger);
  service.updateAdditionalInfo(additionalIfo);
  service.start();
  service.addToQueue(alert->id, terrama2::core::TimeUtils::nowUTC());

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QGuiApplication::instance(), SLOT(quit()));
  timer.start(150000);
  a.exec();

  return 0;
}
