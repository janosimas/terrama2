'use strict';

define(
  ['components/Utils', 'components/Slider', 'components/Layers', 'components/AnimatedLayer','TerraMA2WebComponents'],
  function(Utils, Slider, Layers, AnimatedLayer, TerraMA2WebComponents) {

    // Flag that indicates if there is a exportation in progress
    var memberExportationInProgress = false;
    // Timeout used to update exportation text
    var memberExportationTextTimeout = null;

    var loadSocketsListeners = function() {
      Utils.getWebAppSocket().on('generateFileResponse', function(result) {
        if(result.progress !== undefined && result.progress >= 100) {
          $('#exportation-status > div > span').html('Almost there! The file is being prepared for download<span>...</span>');
          $('#exportation-status > div > div').addClass('hidden');

          $('#exportation-status > div > div > div > span').text('0% Complete');
          $('#exportation-status > div > div > div').css('width', '0%');
          $('#exportation-status > div > div > div').attr('aria-valuenow', 0);
        } else if(result.progress !== undefined) {
          if($('#exportation-status > div > div').hasClass('hidden')) {
            $('#exportation-status > div > span').html('Please wait, the requested data is being exported<span>...</span>');
            $('#exportation-status > div > div').removeClass('hidden');
          }

          $('#exportation-status > div > div > div > span').text(result.progress + '% Complete');
          $('#exportation-status > div > div > div').css('width', result.progress + '%');
          $('#exportation-status > div > div > div').attr('aria-valuenow', result.progress);
        } else {
          memberExportationInProgress = false;
          $('#exportation-status').addClass('hidden');
          window.clearInterval(memberExportationTextTimeout);
          memberExportationTextTimeout = null;
          $('#exportation-status > div > span').html('');
          $('#exportation-status > div > div').addClass('hidden');
          $('#exportation-status > div > div > div > span').text('0% Complete');
          $('#exportation-status > div > div > div').css('width', '0%');

          var exportLink = webadminHostInfo.protocol + webadminHostInfo.host + ":" + webadminHostInfo.port + webadminHostInfo.basePath + "export?folder=" + result.folder + "&file=" + result.file;
          $('#exportation-iframe').attr('src', exportLink);
        }
      });
    };

    var loadEvents = function() {
      $('#export').on('click', function() {
        var layer = Layers.getLayerById($(this).data("layerid"));

        if(layer !== null && layer.exportation !== null) {
          if(layer.exportation.hasOwnProperty("table")) {
            var exportationParams = {
              format: $("#exportation-type").val().toString(),
              schema: layer.exportation.schema,
              table: layer.exportation.table,
              dataProviderId: layer.exportation.dataProviderId,
              fileName: layer.name
            };

            if(layer.exportation.dateField !== null) {
              var dateInfo = layer.dateInfo;
              exportationParams.dateTimeField = layer.exportation.dateField;
              exportationParams.dateTimeFrom = dateInfo.startFilterDate;
              exportationParams.dateTimeTo = dateInfo.endFilterDate;
            }

            $('#exportation-status > div > span').html('Verifying data for export<span>...</span>');

            memberExportationTextTimeout = setInterval(function() {
              var text = $('#exportation-status > div > span > span').html();

              if(text === "...")
                $('#exportation-status > div > span > span').html('&nbsp;&nbsp;&nbsp;');
              else if(text === "..&nbsp;")
                $('#exportation-status > div > span > span').html('...');
              else if(text === ".&nbsp;&nbsp;")
                $('#exportation-status > div > span > span').html('..&nbsp;');
              else
                $('#exportation-status > div > span > span').html('.&nbsp;&nbsp;');
            }, 800);

            $('#exportation-status').removeClass('hidden');

            memberExportationInProgress = true;

            Utils.getWebAppSocket().emit('generateFileRequest', exportationParams);
          } else {
            var urlParams = "?dpi=" + layer.exportation.dataProviderId + "&mask=" + layer.exportation.mask + "&file=" + layer.name;

            var params = {
              dpi: layer.exportation.dataProviderId,
              mask: layer.exportation.mask,
              file: layer.name
            };

            if(layer.dateInfo.dates !== undefined && layer.dateInfo.dates.length > 0) {
              urlParams += "." + layer.dateInfo.dates[layer.dateInfo.initialDateIndex] + "&date=" + layer.dateInfo.dates[layer.dateInfo.initialDateIndex];
              params.date = layer.dateInfo.dates[layer.dateInfo.initialDateIndex];
              params.file += "." + layer.dateInfo.dates[layer.dateInfo.initialDateIndex];
            }

            $.post(BASE_URL + "check-grid", params, function(data) {
              if(data.result)
                $('#exportation-iframe').attr('src', webadminHostInfo.protocol + webadminHostInfo.host + ":" + webadminHostInfo.port + webadminHostInfo.basePath + "export-grid" + urlParams);
              else {
                $("#terrama2Alert > p > strong").text('');
                $("#terrama2Alert > p > span").text('O arquivo não foi encontrado.');
                $("#terrama2Alert").removeClass('hide');
              }
            });
          }
        }
      });

      $("#terrama2-sortlayers").on("click", ".glyphicon-resize-full", function() {
        var layer = Layers.getLayerById($(this).parent().parent().data("layerid"));

        if(layer !== null) {
          TerraMA2WebComponents.MapDisplay.zoomToExtent(layer.boundingBox);
        }
      });

      $("#visible-layers-extent").on("click", function() {
        var allVisibleLayers = Layers.getVisibleLayers();
        var visibleLayers = [];

        for(var i = 1, allVisibleLayersLength = allVisibleLayers.length; i < allVisibleLayersLength; i++) {
          if(allVisibleLayers[i].parent !== "custom" && allVisibleLayers[i].parent !== "template")
            visibleLayers.push(allVisibleLayers[i]);
        }

        if(visibleLayers.length > 0) {
          var boundingBox = [visibleLayers[0].boundingBox[0], visibleLayers[0].boundingBox[1], visibleLayers[0].boundingBox[2], visibleLayers[0].boundingBox[3]];

          for(var i = 1, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
            if(visibleLayers[i].boundingBox[0] < boundingBox[0])
              boundingBox[0] = visibleLayers[i].boundingBox[0];

            if(visibleLayers[i].boundingBox[1] < boundingBox[1])
              boundingBox[1] = visibleLayers[i].boundingBox[1];

            if(visibleLayers[i].boundingBox[2] > boundingBox[2])
              boundingBox[2] = visibleLayers[i].boundingBox[2];

            if(visibleLayers[i].boundingBox[3] > boundingBox[3])
              boundingBox[3] = visibleLayers[i].boundingBox[3];
          }

          TerraMA2WebComponents.MapDisplay.zoomToExtent(boundingBox);
        }
      });

      $("#terrama2-sortlayers").on("click", ".fa-gear", function() {
        var layer = Layers.getLayerById($(this).parent().parent().data("layerid"));
        $("#animate").data("layerid", layer.id);

        if(layer !== null) {
          var openLayerToolbox = function() {
            $("#layer-toolbox .layer-toolbox-header > .layer-name").text(layer.name);
            $("#layer-toolbox .layer-toolbox-header > .layer-name").attr("title", layer.name);

            if(layer.description) {
              $("#layer-toolbox .layer-toolbox-body .layer-description .box-body").text(layer.description);

              if($("#layer-toolbox .layer-toolbox-body .layer-description").hasClass("hidden"))
                $("#layer-toolbox .layer-toolbox-body .layer-description").removeClass("hidden");
            } else {
              $("#layer-toolbox .layer-toolbox-body .layer-description .box-body").text("");

              if(!$("#layer-toolbox .layer-toolbox-body .layer-description").hasClass("hidden"))
                $("#layer-toolbox .layer-toolbox-body .layer-description").addClass("hidden");
            }

            $("#layer-toolbox .layer-toolbox-body > #slider-box .box-body").empty().html("<label></label><br/><div id=\"opacity" + layer.id.replace(":","") + "\"></div>");
            var currentOpacity = TerraMA2WebComponents.MapDisplay.getLayerOpacity(layer.id) * 100;
            Slider.setOpacitySlider(layer.id, currentOpacity);

            if($("#layer-toolbox").hasClass("hidden"))
              $("#layer-toolbox").removeClass("hidden");
          };

          if(layer.exportation !== null && layer.dataSeriesTypeName === "GRID") {
            $.post(BASE_URL + "check-grid-folder", { dpi: layer.exportation.dataProviderId }, function(data) {
              if(data.result) {
                if(!$("#exportation-type").hasClass("hidden"))
                  $("#exportation-type").addClass("hidden");

                $("#export").data("layerid", layer.id);

                if($("#exportation-box").hasClass("hidden"))
                  $("#exportation-box").removeClass("hidden");
              } else {
                if(!$("#exportation-box").hasClass("hidden"))
                  $("#exportation-box").addClass("hidden");
              }

              openLayerToolbox();
            });
          } else if(layer.exportation !== null) {
            if($("#exportation-type").hasClass("hidden"))
              $("#exportation-type").removeClass("hidden");

            $("#export").data("layerid", layer.id);

            if($("#exportation-box").hasClass("hidden"))
              $("#exportation-box").removeClass("hidden");

            openLayerToolbox();
          } else {
            if(!$("#exportation-box").hasClass("hidden"))
              $("#exportation-box").addClass("hidden");

            openLayerToolbox();
          }

          if (layer.dateInfo && layer.dateInfo.dates && Array.isArray(layer.dateInfo.dates)){
            if($("#animate-layer-box").hasClass("hidden"))
              $("#animate-layer-box").removeClass("hidden");

            if($("#dates-slider").hasClass("hidden"))
              $("#dates-slider").removeClass("hidden");

            if(!$("#dates-calendar").hasClass("hidden"))
              $("#dates-calendar").addClass("hidden");

            $("#layer-toolbox .layer-toolbox-body > #animate-layer-box #dates-calendar").empty();
            $("#layer-toolbox .layer-toolbox-body > #animate-layer-box #dates-slider").empty().html("<div id=\"dates" + layer.id.replace(":","") + "\"></div><div id=\"rangeDates\"><label>From:&nbsp</label><span id=\"initialDate\"></span></br><label>To:&nbsp </label><span id=\"finalDate\"></span></div>");
            AnimatedLayer.setLayerToAnimate(layer);
            AnimatedLayer.setDatesSlider();
          } else if (layer.dateInfo && layer.dateInfo.dates && typeof layer.dateInfo.dates === "object"){

            if($("#animate-layer-box").hasClass("hidden"))
              $("#animate-layer-box").removeClass("hidden");

            if($("#dates-calendar").hasClass("hidden"))
              $("#dates-calendar").removeClass("hidden");

            if(!$("#dates-slider").hasClass("hidden"))
              $("#dates-slider").addClass("hidden");

            var calendarFieldsHtml = "<div class=\"input-group date\" datetimepicker><input type=\"text\" id=\"animation-calendar\" class=\"form-control\" style=\"padding-left:5px\"/>" +
                                       "<span class=\"input-group-addon\">" +
                                         "<span class=\"glyphicon glyphicon-calendar\"></span>" +
                                       "</span>" +
                                     "</div>" +
                                     "<div style=\"margin-top:8px\">" +
                                       "<label>Period:&nbsp</label>" +
                                       "<input type=\"number\" id=\"frequency\" value=1 min=1>&nbsp" +
                                       "<select id=\"unitTime\" class=\"form-control\">" +
                                         "<option value=\"minutes\">Minutes</option>" + 
                                         "<option value=\"hours\" selected=\"selected\">Hours</option>" + 
                                         "<option value=\"days\">Days</option>" + 
                                       "</select>" +
                                     "</div>"

            $("#layer-toolbox .layer-toolbox-body > #animate-layer-box #dates-calendar").empty().html(calendarFieldsHtml);
            $("#layer-toolbox .layer-toolbox-body > #animate-layer-box #dates-slider").empty();
            AnimatedLayer.setLayerToAnimate(layer);
            AnimatedLayer.setDatesCalendar();
          } else {
            
            if(!$("#animate-layer-box").hasClass("hidden"))
              $("#animate-layer-box").addClass("hidden");

            $("#layer-toolbox .layer-toolbox-body > #animate-layer-box #dates-calendar").empty();
            $("#layer-toolbox .layer-toolbox-body > #animate-layer-box #dates-slider").empty();
          }
        }
      });

      $("#layer-toolbox .layer-toolbox-header > .btn").on("click", function() {
        $("#layer-toolbox").addClass("hidden");
      });
    };

    var init = function() {
      loadSocketsListeners();
      loadEvents();

      $("#layer-toolbox").draggable({
        containment: $('#terrama2-map'),
        handle: '.layer-toolbox-header'
      })
    };

    return {
      init: init
    };
  }
);
