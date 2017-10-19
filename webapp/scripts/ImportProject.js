"use strict";

var fs = require('fs');
var ImportProjectMember = require("./../core/ImportProject");

var configFile = process.argv[2];
var configFilePath = "";
if (!configFile){
  console.log("Invalid database configuration");
  return;
} else {
  configFilePath = "./../config/instances/" + configFile + ".json";
}

var projectFilePath = process.argv[3];
if (!projectFilePath) {
  console.log("Invalid project path");
  return;
}

var DataManager = require("./../core/DataManager");
DataManager.init(configFilePath, function(err){
  if (err){
    console.log("Error to load database. Error: " + err);
  }
  try {
    var projectJson = JSON.parse(fs.readFileSync(projectFilePath));
    ImportProjectMember(projectJson).then(function(result){
      console.log("Project successfully imported!");
    }).catch(function(result){
      console.log("Error to import the Project. Error: " + result.err);
    });
  }
  catch(err){
    console.log("Error: " + err.message);
  }
});
