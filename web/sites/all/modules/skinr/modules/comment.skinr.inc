<?php
/**
 * @file
 * Implements Skinr hooks for comment.module.
 */

/**
 * Implements hook_skinr_config_info().
 */
function comment_skinr_config_info() {
  return array('comment');
}

/**
 * Implements hook_skinr_ui_element_options().
 */
function comment_skinr_ui_element_options($theme_name = NULL) {
  $options = array('comment' => array());

  $types = node_type_get_types();
  foreach ($types as $type) {
    $options['comment'][$type->type] = $type->name;
  }
  asort($options['comment']);

  return $options;
}

/**
 * Implements hook_skinr_ui_element_title().
 */
function comment_skinr_ui_element_title($module, $element, $theme_name) {
  if ($module == 'comment') {
    $type = node_type_get_type($element);
    return $type->name;
  }
}

/**
 * Implements hook_skinr_theme_hooks().
 */
function comment_skinr_theme_hooks($module, $element) {
  $theme_hooks = array();

  if ($module == 'comment') {
    $theme_hooks = array(
      'comment_wrapper__' . $element,
      'comment_wrapper',
    );
  }

  return $theme_hooks;
}

/**
 * Implements hook_skinr_elements().
 */
function comment_skinr_elements($variables, $hook) {
  $elements = array();
  if ($hook == 'comment_wrapper') {
    $elements['comment'] = array($variables['node']->type);
  }
  return $elements;
}
