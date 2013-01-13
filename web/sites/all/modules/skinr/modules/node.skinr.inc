<?php
/**
 * @file
 * Implements Skinr hooks for node.module.
 */

/**
 * Implements hook_skinr_config_info().
 */
function node_skinr_config_info() {
  return array('node');
}

/**
 * Implements hook_skinr_ui_element_options().
 */
function node_skinr_ui_element_options($theme_name = NULL) {
  $options = array('node' => array());

  $types = node_type_get_types();
  foreach ($types as $type) {
    $options['node'][$type->type] = $type->name;
  }
  asort($options['node']);

  return $options;
}

/**
 * Implements hook_skinr_ui_element_title().
 */
function node_skinr_ui_element_title($module, $element, $theme_name) {
  if ($module == 'node') {
    $type = node_type_get_type($element);
    return $type->name;
  }
}

/**
 * Implements hook_skinr_theme_hooks().
 */
function node_skinr_theme_hooks($module, $element) {
  $theme_hooks = array();

  if ($module == 'node') {
    $theme_hooks = array(
      'node__' . $element,
      'node',
    );
  }

  return $theme_hooks;
}

/**
 * Implements hook_skinr_elements().
 */
function node_skinr_elements($variables, $hook) {
  $elements = array();
  if ($hook == 'node') {
    $elements['node'] = array($variables['node']->type);
  }
  return $elements;
}
