(function ($) {

  Drupal.simpleCrop = Drupal.simpleCrop || {};
  Drupal.simpleCrop.setCoords = Drupal.simpleCrop.setCoords || {};

  Drupal.behaviors.simpleCrop = {
    attach: function(context, settings) {

      // Ensure that simplecrop js settings are defined.
      settings.simpleCrop = settings.simpleCrop || {};
      settings.simpleCrop.images = settings.simpleCrop.images || {};

      jQuery.each(settings.simpleCrop.images, function(i, value) {

        // Get wrapper of image field.
        var $widget = $('.simplecrop-widget.image-' + value.fid, context);

        // Get unprocessed original image.
        var $original_img = $('.original-image img', $widget).filter(':not(.crop-processed)');

        // If this original unprocessed image exists - then process it.
        if ($original_img.length == 1) {

          // Trigger jcrop processment only after image has been loaded.
          var $image = $('.original-image img', $widget);
          $image.bind('load', function() {

            // Initialize jcrop plugin.
            var jcrop = $.Jcrop($image);

            // Get crop settings.
            var crop = value.settings;

            // Get scaled image width and height.
            crop.resolution = crop.resolution || [$original_img.attr('width'), $original_img.attr('height')];

            // Calculate image min/max crop area selections.
            crop.min_area = crop.min_area || [0, 0];
            crop.max_area = crop.max_area || crop.resolution;

            // Set default handlers to keep coords up to date.
            jcrop.setOptions({
              onChange: Drupal.simpleCrop.setCoords,
              onSelect: Drupal.simpleCrop.setCoords,
              minSize: crop.min_area,
              maxSize: crop.max_area
            });

            // Set aspect ratio (if exists).
            crop.ratio = crop.ratio || 0;
            if (crop.ratio) {
              jcrop.setOptions({
                aspectRatio: crop.ratio
              });
            }

            // Define initial crop area (if exists).
            crop.initial_area = crop.initial_area || false;
            if (crop.initial_area) {
              var coords = crop.initial_area;
              jcrop.setSelect([coords.x, coords.y, coords.x2, coords.y2]);
            }
            // Remove crop area selection from image.
            else {
              jcrop.release();
            }

            // Mark image as processed to avoid duplicated processments.
            $original_img.addClass('crop-processed');

          });
        }

      });

    }
  };

  Drupal.simpleCrop.setCoords = function(coords) {

    // Find a field wrapper for simplecrop widget.
    var $widget = $(this.ui.holder).parents('.simplecrop-widget');

    // Set an actual values in hidden inputs.
    $('.x', $widget).val(coords.x);
    $('.y', $widget).val(coords.y);
    $('.x2', $widget).val(coords.x2);
    $('.y2', $widget).val(coords.y2);
  }

})(jQuery);
