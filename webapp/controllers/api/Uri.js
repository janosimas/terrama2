var RequestFactory = require("../../core/RequestFactory");
var handleRequestError = require("./../../core/Utils").handleRequestError;

module.exports = function(app) {
  return {
    post: function(request, response) {
      var obj = request.body;

      try {
        // requesting for an object to check connection
        var factoryResult = RequestFactory.build(obj);

        factoryResult.request().then(function(data) {
          return response.json({status: 200, data: data});
        }).catch(function(err) {
          handleRequestError(response, err, 400);
        });
      } catch(err) {
        handleRequestError(response, err, 400);
      }
    },
    //get request
    get: function(request, response) {
      var obj = request.query;

      try {
        var factoryResult = RequestFactory.build(obj);
        factoryResult.get().then(function(data){
          return response.json({status: 200, data: data});
        }).catch(function(err){
          handleRequestError(response, err, 400);
        });
      } catch(err) {
        handleRequestError(response, err, 400);
      }
    }
  }
};
