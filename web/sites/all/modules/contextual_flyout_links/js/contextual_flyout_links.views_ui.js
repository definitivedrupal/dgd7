/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

(function ($) {
  /**
   * Views override for the Contextual Flyout Links
   */

  Drupal.viewsContextualLinks = Drupal.viewsContextualLinks || {};
  
  Drupal.behaviors.viewsContextualLinks = {
    attach: function (context) {
      // Listen for events
      $('div.contextual-links-wrapper', context).once('views-contextual-links', function () {
        var $wrapper = $(this),
            $region = $wrapper.closest('div.contextual-links-region'),
            $target = $region.find('div.preview-section');
        // This unbind is necessary to bypass the default contextualFlyoutLinks behavior.
        $region.unbind('mouseenter.contextualFlyoutLinks');
        Drupal.viewsContextualLinks.establishBindings($region, $wrapper, $target);
      });
    }
  };

  Drupal.viewsContextualLinks.establishBindings = function ($region, $wrapper, $target) {
    var $proxy = $('.contextual-links-region-proxy');

    // Listen for events
    $region.bind('mouseenter.contextualFlyoutLinks_views',
      {
        'region': $region,
        'wrapper': $wrapper,
        'target': $target,
        'functions': [
          {'action': Drupal.viewsContextualLinks.activateProxy, 'delay': 150 }
        ]
      }, Drupal.contextualFlyoutLinks.createDelay);
  };

  Drupal.viewsContextualLinks.activateProxy = function (event) {
    // Get the contextual links proxy and empty it of any links already in it
    var $proxy = $('.contextual-links-region-proxy');
    $proxy.hide().empty();

    // Clone the current contextual links and add them to the proxy region
    var $contextLinks = event.data.wrapper.clone();
    // Remove any inlined styling.
    $contextLinks.removeAttr('style');
    $contextLinks.appendTo($proxy);

    // Get the offset of the original region and position the proxy over it
    var $target = event.data.target;

    //Place the proxy
    Drupal.viewsContextualLinks.placeProxy($proxy, $target);

    // Get the trigger and the links to bind events to them.
    var $trigger = $('.contextual-links-trigger', $proxy),
        $links = $('.contextual-links', $proxy);

    // Establish bindings
    // Close the proxy if a contextual action link is clicked
    $('a', $links).bind('click.contextualFlyoutLinks',
      {},
      Drupal.contextualFlyoutLinks.deactivateProxy);
    // Show the links when the gear is hovered
    $trigger.bind('mouseenter.contextualFlyoutLinks',
      {
        'functions': [
          { 'action': Drupal.contextualFlyoutLinks.showLinks, 'delay': 150 }
        ]
      }, Drupal.contextualFlyoutLinks.createDelay);
    // Hide the links when the user mouses out
    $links.bind('mouseleave.contextualFlyoutLinks',
      {
        'functions': [
          { 'action': Drupal.contextualFlyoutLinks.hideLinks, 'delay': 50 }
        ]
      }, Drupal.contextualFlyoutLinks.createDelay);

    // Show the proxy and dotted lines
    $proxy.show();

    // Check for collision of the gear icon with the viewport edge
    event.data.element = $trigger;
    Drupal.contextualFlyoutLinks.queueEdgeCollisionCorrection(event);
  };

  Drupal.viewsContextualLinks.placeProxy = function ($proxy, $region) {
    if ($proxy && $region) {
      var offset = $region.offset(),
          regionWidth = $region.width(),
          regionHeight = $region.height();

      // Place the proxy element
      $proxy
        .css({
          'left' : offset.left,
          'top' : offset.top,
          'width' : $region.width(),
          'height' : 0
        });
      return true;
    }
    return false;
  };
})(jQuery);
