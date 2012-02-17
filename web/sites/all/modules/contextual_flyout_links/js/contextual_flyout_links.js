/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

(function ($) {
  /**
   * Override the default behavior and style by moving the edit gear to the left
   * of the element and outside the content area.
   *
   * Adds edge detection so that the links aren't displaying off screen
   */

  Drupal.contextualFlyoutLinks = Drupal.contextualFlyoutLinks || {};

  /**
   * UI interaction delay timeout IDs are stored in the delayTimeouts array
   */
  Drupal.contextualFlyoutLinks.delayTimeouts = [];


  Drupal.behaviors.contextualFlyoutLinks = {
    attach: function (context) {
      // Set up contextual links for logged in users, but not when the themebuilder is open
      if ($('body').hasClass('logged-in') && !$('body').hasClass('themebuilder')) {
        // Create the proxy links overlay
        // This will contain the contextual links and be placed over the
        // active contextual links region
        var $proxy = $('<div class="contextual-links-region-proxy" style="display:none;"></div>')
          .prependTo('body')
          .bind('mouseleave.contextualFlyoutLinks',
        {
          'functions': [
            {
              'action': Drupal.contextualFlyoutLinks.deactivateProxy,
              'delay': 300
            }
          ]
        }, Drupal.contextualFlyoutLinks.createDelay);
        var $boxTop = $('<div>').addClass('contextual-links-region-proxy-outline outline-top').insertAfter($proxy);
        var $boxRight = $('<div>').addClass('contextual-links-region-proxy-outline outline-right').insertAfter($boxTop);
        var $boxBottom = $('<div>').addClass('contextual-links-region-proxy-outline outline-bottom').insertAfter($boxRight);
        $('<div>').addClass('contextual-links-region-proxy-outline outline-left').insertAfter($boxBottom);

        // Process each instance of a contextual link set
        $('div.contextual-links-wrapper', context).once('contextual-flyout-links', function () {
          var $wrapper = $(this),
              $region = $wrapper.closest('div.contextual-links-region'),
              $trigger = $('<a class="contextual-links-trigger" href="#" />').text(Drupal.t('Configure'));
          $wrapper.prepend($trigger);
          Drupal.contextualFlyoutLinks.establishBindings($region, $wrapper);
          // Hide the core contextual links. We keep them in the DOM for accessibility.
          $wrapper.hide();
        });
      }
    }
  };
  
  Drupal.contextualFlyoutLinks.establishBindings = function ($region, $wrapper) {
    var $proxy = $('.contextual-links-region-proxy');

    // Listen for events
    $region.bind('mouseenter.contextualFlyoutLinks',
      {
        'region': $region,
        'wrapper': $wrapper,
        'functions': [
          {'action': Drupal.contextualFlyoutLinks.activateProxy, 'delay': 150 }
        ]
      }, Drupal.contextualFlyoutLinks.createDelay);
    $region.bind('mouseleave.contextualFlyoutLinks',
      {
        'region': $region,
        'wrapper': $wrapper,
        'functions': [
          {'action': Drupal.contextualFlyoutLinks.deactivateProxy, 'delay': 350 }
        ]
      }, Drupal.contextualFlyoutLinks.createDelay);
  };

  Drupal.contextualFlyoutLinks.activateProxy = function (event) {
    // Get the contextual links proxy and empty it of any links already in it
    var $proxy = $('.contextual-links-region-proxy');
    $proxy.hide().empty();

    // Clone the current contextual links and add them to the proxy region
    var $contextLinks = event.data.wrapper.clone();
    // Remove any inlined styling.
    $contextLinks.removeAttr('style');
    $contextLinks.appendTo($proxy);

    // Get the offset of the original region and position the proxy over it
    var $region = event.data.region;

    //Place the proxy
    Drupal.contextualFlyoutLinks.placeProxy($proxy, $region);

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
    $proxy.nextAll('.contextual-links-region-proxy-outline').andSelf().show();

    // Check for collision of the gear icon with the viewport edge
    event.data.element = $trigger;
    Drupal.contextualFlyoutLinks.queueEdgeCollisionCorrection(event);
  };
  Drupal.contextualFlyoutLinks.deactivateProxy = function (event) {
    $('.contextual-links-region-proxy').nextAll('.contextual-links-region-proxy-outline').andSelf().hide();
  };
  Drupal.contextualFlyoutLinks.placeProxy = function ($proxy, $region) {
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

      // Place the dotted outlines. The 'spacing' and 'adjust' add a little
      // space between the dotted lines and the content.
      var spacing = 3,
          adjust = 1;
      $('.contextual-links-region-proxy-outline.outline-top').css({
        'left': (offset.left - adjust),
        'height': '1px',
        'top': (offset.top - adjust),
        'width': (regionWidth + spacing)
      });
      $('.contextual-links-region-proxy-outline.outline-right').css({
        'left': (offset.left + regionWidth + adjust),
        'height': (regionHeight + spacing),
        'top': (offset.top - adjust),
        'width': '1px'
      });
      $('.contextual-links-region-proxy-outline.outline-bottom').css({
        'left': (offset.left - adjust),
        'height': '1px',
        'top': (offset.top + regionHeight + adjust),
        'width': (regionWidth + spacing)
      });
      $('.contextual-links-region-proxy-outline.outline-left').css({
        'left': (offset.left - adjust),
        'height': (regionHeight + spacing),
        'top': (offset.top - adjust),
        'width': '1px'
      });
      return true;
    }
    return false;
  };
  Drupal.contextualFlyoutLinks.showLinks = function (event) {
    var $links = $(event.currentTarget).next();
    event.data.element = $links;
    // Setting the visibility to hidden hides the element, but allows us to get
    // measurements for placement because it renders invisibly.
    $links.css('visibility', 'hidden');
    // Remove the display:none so that we can measure the element
    $links.show();
    // Check for collision of the links with the viewport edge
    Drupal.contextualFlyoutLinks.queueEdgeCollisionCorrection(event);
    // Hide this element before removing the invisibility
    $links.hide();
    // Remove the invisibility
    $links.css('visibility', 'visible');
    // Slide down and reveal
    $links.slideDown(125);
  };
  Drupal.contextualFlyoutLinks.hideLinks = function (event) {
    $(event.currentTarget).slideUp(200);
  };
  Drupal.contextualFlyoutLinks.createDelay = function (event) {
    // Clear the existing timeouts first
    Drupal.contextualFlyoutLinks.destroyDelay();
    // Go through the functions passed in the event and call them with their
    // designated delay.
    for (var i = 0; i < event.data.functions.length; i++) {
      var delay = event.data.functions[i].delay || 500;
      var action = event.data.functions[i].action;
      // @todo This implementation suffers from improper handling of closures
      // when calling the setTimeout function. It works now because only
      // one function is ever called, even though the loop is present to
      // support more than one function being passed in on event.data.functions.
      // A more robust approach that deals with the function closure is needed.
      Drupal.contextualFlyoutLinks.delayTimeouts.unshift(setTimeout(function () {
          action(event);
          event = null;
        }, delay)
      );
    }
  };
  Drupal.contextualFlyoutLinks.destroyDelay = function () {
    // Go through the timeout IDs and clear them
    while (Drupal.contextualFlyoutLinks.delayTimeouts.length > 0) {
      clearTimeout(Drupal.contextualFlyoutLinks.delayTimeouts.pop());
    }
  };
  Drupal.contextualFlyoutLinks.queueEdgeCollisionCorrection = function (event) {
    // Queue the collision detection after any animations that may be
    // associated with the links
    event.data.element.queue(function (next) {
      // If the object was just hidden, we don't need to act on it further. Just return out.
      if (event.data.element.is(':hidden')) {
        // Call the next animation in the fx queue and return
        next();
        return false;
      }
      // If the element is visible, attempt to correct any edge collision
      Drupal.contextualFlyoutLinks.correctEdgeCollision(event);
      // Call the next animation in the fx queue
      next();
      return true;
    });
  };
  Drupal.contextualFlyoutLinks.correctEdgeCollision = function (event) {
    // Check for RTL setting
    var isRTL = ($('html').attr('dir') === 'rtl'),
        isLinks = event.data.element.is('.contextual-links'),
        isTrigger = event.data.element.is('.contextual-links-trigger');
    // Remove the edge-collision class if it exists before we determine a collision
    if (isTrigger) {
      event.data.element.parent().removeClass('edge-collision');
    }
    if (isLinks) {
      event.data.element.removeClass('edge-collision');
    }
    // Get the dimensions of the element and the viewport
    var element = {
        offset: event.data.element.offset(),
        dimensions: {
          width: event.data.element.outerWidth(false)
        }
      },
      viewport = {
        dimensions: {
          width: $('html').width()
        }
      };
    // Determine if the element is rendered outside the viewport
    var isCollided = ((!isRTL && element.offset.left < 0) || (isRTL && ((element.offset.left + element.dimensions.width) > viewport.dimensions.width)));

    // If the left edge of the links is outside the viewport, move them inside the content; or,
    // If the right edge of the links is outside the viewport, move them inside the content
    if (isCollided && isLinks) {
      event.data.element.addClass('edge-collision');
    }
    // If the trigger has collided with the window edge, mark its parent as having collided
    if (isCollided && isTrigger) {
      event.data.element.parent().addClass('edge-collision');
    }
  };
})(jQuery);
