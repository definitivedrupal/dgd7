
/**
 * Quick tooltips behavior.
 */
Drupal.behaviors.filePermissionsTooltip = function(context) {
  var $tooltip, timerID = 0;
  $('.field-permissions-status:not(.field-permissions-processed)', context).addClass('field-permissions-processed').each(function() {
    var $icon = $(this), $cell = $icon.parents('td:first'), $tip = $cell.find('.field-permissions-tooltip');
    if (!$tip.size()) {
      return;
    }
    if (!$tooltip) {
      if (!$('#field-permissions-tooltip').size()) {
        $tooltip = $('<div id="field-permissions-tooltip"></div>');
        $tooltip.appendTo(document.body);
        $tooltip.css({position: 'absolute'});
      }
      else {
        $tooltip = $('#field-permissions-tooltip');
      }
    }
    $cell.hover(
      function() {
        if (timerID) {
          clearTimeout(timerID);
          timerID = 0;
        }
        var offset = $cell.offset(), tooltipTop = offset.top + $cell.outerHeight() + 4, tooltipLeft = (offset.left + $cell.outerWidth()) - $tip.outerWidth();
        $tooltip.html($tip.html()).css({left: tooltipLeft, top: tooltipTop}).fadeIn('normal');
      },
      function() {
        if (timerID) {
          clearTimeout(timerID);
        }
        timerID = setTimeout(function() {
          $tooltip.fadeOut('fast');
        }, 100);
      }
    );
  });
};
