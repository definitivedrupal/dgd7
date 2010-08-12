// $Id: ajax_example.js,v 1.1 2010/03/23 03:21:29 rfay Exp $

(function($) {

  // Re-enable form elements that are disabled for non-ajax situations.
  Drupal.behaviors.enableFormItemsForAjaxForms = {
    attach: function() {
    // If ajax is enabled.
    if (Drupal.ajax) {
      $('.enabled-for-ajax').removeAttr('disabled');
    }
    
    
    // Below is only for the demo case of showing with js turned off.
    // It overrides the behavior of the CSS that would normally turn off
    // the 'ok' button when JS is enabled. Here, for demonstration purposes,
    // we have AJAX disabled but JS turned on, so use this to simulate.
    if (!Drupal.ajax) {
      $('html.js .next-button').show();
    }
  }
  };


})(jQuery);
