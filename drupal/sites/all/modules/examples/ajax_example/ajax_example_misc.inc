<?php
// $Id: ajax_example_misc.inc,v 1.2 2010/03/23 02:26:24 rfay Exp $
/**
 * @file
 * AJAX Miscellaneous Topics.
 *
 *
*/

/**
 * Demonstrates a clickable AJAX-enabled link.
 *
 * Because of the 'use-ajax' class applied here, the link submission is done
 * via AJAX.
 *
 * This will not work if ajax.js is not loaded on the page.
 * @return unknown_type
 */
function ajax_example_render_link() {
  drupal_add_js('misc/ajax.js');
  $explanation = "
The link below has the <i>use-ajax</i> class applied to it, so if
javascript is enabled, ajax.js will try to submit it via an AJAX call instead
of a normal page load. The URL also contains the '/nojs/' magic string, which
is stripped if javascript is enabled, allowing the server code to tell by the
URL whether JS was enabled or not, letting it do different things based on that.";
  $output = "<div>" . t($explanation);
  $link = l(t('Click here'), 'ajax_link_callback/nojs/', array('attributes' => array('class' => array('use-ajax'))));
  $output .= "<div>$link</div><div id='myDiv'></div>";
  return $output;
}

/**
 * Callback for link example.
 *
 * Takes different logic paths based on whether Javascript was enabled.
 * If $type == 'ajax', it tells this function that ajax.js has rewritten
 * the URL and thus we are doing an AJAX and can return an array of commands.
 * @param $type
 *   Either 'ajax' or 'nojs. Type is simply the normal URL argument to this
 *   URL.
 * @return
 *   If $type == 'ajax', returns an array of AJAX Commands.
 *   Otherwise, just returns the content, which will end up being a page.
 * @see ajax
 */
function ajax_link_response($type = 'ajax') {
  if ($type == 'ajax') {
    $output = t("This is some content delivered via AJAX");
    $commands = array();
    $commands[] = ajax_command_append('#myDiv', $output);
    $page = array('#type' => 'ajax', '#commands' => $commands);
    ajax_deliver($page);
  }
  else {
    $output = t("This is some content delivered via a page load.");
    return $output;
  }
}
