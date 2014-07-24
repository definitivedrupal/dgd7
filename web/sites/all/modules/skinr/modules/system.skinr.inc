<?php
/**
 * @file
 * Implements Skinr hooks for system.module.
 */

/**
 * Implements hook_skinr_config_info().
 */
function system_skinr_config_info() {
  return array('system');
}

/**
 * Implements hook_skinr_ui_element_options().
 */
function system_skinr_ui_element_options($theme_name = NULL) {
  $options = array('system' => array());

  $options['system']['html'] = t('Page');
  foreach (list_themes() as $theme_name => $theme) {
    if (empty($theme->status)) {
      continue;
    }
    // Create a list options containing visible regions of this theme.
    $regions = array();
    foreach (system_region_list($theme_name, REGIONS_VISIBLE) as $region_name => $region) {
      $regions['region__' . $region_name] = $region;
    }
    // Group the list of options by theme.
    $key = t('@name Regions', array('@name' => $theme->info['name']));
    $options['system'][$key] = $regions;
  }

  return $options;
}

/**
 * Implements hook_skinr_ui_element_title().
 */
function system_skinr_ui_element_title($module, $element, $theme_name) {
  if ($module == 'system') {
    $regions = system_region_list($theme_name);

    $title = t('Page');
    if (strpos($element, 'region') === 0) {
      // Strip the region__ part off the region name.
      $region = substr($element, 8);
      $title = t('Region %region', array('%region' => isset($regions[$region]) ? $regions[$region] : $region));
    }
    return $title;
  }
}

/**
 * Implements hook_skinr_theme_hooks().
 */
function system_skinr_theme_hooks($module, $element) {
  $theme_hooks = array();

  if ($module == 'system') {
    if ($element == 'html') {
      $theme_hooks[] = 'html';
    }
    else {
      $theme_hooks[] = $element;
      $theme_hooks[] = 'region';
    }
  }

  return $theme_hooks;
}

/**
 * Implements hook_skinr_elements().
 */
function system_skinr_elements($variables, $hook) {
  $elements = array();
  if ($hook == 'html') {
    $elements['system'] = array('html');
  }
  elseif ($hook == 'region') {
    $elements['system'] = array('region__' . $variables['region']);
  }
  return $elements;
}
