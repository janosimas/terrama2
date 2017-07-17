"use strict";

define(
  ['components/Calendar',
  'components/Capabilities',
  'components/Slider',
  'components/Utils',
  'components/LayerStatus',
  'components/Layers',
  'components/AddLayerByUri',
  'components/Sortable',
  'enums/LayerStatusEnum',  
  'TerraMA2WebComponents'],
  function(Calendar, Capabilities, Slider, Utils, LayerStatus, Layers, AddLayerByUri, Sortable, LayerStatusEnum, TerraMA2WebComponents) {

		var visibleLayers = [];
    var memberWindowHeight;
    var memberReducedHeight;

    // function to add class treeview-menu to work lte admin css behavior when sidebar is mini
    var addTreeviewMenuClass = function() {
      var customParentLi = $(".parent_li").children('ul');
      for (var i = 0; i < customParentLi.length; i++){
        customParentLi[i].className += ' treeview-menu';
      }
    };

    var changeProjects = function(){
      var allLayers = Layers.getAllLayers();
      allLayers.forEach(function(layer){
        if (layer.projectId){
          removeLayerOfExplorer(layer);
        }
      });
      Layers.fillLayersData();
    }

    var removeLayerOfExplorer = function(layer){
      var layerId = layer.id;
      var parent = layer.parent;

      if (layer.visible){
        $("#"+ layer.htmlId + " input.terrama2-layerexplorer-checkbox").trigger("click");
      }
      $("#terrama2-sortlayers").find('li#' + layer.htmlId).remove();
      TerraMA2WebComponents.LayerExplorer.removeLayer(layerId, "terrama2-layerexplorer");
      if ($("#" + parent + " li").length == 0){
        Layers.changeParentLayerStatus(parent, "");
      }
    }

    var loadEvents = function() {
      $('#projects').on('change', changeProjects);
      
			$('#mini-toggle').click(function(){
				TerraMA2WebComponents.MapDisplay.updateMapSize();
			});

			/**
			 * When window resize the map must follow the width
			 */
			$(window).resize(function() {
				memberWindowHeight = $(window).height();
				memberReducedHeight = memberWindowHeight - $("#terrama-header").height();

				if ($("body").hasClass('full_screen')) {
					var interval = window.setInterval(function() { $("#terrama2-map").width("100%"); }, 100);
					window.setTimeout(function() { clearInterval(interval); }, 2000);

					$("#terrama2-map").height(memberWindowHeight + "px");
					$("#content").height(memberWindowHeight + "px");
				} else {
					var interval = window.setInterval(function() { $("#terrama2-map").width($("#content").width() + "px"); }, 100);
					window.setTimeout(function() { clearInterval(interval); }, 2000);

					$("#terrama2-map").height(memberReducedHeight + "px");
					$("#content").height(memberReducedHeight + "px");
				}

				TerraMA2WebComponents.MapDisplay.updateMapSize();
			});

			$('#close-alert').on('click', function(){
				var alertDiv = $(this).closest('div');
				alertDiv.addClass('hide');
			});

			$('#terrama2-layerexplorer').on('click', 'input.terrama2-layerexplorer-checkbox', function(){

				var layerid = $(this).closest('li').data('layerid');
        var layerObject = Layers.getLayerById(layerid);
        var isVisible = layerObject.visible;
				if(isVisible) {
					$('#terrama2-sortlayers').find('li#' + layerObject.htmlId).addClass('hide');
          Layers.changeLayerVisible(layerObject.id, false);
				} else {
					$('#terrama2-sortlayers').find('li#' + layerObject.htmlId).removeClass('hide');
          Layers.changeLayerVisible(layerObject.id, true);
				}

        $("#terrama2-map").trigger("setGetFeatureInfoToolSelect");
        $("#terrama2-map").trigger("createAttributesTable");
        $("#legend-box").trigger("setLegends");

        if (layerObject.status == LayerStatusEnum.NEW || layerObject.status == LayerStatusEnum.ALERT){
					TerraMA2WebComponents.MapDisplay.updateLayerSourceParams(layerObject.id, { "": Date.now().toString() }, true);
					Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ONLINE);
        }
        
			});

			//change status icon when close the group layer
			$('.parent_li').on('click', function() {
				var parent = this.getAttribute('id');
				var parentLi = $("#" + parent);
        var parentLayer = Layers.getLayerById(parent);
        if (!parentLayer)
          return;

				if(parentLi.hasClass('open')) {
          if (parentLayer.status == LayerStatusEnum.NEW || parentLayer.status == LayerStatusEnum.ALERT)
            Layers.changeParentLayerStatus(parentLayer.id, LayerStatusEnum.ONLINE);
          else if (parentLayer.status == LayerStatusEnum.NEW_OFFLINE || parentLayer.status == LayerStatusEnum.ALERT)
            Layers.changeParentLayerStatus(parentLayer.id, LayerStatusEnum.OFFLINE);
				}
			});

    };

    var loadSocketsListeners = function() {
			Utils.getWebAppSocket().on("viewResponse", function(data) {
				$('#projects').empty();
				for(var i = 0, projectsLength = data.projects.length; i < projectsLength; i++)
					$('#projects').append($('<option></option>').attr('value', data.projects[i].id).text(data.projects[i].name));
        
        data.views.forEach(function(view){
          var objectLayer = Layers.createLayerObject(view);
          Layers.addLayer(objectLayer);
        });

				Layers.fillLayersData();
			});

			// When receive a new view, add in layers component
			Utils.getWebAppSocket().on('viewReceived', function(data){
				if(!data.private || (data.private && userLogged)) {

          var layerObject = Layers.createLayerObject(data);
          var newLayer = Layers.getLayerById(layerObject.id) == null ? true : false;

					if(!newLayer) {
						Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.NEW);
						Layers.changeParentLayerStatus(layerObject.parent, LayerStatusEnum.NEW);
						var workspace = layerObject.workspace;
						var uriGeoServer = layerObject.uriGeoServer;
						var serverType = layerObject.serverType;
            var url = uriGeoServer + '/' + workspace + '/' + layerObject.nameId + '/wms?service=WMS&version=1.1.0&request=GetCapabilities';
            var getCapabilitiesUrl = {
              layerName: layerObject.nameId,
              layerId: layerObject.id,
              parent: layerObject.parent,
              url: url,
              format: 'xml',
              update: true
            }
            Utils.getSocket().emit('proxyRequestCapabilities', getCapabilitiesUrl);
						return;
					}
          var currentProject = $("#projects").val();
          Layers.addLayer(layerObject);
          if (layerObject.projectId == currentProject){
            Layers.fillLayersData([layerObject]);
            Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.NEW);
            Layers.changeParentLayerStatus(layerObject.parent, LayerStatusEnum.NEW);
          }
				}
			});

			Utils.getWebAppSocket().on("notifyView", function(data) {
				var layerId = data.workspace + ":" + data.layer.name;
        var layerObject = Layers.getLayerById(layerId);
        if (layerObject){
          Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ALERT);
          Layers.changeParentLayerStatus("alert", LayerStatusEnum.ALERT);
        }
			});

			Utils.getWebAppSocket().on("removeView", function(data) {
        var layerId = data.workspace + ":" + data.layer.name;
        var parent = data.parent;
        var layerObject = Layers.getLayerById(layerId);
        if (layerObject){
          removeLayerOfExplorer(layerObject);
          Layers.removeLayer(layerObject.id);
        }
			});

			// Checking map server connection response
			Utils.getSocket().on('connectionResponse', function(data) {
				if(data.url) {
					//getting element to disable if there are no connection with map server
					var listElement = $("li[data-layerid='"+ data.requestId +"'].treeview");
					var inputElement = listElement.find('input');
					var parent = listElement.attr('data-parentid')
          var layerObject = Layers.getLayerById(data.requestId);
          if (!layerObject)
            return;

					//if not connected disabled the layer selection
					if(!data.connected) {
						listElement.prop("title", "Map Server is not responding");
						if(inputElement.is(':checked'))
							inputElement.trigger("click");

						if(!inputElement.hasClass("disabled-content"))
							inputElement.addClass("disabled-content");

						Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.OFFLINE);
            Layers.changeParentLayerStatus(parent, LayerStatusEnum.OFFLINE);
					} else {
						listElement[0].removeAttribute("title");
						if (inputElement.hasClass("disabled-content"))
							inputElement.removeClass("disabled-content");
						
						var lastStatus = layerObject.status;
						if(lastStatus == LayerStatusEnum.OFFLINE) {
							Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ONLINE);
              Layers.changeParentLayerStatus(parent, LayerStatusEnum.ONLINE);
						}
					}
				}
			});

			Utils.getSocket().on('proxyResponseCapabilities', function(data) {
				try {
					var layerCapabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
					var layerIndex = layerCapabilities.map(function (c){return c.name}).indexOf(data.layerName);

					if(layerIndex < 0)
						return;

					if(!layerCapabilities[layerIndex].extent)
						return;

					var span = "";
					var listElement = $("li[data-layerid='"+ data.parent +"']");
					var li = $(listElement).find("li[data-layerid='" + data.layerId +"']");

					if(li.length === 0)
						return;

					if(data.update) {
						Calendar.updateDatePicker(layerCapabilities[layerIndex], data.layerId);
					} else {
						var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':','') + "'></div></div>";
						$(li).append(sliderDiv);

						if(layerCapabilities[layerIndex].extent instanceof Array) {
							Slider.insertIntoSliderCapabilities(layerCapabilities[layerIndex]);
							span += "<span id='terrama2-slider' class='terrama2-datepicker-icon'>" + Calendar.makeHelperDatePicker(layerCapabilities[layerIndex]) + "<i class='fa fa-sliders'></i></span>";
						} else if (layerCapabilities[layerIndex].extent instanceof Object) {
							span += "<span id='terrama2-calendar' class='terrama2-datepicker-icon'>" + Calendar.makeHelperDatePicker(layerCapabilities[layerIndex]) + "<i class='fa fa-calendar'></i></span>";
						}

						$(li).append($(span));
					}
				} catch(e) {
					return;
				}
			});

			Utils.getSocket().on('proxyResponse', function(data){
				if(data.requestId == "GetFeatureInfoTool") {
					var featureInfo = data.msg;
					var featuresLength = featureInfo.features.length;

					if(featuresLength > 0) {
						var firesAttributes = "";

						for(var i = 0; i < featuresLength; i++) {
							firesAttributes += "<table class=\"table table-striped\"><tbody>";

							for(var key in featureInfo.features[i].properties) {
								firesAttributes += "<tr><td><strong>" + key + "</strong></td><td>" + featureInfo.features[i].properties[key] + "</td></tr>";
							}

							firesAttributes += "</tbody></table>";
							if(featuresLength > (i + 1)) firesAttributes += "<hr/>";
						}

						$('#feature-info-box').html(firesAttributes);

						$('#feature-info-box').dialog({
							dialogClass: "feature-info-box",
							title: "Attributes of layer: " + data.params.layerName,
							width: 400,
							height: 380,
							modal: false,
							resizable: true,
							draggable: true,
							closeOnEscape: true,
							closeText: "",
							position: { my: 'top', at: 'top+75' },
							open: function () {
								$('.ui-dialog-titlebar-close').css('background-image', 'url(../images/close.png)');
								$('.ui-dialog-titlebar-close').css('background-position', 'center');
								$('.ui-dialog-titlebar-close').css('background-size', '20px');
							},
							close: function () {
								$('.ui-dialog-titlebar-close').css('background-image', '');
								$('.ui-dialog-titlebar-close').css('background-position', '');
								$('.ui-dialog-titlebar-close').css('background-size', '');
							}
						});
					}
				} else {
					try {
						var capabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
						AddLayerByUri.fillModal(capabilities);
						$('#layersModal').modal('show');
					} catch(e) {
						$("#terrama2Alert > p > strong").text('Invalid URL!');
						$("#terrama2Alert > p > span").text('Error to find capabilities.');
						$("#terrama2Alert").removeClass('hide');
					}
				}
			});
    };

		var loadLayout = function() {
			memberWindowHeight = $(window).height();
			memberReducedHeight = memberWindowHeight - $("#terrama-header").height();

			$.TerraMAMonitor = {};

			$.TerraMAMonitor.options = {
				sidebarToggleSelector: "[full_screen='true']",
			};

			$.TerraMAMonitor.pushMenu = {
				activate: function(toggleBtn){
					$(document).on('click', toggleBtn, function(e){
					e.preventDefault();

					if($("body").hasClass('full_screen')) {
						$("body").removeClass('full_screen');
						$("body").addClass('sidebar-mini'); 

						$("#content").height(memberReducedHeight + "px");
						$("#terrama2-map").height(memberReducedHeight + "px");
						$("#terrama2-map").width("auto");
					} else {
						$("body").addClass('full_screen');
						$("body").removeClass('sidebar-mini');

						$("#content").height(memberWindowHeight + "px");
						$("#terrama2-map").height(memberWindowHeight + "px");
						$("#terrama2-map").width("100%");
					}
					
					TerraMA2WebComponents.MapDisplay.updateMapSize();
					})
				}
			};

			$.TerraMAMonitor.tree = function(menu) {
				var _this = this;
				$(document).off('click', menu + ' li span').on('click', menu + ' li span', function (e) {
					//Get the clicked link and the next element
					var $this = $(this);
					var checkElement = $this.next();

					//Check if the next element is a menu and is visible
					if ((checkElement.is('.treeview-menu')) && (checkElement.is(':visible')) && (!$('body').hasClass('sidebar-collapse'))) {
						//Close the menu
						checkElement.slideUp(500, function () {
							checkElement.removeClass('menu-open');
						});
						checkElement.parent("li").removeClass("active");
					}
					//If the menu is not visible
					else if ((checkElement.is('.treeview-menu')) && (!checkElement.is(':visible'))) {
						//Get the parent menu
						var parent = $this.parents('ul').first();
						//Get the parent li
						var parent_li = $this.parent("li");

						//Open the target menu and add the menu-open class
						checkElement.slideDown(50, function () {
							//Add the class active to the parent li
							checkElement.addClass('menu-open');
							parent.find('li.active').removeClass('active');
							parent_li.addClass('active');
						});
					}
					//if this isn't a link, prevent the page from being redirected
					if (checkElement.is('.treeview-menu')) {
						e.preventDefault();
					}
				});
			};

			var o = $.TerraMAMonitor.options;

			$.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
			$.TerraMAMonitor.tree('.sidebar');

			$("#content").height(memberReducedHeight + "px");
			$("#terrama2-map").height(memberReducedHeight + "px");

			var mapWidthInterval = window.setInterval(function() { $("#terrama2-map").width($("#content").width() + "px"); }, 100);
			window.setTimeout(function() { clearInterval(mapWidthInterval); }, 2000);

			$("#dynamic").find("div").each(function() {
				$(this).addClass("fa fa-clock-o");
			});

			$("#static").find("div").each(function() {
				$(this).addClass("fa fa-folder");
			});

			$("#analysis").find("div").each(function() {
				$(this).addClass("fa fa-search");
			});

			$("#alert").find("div").each(function() {
				$(this).addClass("fa fa-warning");
			});

			$("#template").find("div").each(function() {
				$(this).addClass("fa fa-map");
			});

			$("#custom").find("div").each(function() {
				$(this).addClass("fa fa-link");
			});

			var leftArrow = "<span class='pull-right-container'> <i class='fa fa-angle-left pull-right'></i> </span>";
			
			$("#dynamic").children("span").each(function() {
				$(this).append(leftArrow);
			});

			$("#static").children("span").each(function() {
				$(this).append(leftArrow);
			});

			$("#analysis").children("span").each(function() {
				$(this).append(leftArrow);
			});

			$("#alert").children("span").each(function() {
				$(this).append(leftArrow);
			});

			$("#template").children("span").each(function() {
				$(this).append(leftArrow);
			});

			$("#custom").children("span").each(function() {
				$(this).append(leftArrow);
			});

		};

    var init = function() {
			if(message != "") {
				$("#terrama2Alert > p > strong").text('');
				$("#terrama2Alert > p > span").text(message);
				$("#terrama2Alert").removeClass('hide');
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("custom", "Custom", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("custom", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({layers:["custom"], name: "Custom"});
        Layers.addLayer(layerObject);
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("template", "Template", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("template", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({layers:["template"], name: "Template"});
        Layers.addLayer(layerObject);
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("static", "Static Data", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("static", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({layers:["static"], name: "Static Data"});
        Layers.addLayer(layerObject);
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("dynamic", "Dynamic Data", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("dynamic", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({layers:["dynamic"], name: "Dynamic Data"});
        Layers.addLayer(layerObject);
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("analysis", "Analysis", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("analysis", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({layers:["analysis"], name: "Analysis"});
        Layers.addLayer(layerObject);
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("alert", "Alert", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("alert", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({layers:["alert"], name: "Alert"});
        Layers.addLayer(layerObject);
			}

			//Adding open map street
			if(TerraMA2WebComponents.MapDisplay.addOSMLayer("osm", "Open street", "Open street", false, "terrama2-layerexplorer", false)) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("osm", "template", null, "treeview unsortable terrama2-truncate-text", null);
        var layerObject = Layers.createLayerObject({layers:["osm"], name: "Open street", type: "template"});
        Layers.addLayer(layerObject);
				LayerStatus.addLayerStatusIcon("osm");
				Layers.changeLayerStatus("osm", LayerStatusEnum.ONLINE);
			}

			addTreeviewMenuClass();
			LayerStatus.addGroupSpanIcon();
      Layers.addLayersToSort();
      Sortable.setSortable();
      Layers.changeParentLayerStatus("template", LayerStatusEnum.ONLINE);

			// Check connections every 30 seconds
			var intervalID = setInterval(function(){
        var allLayers = Layers.getAllLayers();
				allLayers.forEach(function(layerObject){
          var currentProject = $("#projects").val();
          if (currentProject == layerObject.projectId)
					  Utils.getSocket().emit('checkConnection', {url: layerObject.uriGeoServer, requestId: layerObject.id});
				});
			}, 30000);

			loadSocketsListeners();
			loadEvents();
			loadLayout();
      $("#osm input").trigger("click");
      
			Utils.getWebAppSocket().emit('viewRequest');
    };

    return {
      init: init
    };
  }
);
