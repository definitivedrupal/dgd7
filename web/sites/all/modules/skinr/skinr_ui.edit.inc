<?php

/**
 * @file
 * Page callbacks for the Skinr UI module related to editing skins.
 */


/**
 * Menu callback; prepares some variables and displays a Skinr edit form.
 *
 * @param $module
 *   The module that we're editing settings of.
 * @param $element
 *   The element of the object we're editing settings of.
 */
function skinr_ui_edit($module, $element) {
  $defaults = skinr_ui_form_get_defaults($module, $element);
  return drupal_get_form('skinr_ui_form', array('module' => $module, 'element' => $element), $defaults);
}

/**
 * Helper function to get the default values for the skinr edit form.
 *
 * @param $module
 *   The module that we're editing settings of.
 * @param $element
 *   The element of the object we're editing settings of.
 */
function skinr_ui_form_get_defaults($module, $element) {
  // Set defaults.
  $defaults = array();

  $themes = list_themes();
  foreach ($themes as $theme) {
    if (!$theme->status) {
      continue;
    }

    $params = array(
      'theme' => $theme->name,
      'module' => $module,
      'element' => $element,
    );
    // Don't nest the call to skinr_skin_get_sids() in skinr_skin_load_multiple().
    // If the prior functions returns no results, the second function will load
    // ALL skins.
    if ($sids = skinr_skin_get_sids($params)) {
      $skins = skinr_skin_load_multiple($sids);
      foreach ($skins as $skin) {
        $defaults[$theme->name][$skin->skin] = $skin->options;
      }
    }
  }

  return $defaults;
}

/**
 * Form builder for the skins configuration form.
 *
 * @param $module
 *   The module that we're editing settings of.
 * @param $element
 *   The element of the object we're editing settings of.
 * @param $defaults
 *   An array of default values keyed by theme name.
 *
 * @ingroup forms
 */
