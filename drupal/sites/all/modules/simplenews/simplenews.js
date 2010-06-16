// $Id: simplenews.js,v 1.4 2010/02/20 09:23:04 sutharsan Exp $

/**
 * Set text of Save button dependent on the selected send option.
 * @todo Changed Drupal.behaviors: http://drupal.org/node/224333#drupal_behaviors
 * @todo Wrap jQuery code: http://drupal.org/node/224333#javascript_compatibility
 * @todo behaviors settings passed locally: http://drupal.org/node/224333#local_settings_behaviors
 */
// TODO This does not work
Drupal.behaviors.simplenewsCommandSend = function (context) {
  var simplenewsSendButton = function () {
    switch ($(".simplenews-command-send :radio:checked").val()) {
      case '0':
        $('#edit-submit').attr({value: Drupal.t('Save')});
        break;
      case '1':
        $('#edit-submit').attr({value: Drupal.t('Save and send')});
        break;
      case '2':
        $('#edit-submit').attr({value: Drupal.t('Save and send test')});
        break;
    }
  }
  
  // Update send button at page load and when a send option is selected.
  $(function() { simplenewsSendButton(); });
  $(".simplenews-command-send").click( function() { simplenewsSendButton(); });
  
  
}
