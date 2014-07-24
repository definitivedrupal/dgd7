<?php

/**
 * @file
 * Implements features integration for Skinr Context.
 */

/**
 * Implements hook_features_api().
 */
function skinr_context_features_api() {
  return array(
    'skinr_group' => array(
      'name' => t('Skinr groups'),
      'default_hook' => 'skinr_context_group_defaults',
      'default_file' => FEATURES_DEFAULTS_CUSTOM,
      'default_filename' => 'skinr_default',
      'feature_source' => TRUE,
      'api' => 'skin',
      'file' => drupal_get_path('module', 'skinr_context') .'/skinr_context.features.inc',
    ),
  );
}

/**
 * Implements hook_features_export().
 */
function skinr_group_features_export($data, &$export, $module_name) {
  $export['dependencies']['skinr_group'] = 'skinr_context';

  foreach ($data as $gid) {
    $export['features']['skinr_group'][$gid] = $gid;
  }
  return array();
}

/**
 * Implements hook_features_pipe_COMPONENT_alter().
 */
function skinr_context_features_pipe_skinr_skin_alter(&$pipe, $data, $export) {
  // Skin configuration objects should always include the Skin settings groups
  // they depend on.
  foreach ($data as $uuid) {
    if (($skin = skinr_skin_load_by_uuid($uuid)) && !empty($skin->gid)) {
      if ($group = skinr_context_group_load($skin->gid)) {
        $pipe['skinr_group'][] = $group->gid;
      }
    }
  }
}

/**
 * Implements hook_features_export_options().
 */
function skinr_group_features_export_options() {
  // Some title callbacks require a theme; for lack of better data use the
  // default theme.
  $theme = skinr_current_theme(TRUE);

  $options = array();
  foreach (skinr_context_group_load_multiple(FALSE) as $group) {
    $title = skinr_invoke_all('skinr_ui_element_title', $group->module, $group->element, $theme);
    $title = reset($title);
    $options[$group->gid] = t('@type: @element: @group (@gid)', array(
      '@type' => $group->module,
      '@element' => $title ? strip_tags($title) : $group->element,
      '@group' => $group->title,
      '@gid' => $group->gid,
    ));
  }
  asort($options);

  return $options;
}

/**
 * Implements hook_features_export_render().
 */
function skinr_group_features_export_render($module_name, $data, $export = NULL) {
  $code = array();
  // $code[] = '  $groups = array();';
  // $code[] = '';
  foreach ($data as $gid) {
    if ($group = skinr_context_group_load($gid)) {
      $code[] = skinr_context_group_export($group, '  ');
    }
  }
  $code[] = '';
  $code[] = '  return $groups;';
  $code = implode("\n", $code);

  return array('skinr_context_group_defaults' => $code);
}

/**
 * Implements hook_features_rebuild().
 */
function skinr_group_features_rebuild($module_name) {
  skinr_context_group_defaults_import($module_name, TRUE);
}

/**
 * Implements hook_features_revert().
 */
function skinr_group_features_revert($module_name) {
  skinr_context_group_defaults_import($module_name, TRUE);
}