function skinr_ui_form($form, &$form_state, $arguments, $defaults) {
  if (!skinr_ui_access('edit skin settings')) {
    // Deny access.
    return;
  }

  // Set arguments.
  $module = $arguments['module'];
  $element = $arguments['element'];

  // Set default values.
  $form_state['values']['skinr_settings'] = $defaults;

  // Set form class.
  $form['#attributes'] = array('class' => array('skinr-form'));

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 50,
  );

  $form['skinr']['module'] = array(
    '#type' => 'hidden',
    '#value' => $module,
  );
  $form['skinr']['element'] = array(
    '#type' => 'hidden',
    '#value' => $element,
  );

  $groups = skinr_get_group_info();
  $skin_infos = skinr_get_skin_info();

  // Apply overridden status to skins.
  foreach ($skin_infos as $skin_name => $skin_info) {
    $skin_infos[$skin_name]['status'] = skinr_skin_info_status_get($skin_infos[$skin_name]);
  }

  // Invoke hook_skinr_theme_hooks() and hook_skinr_theme_hooks_alter().
  $theme_hooks = skinr_theme_hooks($module, $element);

  $form['skinr_settings'] = array(
    '#tree' => TRUE,
    '#type' => 'container',
    // Move settings to top.
    '#weight' => 0,
  );


  $themes = list_themes();
  ksort($themes);
  // Get current theme, but make sure it's not the admin theme when we're editing with AJAX.
  $current_theme = skinr_current_theme(TRUE);

  foreach ($themes as $theme) {
    if (!$theme->status) {
      continue;
    }

    // If this hook is a region, and the region does not exist for this
    // theme, don't bother outputting any of the settings.
    if (strpos($theme_hooks[0], 'region') === 0) {
      // Strip the region__ part off the region name.
      $region = substr($theme_hooks[0], 8);

      $regions = system_region_list($theme->name, REGIONS_VISIBLE);
      if (!isset($regions[$region])) {
        continue;
      }
    }

    // Load defaults for theme sub-form.
    $defaults = isset($form_state['values']['skinr_settings']) ? $form_state['values']['skinr_settings'] : array();

    // Build theme sub-form.
    $form['skinr_settings'][$theme->name] = array(
      '#type' => 'fieldset',
      '#title' => $theme->info['name'] . ($theme->name == $current_theme ? ' (' . t('enabled + default') . ')' : ''),
      '#collapsible' => TRUE,
      '#collapsed' => $theme->name == $current_theme ? FALSE : TRUE,
    );
    if ($theme->name == $current_theme) {
      // Current theme goes at the top.
      $form['skinr_settings'][$theme->name]['#attributes'] = array('class' => array('skinr-ui-current-theme'));
      $form['skinr_settings'][$theme->name]['#weight'] = -10;

      // Use vertical tabs.
      $form['skinr_settings'][$theme->name]['groups'] = array(
        '#type' => 'vertical_tabs',
      );
    }

    // Create individual widgets for each skin.
    foreach ($skin_infos as $skin_name => $skin_info) {
      // Check if this skin is disabled.
      if (empty($skin_info['status'][$theme->name])) {
        continue;
      }

      // Check if this skin applies to this hook.
      if (!is_array($skin_info['theme hooks']) || (!in_array('*', $skin_info['theme hooks']) && !_skinr_is_featured($theme_hooks, $skin_info['theme hooks']))) {
        continue;
      }

      // Create widget.
      $field = array();
      if (!empty($skin_info['form callback'])) {
        // Process custom form callbacks.

        // Load include file.
        if (!empty($skin_info['source']['include file'])) {
          skinr_load_include($skin_info['source']['include file']);
        }

        // Execute form callback.
        if (function_exists($skin_info['form callback'])) {
          $context = array(
            'theme' => $theme->name,
            'skin_name' => $skin_name,
            'skin_info' => $skin_info,
            'value' => isset($defaults[$theme->name][$skin_name]) ? $defaults[$theme->name][$skin_name] : array(),
          );
          $field = $skin_info['form callback']($form, $form_state, $context);
        }
      }
      else {
        switch ($skin_info['type']) {
          case 'checkboxes':
            $field = array(
              '#type' => 'checkboxes',
              '#multiple' => TRUE,
              '#title' => t($skin_info['title']),
              '#options' => skinr_ui_info_options_to_form_options($skin_info['options']),
              '#default_value' => isset($defaults[$theme->name][$skin_name]) ? $defaults[$theme->name][$skin_name] : array(),
              '#description' => t($skin_info['description']),
              '#weight' => isset($skin_info['weight']) ? $skin_info['weight'] : NULL,
            );
            break;
          case 'radios':
            $field = array(
              '#type' => 'radios',
              '#title' => t($skin_info['title']),
              '#options' => array_merge(array('' => '&lt;none&gt;'), skinr_ui_info_options_to_form_options($skin_info['options'])),
              '#default_value' => isset($defaults[$theme->name][$skin_name]) ? $defaults[$theme->name][$skin_name] : '',
              '#description' => t($skin_info['description']),
              '#weight' => isset($skin_info['weight']) ? $skin_info['weight'] : NULL,
            );
            break;
          case 'select':
            $field = array(
              '#type' => 'select',
              '#title' => t($skin_info['title']),
              '#options' => array_merge(array('' => '<none>'), skinr_ui_info_options_to_form_options($skin_info['options'])),
              '#default_value' => isset($defaults[$theme->name][$skin_name]) ? $defaults[$theme->name][$skin_name] : '',
              '#description' => t($skin_info['description']),
              '#weight' => isset($skin_info['weight']) ? $skin_info['weight'] : NULL,
            );
            break;
          default:
            // Raise an error.
            drupal_set_message(t("Widget %name's type is invalid.", array('%name' => $skin_name)), 'error', FALSE);
            break;
        }
      }
      if (empty($skin_info['group']) || empty($groups[$skin_info['group']])) {
        $form['skinr_settings'][$theme->name][$skin_name] = $field;
      }
      else {
        if (!empty($field) && !isset($form['skinr_settings'][$theme->name]['groups'][$skin_info['group']])) {
          $group = $groups[$skin_info['group']];
          $form['skinr_settings'][$theme->name]['groups'][$skin_info['group']] = array(
            '#type' => 'fieldset',
            '#title' => t($group['title']),
            '#description' => t($group['description']),
            '#weight' => isset($group['weight']) ? $group['weight'] : NULL,
          );
        }
        $form['skinr_settings'][$theme->name]['groups'][$skin_info['group']][$skin_name] = $field;
      }
    }

    // Check for access.
    if (skinr_ui_access('edit advanced skin settings')) {
      $skin_name = '_additional';
      $form['skinr_settings'][$theme->name]['groups']['_additional'] = array(
        '#type' => 'fieldset',
        '#title' => t('Advanced'),
        '#weight' => 50,
      );
      $form['skinr_settings'][$theme->name]['groups']['_additional']['_additional'] = array(
        '#type' => 'textfield',
        '#title' => t('CSS classes'),
        '#size' => 40,
        '#description' => t('To add CSS classes manually, enter classes separated by a single space i.e. <code>first-class second-class</code>'),
        '#default_value' => isset($defaults[$theme->name][$skin_name]) ? $defaults[$theme->name][$skin_name] : '',
      );
    }
  }

  // Only add validation handler once.
  if (!isset($form['#validate']) || !in_array('skinr_ui_form_validate', $form['#validate'])) {
    $form['#validate'][] = 'skinr_ui_form_validate';
  }

  // Only add submit handler once.
  if (!isset($form['#submit']) || !in_array('skinr_ui_form_submit', $form['#submit'])) {
    $form['#submit'][] = 'skinr_ui_form_submit';
  }

  return $form;
}

/**
 * Form validation handler for skinr_ui_form_alter().
 */
