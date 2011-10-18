
(function ($) {

/**
 * Control the visibility of additional configuration options
 */
Drupal.behaviors.webformAltUI_file_field = {
  attach: function (context) {
    $('.type-groups', context).once('type-groups', function () {
      $(this).find('input[type=checkbox]').bind('click', Drupal.behaviors.webformAltUI_file_field.groupToggle);
    });
    $('.file-types', context).once('file-types', function () {
      $(this).find('input[type=checkbox]').bind('click', Drupal.behaviors.webformAltUI_file_field.checkToggle);
    });
  },
  groupToggle: function (event) {
    var $this = $(this);
    $.each(Drupal.settings.webform_alt_ui[$this.val()], function (index, value) {
      $('input[value=' + value + ']').attr('checked', $this.attr('checked'));
    });
    Drupal.behaviors.webformAltUI_file_field.checkToggle();
  },
  checkToggle: function () {
    var $this = $(this);
    $.each(Drupal.settings.webform_alt_ui, function(index, value) {
      var checked = true;
      $.each(value, function(index2, value2) {
        if ($('.file-types input[value=' + value2 + ']').attr('checked') == false) {
          checked = false;
        }
      });
      $('.type-groups input[value=' + index + ']').attr('checked', checked);
    });
  }
};

})(jQuery);
