// $Id: comment-node-form.js,v 1.5 2010/04/16 13:55:06 dries Exp $

(function ($) {

Drupal.behaviors.commentFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-comment-settings', context).drupalSetSummary(function (context) {
      return Drupal.checkPlain($('input:checked', context).next('label').text());
    });
    // Provide the summary for the node type form.
    $('fieldset#edit-comment', context).drupalSetSummary(function(context) {
      var vals = [];

      // Default comment setting.
      vals.push($("select[name='comment'] option:selected", context).text());

      // Threading.
      var threading = $("input[name='comment_default_mode']:checked", context).next('label').text();
      if (threading) {
        vals.push(threading);
      }

      // Comments per page.
      var number = $("select[name='comment_default_per_page'] option:selected", context).val();
      vals.push(Drupal.t('@number comments per page', {'@number': number}));

      return Drupal.checkPlain(vals.join(', '));
    });
  }
};

})(jQuery);