function skinr_ui_form_validate($form, &$form_state) {
  $module = $form_state['values']['module'];
  $element = $form_state['values']['element'];

  $error = FALSE;
  if (isset($form_state['values']['skinr_settings'])) {
    foreach ($form_state['values']['skinr_settings'] as $theme_name => $theme) {
      if (isset($theme['groups']['_additional']['_additional'])) {
        // Validate additional classes field.
        if (preg_match('/[^a-zA-Z0-9\-\_\s]/', $theme['groups']['_additional']['_additional'])) {
          form_set_error('skinr_settings][' . $theme_name . '][groups][_additional][_additional', t('Additional classes for Skinr may only contain alphanumeric characters, spaces, - and _.'));
          $error = TRUE;
        }
      }
    }
  }

  if (!$error) {
    $groups = skinr_get_group_info();
    // Add hard-coded additional classes group.
    $groups['_additional'] = array(
      'title' => 'Additional',
      'description' => 'Additional custom classes.',
      'weight' => 0,
    );

    if (!empty($form_state['values']['skinr_settings'])) {
      foreach ($form_state['values']['skinr_settings'] as $theme_name => $theme) {
        // Unset active tab variables.
        foreach ($theme['groups'] as $skin_name => $options) {
          if (strpos($skin_name, '__groups__active_tab') !== FALSE) {
            unset($form_state['values']['skinr_settings'][$theme_name]['groups'][$skin_name]);
            continue;
          }
        }
        // Undo any grouping to ease processing on submit.
        foreach ($groups as $group_name => $group) {
          if (!empty($theme['groups'][$group_name]) && is_array($theme['groups'][$group_name])) {
            $group_values = $theme['groups'][$group_name];
            unset($form_state['values']['skinr_settings'][$theme_name]['groups'][$group_name]);
            $form_state['values']['skinr_settings'][$theme_name] = array_merge($form_state['values']['skinr_settings'][$theme_name], $group_values);
          }
        }
        unset($form_state['values']['skinr_settings'][$theme_name]['groups']);
      }
    }
  }
}

/**
 * Form submission handler for skinr_ui_form_alter().
 */
function skinr_ui_form_submit($form, &$form_state) {
  $current_theme = skinr_current_theme(TRUE);

  $module = $form_state['values']['module'];
  $element = $form_state['values']['element'];

  if (!empty($form_state['values']['skinr_settings'])) {
    foreach ($form_state['values']['skinr_settings'] as $theme_name => $theme) {
      // Process widgets.
      if (!empty($theme) && is_array($theme)) {
        foreach ($theme as $skin_name => $options) {
          if ($skin_name == '_additional' && !user_access('edit advanced skin settings')) {
            // This user doesn't have access to alter these options.
            continue;
          }

          // Ensure options is an array.
          if (!is_array($options)) {
            $options = $skin_name == '_additional' ? explode(' ', $options) : array($options);
          }
          // Sanitize options.
          $options = _skinr_array_strip_empty($options);

          // Find existing skin.
          $params = array(
            'theme' => $theme_name,
            'module' => $module,
            'element' => $element,
            'skin' => $skin_name,
          );
          $sids = skinr_skin_get_sids($params);

          unset($skin);
          if (!empty($sids)) {
            $sid = reset($sids);
            $skin = skinr_skin_load($sid);
          }

          if (empty($options)) {
            if (!empty($skin)) {
              // Delete this skin configuration.
              skinr_skin_delete($skin->sid);
            }
            continue;
          }

          if (empty($skin)) {
            // It doesn't exist, so create a new skin.
            $skin = new stdClass();
            $skin->theme = $theme_name;
            $skin->module = $module;
            $skin->element = $element;
            $skin->skin = $skin_name;
          }
          $skin->options = $options;
          $skin->status = 1;

          // Save skin.
          if (!skinr_skin_save($skin)) {
            drupal_set_message(t("Skinr settings for %skin weren't saved due to an error.", array('%skin' => $skin_name)), 'error');
          }
        }
      }
    }
  }
}

/**
 * Helper function to determine whether one of a set of hooks exists in a list
 * of required theme hooks.
 *
 * @param $theme_hooks
 *   An array of theme hooks available to this element.
 * @param $allowed_hooks
 *   An array of allowed theme hooks.
 *
 * @return
 *   TRUE if an overlap is found, FALSE otherwise.
 *
 * @todo Rename function to be more descriptive.
 */
function _skinr_is_featured($theme_hooks, $allowed_hooks) {
  foreach ($theme_hooks as $theme_hook) {
    if (in_array($theme_hook, $allowed_hooks)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Helper function to convert an array of options, as specified in the .info
 * file, into an array usable by Form API.
 *
 * @param $options
 *   An array containing at least the 'class' and 'label' keys.
 *
 * @return
 *   A Form API compatible array of options.
 *
 * @todo Rename function to be more descriptive.
 */
function skinr_ui_info_options_to_form_options($options) {
  $form_options = array();
  foreach ($options as $option_name => $option) {
    $form_options[$option_name] = t($option['title']);
  }
  return $form_options;
}
