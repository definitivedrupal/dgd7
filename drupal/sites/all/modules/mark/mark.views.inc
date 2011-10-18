<?php
// $Id: mark.views.inc,v 1.1 2010/02/22 15:22:05 jmiccolis Exp $

/**
 * Implementation of hook_views_data_alter.
 */
function mark_views_data_alter(&$data) {
  $marks = mark_load();
  foreach ($marks as $name => $mark) {
    if (isset($data[$mark->basetable]['table']['base'])) {
      $data[$mark->basetable]['mark_'. $name] = array(
        'field' => array(
          'title' => $mark->settings['title'],
          'help' => t('Provide a marking ui'),
          'handler' => 'views_handler_field_mark',
          'real field' => $data[$mark->basetable]['table']['base']['field'],
        ),
      );
    }
  }
}

/**
 * Implementation of hook_views_handlers().
 */
function mark_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'mark') . '/include',
    ),
    'handlers' => array(
      // field handlers
      'views_handler_field_mark' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}
 
