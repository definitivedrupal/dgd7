//$Id: simplenews.js,v 1.5 2011/01/06 21:19:07 mirodietiker Exp $
(function ($) {

/**
 * Set text of Save button dependent on the selected send option.
 */
Drupal.behaviors.simplenewsCommandSend = {
  attach: function (context) {
    var commandSend = $(".simplenews-command-send", context);
    var sendButton = function () {
      switch ($(":radio:checked", commandSend).val()) {
      case '0':
        $('#edit-submit', context).attr({value: Drupal.t('Save')});
        break;
      case '1':
        $('#edit-submit', context).attr({value: Drupal.t('Save and send')});
        break;
      case '2':
        $('#edit-submit', context).attr({value: Drupal.t('Save and send test')});
        break;
      }
    }

    // Update send button at page load and when a send option is selected.
    sendButton();
    commandSend.click( function() { sendButton(); });
  }
};

})(jQuery);