(function ($) {

/**
 * Attach schemaorg behavior for autocomplete.
 */
Drupal.behaviors.schemaorgs = {
  attach: function(context) {
    $.getJSON(Drupal.settings.schemaorgapiTermsPath, function(data) {
      $("input.schemaorg-autocomplete-types").autocomplete({
        source: data.types,
        delay: 0,
      });
      $("input.schemaorg-autocomplete-properties").autocomplete({
        source: data.properties,
        delay: 0,
      });
   });

  }
};


})(jQuery);
