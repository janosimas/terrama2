'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {

    var sliderCapabilities = [];

    var insertIntoSliderCapabilities = function(capability) {
      sliderCapabilities.push(capability);
    };

    var setSlider = function(rangeDate, layerId) {
			var valMap = rangeDate;

			var slider = $("#slider" + layerId.replace(':',''));
			var sliderParent = $(slider).parent();

			if(!$(sliderParent).is(":visible"))
				$(sliderParent).show();
			else
				$(sliderParent).hide();

			var labelDate = $(sliderParent).find("label");
			$(labelDate).text(moment(rangeDate[0]).format("lll"));
			
			$(slider).slider({
				min: 0,
				max: valMap.length - 1,
				value: 0,
				slide: function(event, ui) {
					$(labelDate).text(moment(rangeDate[ui.value]).format("lll"));
				},
				stop: function(event, ui) {                        
					doSlide(layerId, rangeDate[ui.value]);
				}       
			});
    };

    var doSlide = function(layerId, layerTime) {
      var timeFormat = moment(layerTime).format("YYYY-MM-DDThh:mm:ss") + "Z";
      TerraMA2WebComponents.MapDisplay.updateLayerTime(layerId, layerTime);
    };

    var changeLayerOpacity = function(layerId, opacityValue) {
      TerraMA2WebComponents.MapDisplay.updateLayerOpacity(layerId, opacityValue/100);
    };

    var setOpacitySlider = function(layerId, initialValue) {
			var slider = $("#opacity" + layerId.replace(':','').split('.').join('\\.'));
			var sliderParent = $(slider).parent();

			var label = $(sliderParent).find("label");
			$(label).text("Opacity: " + initialValue + "%");

			$(slider).slider({
				min: 0,
				max: 100,
				value: initialValue,
				slide: function(event, ui) {
					$(label).text("Opacity: " + ui.value + "%");
				},
				stop: function(event, ui) {                        
					changeLayerOpacity(layerId, ui.value);
				}       
			});
    };

    var init = function() {
      /**
       * Triggered when user clicks on TerraMA² Slider in Layers Menu
       * 
       * @param {Event}
       */
      $("#terrama2-layerexplorer").on("click", "#terrama2-slider", function(event) {
        var self = $(this);
        var parentLi = $(self).parent();
        var parentId = $(parentLi).attr("data-layerid");
        var layerName = parentId.split(':')[1];
        var capability = sliderCapabilities.find(function(capability) { return capability.name === layerName; });

        if (!capability) {
          console.log("Capability not found...");
          return;
        }
        if (!capability.extent instanceof Array) {
          console.log("Capability has not extent array.");
          return;
        }

        setSlider(capability.extent, parentId);
      });

      $("#terrama2-layerexplorer").on("click", "button[class~='close-slider']", function(e) {
        $(this).parent().hide();
      });
    };

    return {
      insertIntoSliderCapabilities: insertIntoSliderCapabilities,
      setOpacitySlider: setOpacitySlider,
      init: init
    };
  }
);