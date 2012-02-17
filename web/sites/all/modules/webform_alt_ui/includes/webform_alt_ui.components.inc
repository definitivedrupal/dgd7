<?php

/**
 * @file Alters the configuration forms for webform components
 */

/**
 * Alters the configuration form for textfields
 */
function _webform_alt_ui_textfield_configure($form) {

  return $form;
}

/**
 * Alters the configuration form for textareas
 */
function _webform_alt_ui_textarea_configure($form) {

  return $form;
}

/**
 * Alters the configuration form for radios
 */
function _webform_alt_ui_radios_configure($form) {
  $form = _webform_alt_ui_option_configure($form);

  return $form;
}

/**
 * Alters the configuration form for checkboxes
 */
function _webform_alt_ui_checkboxes_configure($form) {
  $form = _webform_alt_ui_option_configure($form);

  return $form;
}

/**
 * Alters the configuration form for selects
 */
function _webform_alt_ui_select_configure($form) {
  $form = _webform_alt_ui_option_configure($form);

  return $form;
}

/**
 * Alters the configuration form for emails
 */
function _webform_alt_ui_email_configure($form) {

  return $form;
}

/**
 * Alters the configuration form for file uploads
 */
function _webform_alt_ui_file_configure($form) {
  // Deny access to fields we don't want configured
  $form['webform_file_savelocation']['#access'] = FALSE;
  $form['webform_file_filtering']['addextensions']['#access'] = FALSE;

  // Alterations for the webform_file_width field
  $form['webform_file_width']['#title'] = 'Size';
  $form['webform_file_width']['#description'] = NULL;

  $groups = element_children($form['webform_file_filtering']['types']);

  $form['webform_file_filtering']['types']['#prefix'] = '<label>Allow the following types:</label>';

  $options = array();
  $defaults = array();
  $settings = array('all' => array());

  $form['webform_file_filtering']['types']['type_groups'] = array(
    '#type' => 'checkboxes',
    '#options' => array('all' => 'All'),
    '#attributes' => array('class' => array('type-groups')),
  );

  foreach($groups as $key) {
    // Remove disallowed extensions from the options.
    // @todo Is it appropriate for this code to be in webform_alt_ui module, or
    //   is it really more of a product/hosting decision?
    foreach($form['webform_file_filtering']['types'][$key]['#options'] as $extension) {
      if (in_array($extension, array('jar', 'dmg'))) {
        unset($form['webform_file_filtering']['types'][$key]['#options'][$extension]);
      }
    }
    $options = array_merge($options, $form['webform_file_filtering']['types'][$key]['#options']);
    $defaults = array_merge($defaults, $form['webform_file_filtering']['types'][$key]['#default_value']);

    $form['webform_file_filtering']['types']['type_groups']['#options'][$key] = 'All ' . str_replace('image', ' image', $key);
    $settings[$key] = $form['webform_file_filtering']['types'][$key]['#options'];
    $settings['all'] = array_merge($settings['all'], $form['webform_file_filtering']['types'][$key]['#options']);
    unset($form['webform_file_filtering']['types'][$key]);
  }

  $form['webform_file_filtering']['types']['type_groups']['#default_value'] = 'webimages';

  $form['webform_file_filtering']['types']['file_types'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $defaults,
    '#attributes' => array('class' => array('file-types')),
  );

  // Set an absolute size limit and a new default.
  // @todo It's possible that the #default_value line below is no longer needed
  //   due to webform_alt_ui_form_builder_types_alter(), but needs more testing
  //   before it can be removed. The default must be altered in the other
  //   function, because by the time we get here, we can't distinguish between
  //   the webform module default and a prior saved configuration value for this
  //   component.
  $form['webform_file_filtering']['size']['#default_value'] = isset($form['#_edit_element']['#webform_file_filtering']['size']) ? $form['#_edit_element']['#webform_file_filtering']['size'] : 1000;
  $form['webform_file_filtering']['size']['#element_validate'][] = 'webform_alt_ui_file_size_validate';
  $form['webform_file_filtering']['size']['#description'] = t('Enter the max file size (in KB) a user may upload (limit 20,000KB).');

  drupal_add_js(array('webform_alt_ui' => $settings), 'setting');

  $form['#attached']['js'][] = drupal_get_path('module', 'webform_alt_ui') . '/js/webform_alt_ui.file.js';

  return $form;
}

/**
 * Alters the configuration form for markup
 */
function _webform_alt_ui_markup_configure($form) {
  // Hide the title options.
  $form['title']['#access'] = FALSE;
  $form['title_display']['#access'] = FALSE;

  // Modify the title of the markup field.
  $form['markup']['#title'] = t('Formatted content');

  // Prevent the markup field from being hidden behind a checkbox.
  $form['markup']['#checkbox'] = FALSE;

  // Hide parts of the text format selector.
  _webform_alt_ui_add_text_format_simplification($form['markup']);

  return $form;
}

/**
 * Alters the configuration form for fieldsets
 */
function _webform_alt_ui_fieldset_configure($form) {
  $form['description']['#access'] = FALSE;

  return $form;
}

/**
 * Alters the configuration form for hidden fields
 */
function _webform_alt_ui_hidden_configure($form) {

  return $form;
}

/*
 * Alters the configuration form for all option elements.
 * 
 * Applies to select, checkboxes, and radios elements.
 */
function _webform_alt_ui_option_configure($form) {
  $form['options']['#process'] = array('form_options_expand', 'webform_alt_ui_option_process');

  return $form;
}
