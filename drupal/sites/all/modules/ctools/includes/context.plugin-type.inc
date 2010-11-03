<?php
// $Id: context.plugin-type.inc,v 1.1 2010/10/11 22:18:22 sdboyer Exp $

/**
 * @file
 * Contains plugin type registration information for the context tool.
 *
 * Don't actually need to declare anything for these plugin types right now,
 * apart from the fact that they exist. So, an empty array.
 */

function ctools_context_plugin_type(&$items) {
  $items['contexts'] = array();
  $items['arguments'] = array();
  $items['relationships'] = array();
  $items['access'] = array(
    'child plugins' => TRUE,
  );
}
