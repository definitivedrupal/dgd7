/**
 * @author Caroline Schnapp
 */
// $Id: jquery.plus1.js,v 1.1.4.4 2009/03/18 05:04:42 chill35 Exp $
(function ($) {
  // Documentation on Drupal JavaScript behaviors can be found here: http://drupal.org/node/114774#javascript-behaviors
  Drupal.behaviors.plus1 = {
    attach: function(context){
      jQuery('.'+ Drupal.settings.plus1.widget_class +':not(.plus1-processed)', context).addClass('plus1-processed').each(function(){
        var plus1_widget = jQuery(this);
        plus1_widget.find('.'+ Drupal.settings.plus1.link_class).attr('href', function(){ return jQuery(this).attr('href') + '&json=true'; }).click(function(){
          jQuery.getJSON(jQuery(this).attr('href'), function(json){
            plus1_widget.find('.'+ Drupal.settings.plus1.score_class).hide().fadeIn('slow').html(json.score);
            plus1_widget.find('.'+ Drupal.settings.plus1.message_class).html(json.voted);
          });
          // Preventing the /plus1/vote/<nid> target from being triggered.
          return false;
        });
      });
    }
  };
})(jQuery)