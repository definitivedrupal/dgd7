/**
 * @file
 * UI enhancements for Skinr UI.
 */

(function($) {

Drupal.behaviors.skinrUIAdd = {
  attach: function (context, settings) {
    $('form#skinr-ui-add', context).once('skinr-ui-add', function () {
      Drupal.skinrUIAdd.attachUpdateSelects(this, settings);
    });
  }
};

Drupal.skinrUIAdd = {
  /**
   * Implements dependent select dropdowns on the 'Manage fields' screen.
   */
  attachUpdateSelects: function(context, settings) {
    var elementOptions = settings.elementOptions;

    // Store the default text of widget selects.
    $('#edit-element', context).each(function () {
      this.initialValue = this.options[0].text;
    });

    // 'Type' select updates 'Element' select.
    $('#edit-type', context).each(function () {
      this.targetSelect = $('#edit-element', context);

      $(this).bind('change keyup', function () {
        var selectedType = this.options[this.selectedIndex].value;
        var options = (selectedType in elementOptions ? elementOptions[selectedType] : []);
        this.targetSelect.skinrUIPopulateOptions(options);
      });

      // Trigger change on initial pageload to get the right element options
      // when type comes pre-selected (on failed validation).
      $(this).trigger('change', false);
    });
  }
};

/**
 * Populates options in a select input.
 */
jQuery.fn.skinrUIPopulateOptions = function (options, selected) {
  return this.each(function () {
    var disabled = false;
    if (options.length == 0) {
      options = [this.initialValue];
      disabled = true;
    }

    // If possible, keep the same element selected when changing type.
    // This is based on textual value, since the internal value might be
    // different.
    var previousSelectedText = this.options[this.selectedIndex].text;

    var html = '';
    jQuery.each(options, function (value, text) {
      if (typeof(text) == 'object') {
        // Add option group and strip label indent.
        html += '<optgroup label="' + value.replace(/^(&#160;)*\s*/, '') + '">';
        jQuery.each(text, function (value, text) {
          // Figure out which value should be selected. The 'selected' param
          // takes precedence.
          var is_selected = ((typeof selected != 'undefined' && value == selected) || (typeof selected == 'undefined' && text == previousSelectedText));
          html += '<option value="' + value + '"' + (is_selected ? ' selected="selected"' : '') + '>' + text + '</option>';
        });
        html += '</optgroup>';
      }
      else {
        // Figure out which value should be selected. The 'selected' param
        // takes precedence.
        var is_selected = ((typeof selected != 'undefined' && value == selected) || (typeof selected == 'undefined' && text == previousSelectedText));
        html += '<option value="' + value + '"' + (is_selected ? ' selected="selected"' : '') + '>' + text + '</option>';
      }
    });

    if (disabled) {
      $(this).html(html).attr('disabled', 'disabled');
    }
    else {
      $(this).html(html).removeAttr('disabled');
    }
  });
};

})(jQuery);
