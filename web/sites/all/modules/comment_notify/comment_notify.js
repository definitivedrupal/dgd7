(function ($) {
  Drupal.behaviors.commentNotify = {
    attach: function (context) {
      $('#edit-notify-type', context).hide();
      $('#edit-notify', context).bind('change', function() {
        if ($(this).attr('checked')) {
          $('#edit-notify-type', context).show();
          if ($('#edit-notify-type input:checked', context).length == 0) {
            $('#edit-notify-type input', context)[0].checked = 'checked';
          }
        }
        else {
          $('#edit-notify-type', context).hide();
        }
      });
      $('#edit-notify', context).trigger('change');
    }
  }
})(jQuery);
