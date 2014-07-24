<?php

/**
 * @file
 * Implements features integration for Skinr.
 */

/**
 * Implements hook_features_api().
 */
function skinr_features_api() {
  return array(
    'skinr_skin' => array(
      'name' => t('Skinr skins'),
      'default_hook' => 'skinr_skin_defaults',
      'default_file' => FEATURES_DEFAULTS_CUSTOM,
      'default_filename' => 'skinr_default',
      'feature_source' => TRUE,
      'api' => 'skin',
      'file' => drupal_get_path('module', 'skinr') .'/skinr.features.inc',
    ),
  );
}

/**
 * Implements hook_features_export().
 */
function skinr_skin_features_export($data, &$export, $module_name) {
  $export['dependencies']['skinr_skin'] = 'skinr';

  foreach ($data as $uuid) {
    $export['features']['skinr_skin'][$uuid] = $uuid;
  }
  return array();
}

/**
 * Implements hook_features_export_options().
 */
function skinr_skin_features_export_options() {
  $skin_info = skinr_get_skin_info();
  $themes = list_themes();

  $options = array();
  foreach (skinr_skin_load_multiple(FALSE) as $skin) {
    $title = skinr_invoke_all('skinr_ui_element_title', $skin->module, $skin->element, $skin->theme);
    $title = reset($title);
    $name = $skin->skin == '_additional' ? t('Additional classes') : (isset($skin_info[$skin->skin]) ? $skin_info[$skin->skin]['title'] : $skin->skin);

    $options[$skin->uuid] = t('@theme: @type: @element: @name', array(
      '@type' => $skin->module,
      '@element' => $title ? strip_tags($title) : $skin->element,
      '@name' => $name,
      '@theme' => isset($themes[$skin->theme]) ? $themes[$skin->theme]->info['name'] : $skin->theme,
    ));
  }
  asort($options);

  return $options;
}

/**
 * Implements hook_features_export_render().
 */
function skinr_skin_features_export_render($module_name, $data, $export = NULL) {
  $code = array();
  // $code[] = '  $skins = array();';
  // $code[] = '';
  foreach ($data as $uuid) {
    if ($skin = skinr_skin_load_by_uuid($uuid)) {
      $code[] = skinr_skin_export($skin, '  ');
    }
  }
  $code[] = '';
  $code[] = '  return $skins;';
  $code = implode("\n", $code);

  return array('skinr_skin_defaults' => $code);
}

/**
 * Implements hook_features_rebuild().
 */
function skinr_skin_features_rebuild($module_name) {
  skinr_skin_defaults_import($module_name, TRUE);
}

/**
 * Implements hook_features_revert().
 */
function skinr_skin_features_revert($module_name) {
  skinr_skin_defaults_import($module_name, TRUE);
}
