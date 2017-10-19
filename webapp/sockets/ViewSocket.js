(function() {
  'use strict';

  /**
   * Socket responsible for delivering views real time data.
   * @class ViewSocket
   *
   * @author Raphael Willian da Costa [raphael.costa@dpi.inpe.br]
   *
   * @param {}
   * @property {Object} memberIoSocket - Sockets IO object.
   * @property {TcpService} memberTcpService - TerraMA² Tcp Manager.
   * @property {ViewsCache} memberViewsCache - TerraMA² Views Cache.
   */
  var ViewSocket = function(io) {

    // Sockets object
    var memberIoSocket = io.sockets;
    // TerraMA² Tcp Manager
    var memberTcpService = require("./../core/facade/tcp-manager/TcpService");
    // TerraMA² Views Cache
    var memberViewsCache = require("./../core/ViewsCache");

    /**
     * Defines a registered view listener. Triggered when C++ services done view execution and TerraMA² WebApp re-save in database
     * 
     * @param {RegisteredView} registeredView - TerraMA² RegisteredView
     */
    memberTcpService.on("viewReceived", function(registeredView) {
      var registeredViewObject = registeredView.toObject();

      for(var key in memberIoSocket.sockets)
        memberViewsCache.addViewToViewCache(memberViewsCache.TYPES.NEW_AND_UPDATED, memberIoSocket.sockets[key].client.id, registeredViewObject);

      memberIoSocket.emit("viewReceived");
    });

    /**
     * Defines an alert notify. Triggered when C++ services done alert execution and TerraMA² WebApp send info to WebMonitor
     * 
     * @param {Object} viewInfo - View info to notify
     */
    memberTcpService.on("notifyView", function(viewInfo) {
      for(var key in memberIoSocket.sockets)
        memberViewsCache.addViewToViewCache(memberViewsCache.TYPES.NOTIFIED, memberIoSocket.sockets[key].client.id, viewInfo);

      memberIoSocket.emit("notifyView");
    });

    /**
     * Defines a remove notify. Triggered when remove a view in WebApp
     * 
     * @param {Object} viewInfo - View info to remove
     */
    memberTcpService.on("removeView", function(viewInfo) {
      for(var key in memberIoSocket.sockets)
        memberViewsCache.addViewToViewCache(memberViewsCache.TYPES.REMOVED, memberIoSocket.sockets[key].client.id, viewInfo);

      memberIoSocket.emit("removeView");
    });

    /**
     * Defines a project listener. Triggered when a project is added or updated.
     * 
     * @param {Object} project - Project object
     */
    memberTcpService.on("projectReceived", function(project) {
      memberIoSocket.emit("projectReceived", project);
    });

    /**
     * Defines a project removal listener. Triggered when a project is removed.
     * 
     * @param {Object} project - Project object
     */
    memberTcpService.on("projectDeleted", function(project) {
      memberIoSocket.emit("projectDeleted", project);
    });
  };

  module.exports = ViewSocket;
} ());