// $Id: field_example.js,v 1.1 2010/06/22 01:15:30 rfay Exp $

/**
 * @file
 * Javascript for Field Example.
 */

/**
 * Provide a farbtastic colorpicker for the fancier widget.
 */
(function ($) {
  Drupal.behaviors.field_example_colorpicker = {
    attach: function(context) {
      $(".edit-field-example-colorpicker").live("focus", function(event) {
        var edit_field = this;
        var picker = $(this).closest('tr').find(".field-example-colorpicker");
        
        // Hide all color pickers except this one.
        $(".field-example-colorpicker").hide();
        $(picker).show();
        $.farbtastic(picker, function(color) {
          edit_field.value = color;
        }).setColor(edit_field.value);
      });
    }
  }
})(jQuery);
