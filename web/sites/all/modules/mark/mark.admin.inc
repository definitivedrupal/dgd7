<?php
// $Id: mark.admin.inc,v 1.2.4.4 2010/09/17 18:17:40 tristanoneil Exp $
/**
 * @file
 * Administrative functions for the mark module.
 */

/**
 * Form generation for the marks settings page
 */
function mark_settings_form() {
  $form = array('#theme' => 'mark_listing_form');
  $form['marks'] = array('#value' => mark_load());

  module_load_include('inc', 'views', 'includes/admin');
  $base_tables = array();
  foreach (views_fetch_base_tables() as $table => $info) {
    $base_tables[$table] = $info['title'] .': '. $info['description'];
  }

  $form['new'] = array(
    '#tree' => true,
    '#type' => 'fieldset',
    '#title' => t('Create a new mark'),
  );
  $form['new']['basetable'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => $base_tables,
    '#description' => t('Create a mark for items from this table.'),
    '#required' => true,
  );
  $form['new']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#maxlength' => 255,
    '#size' => 15,
    '#required' => true,
    '#element_validate' => array('mark_form_name_validate'),
    '#description' => t('Machine name for the mark; numbers, lowercase letters and underscores only.'),
  );
  $form['new']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new mark'),
    '#submit' => array('mark_settings_create'),
  );
  return $form;
}

/**
 * Validate mark name values.
 */
function mark_form_name_validate($element, &$form_state) {
  // Check for string identifier sanity
  if (!preg_match('!^[a-z0-9_-]+$!', $element['#value'])) {
    form_set_error('name', t('The mark name can only consist of lowercase letters, underscores, and numbers.'));
  }
  // Check for name collision
  else if ($exists = mark_load($element['#value'])) {
    form_set_error('name', t('A mark with this name already exists. Please choose another name or delete the existing mark before creating a new one.'));
  }
}

/**
 * Submit handler for mark creation
 */
function mark_settings_create($form, &$form_state) {
  $mark = new stdClass();
  $mark->basetable = $form_state['values']['new']['basetable'];
  $mark->name = $form_state['values']['new']['name'];

  if (drupal_write_record('mark', $mark) == SAVED_NEW) {
    drupal_set_message(t('Create new mark: %name', array('%name' => $mark->name)));
    $form_state['redirect'] = 'admin/structure/mark/' . $mark->name;
  }
  else {
    drupal_set_message(t('Failed to write new mark to the database.'), 'error');
  }
}

/**
 * Mark edit form
 */
function mark_edit_form($form, $form_state, $mark) {
  $form = array();
  $form['export_type'] = array(
    '#type' => 'value',
    '#value' => $mark->export_type,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $mark->name,
  );
  $form['basetable'] = array(
    '#type' => 'value',
    '#value' => $mark->basetable,
  );
  $form['settings'] = array(
    '#tree' => true,
  );
  $form['settings']['title'] = array(
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#title' => t('Title'),
    '#default_value' => $mark->settings['title'],
    '#description' => t('Brief description of the mark.'),
  );
  $form['settings']['mark_text'] = array(
    '#title' => t('Mark button text'),
    '#type' => 'textfield',
    '#default_value' => $mark->settings['mark_text'],
    '#description' => t('Text displayed for un-marked items.'),
  );
  $form['settings']['mark_confirm_text'] = array(
    '#title' => t('Mark confirmation text'),
    '#type' => 'textfield',
    '#default_value' => $mark->settings['mark_confirm_text'],
    '#description' => t('Message shown to a user who has just marked an item.'),
  );
  $form['settings']['unmark_text'] = array(
    '#title' => t('Un-Mark button text'),
    '#type' => 'textfield',
    '#default_value' => $mark->settings['unmark_text'],
    '#description' => t('Text displayed for marked items.'),
  );
  $form['settings']['unmark_confirm_text'] = array(
    '#title' => t('Un-Mark confirmation text'),
    '#type' => 'textfield',
    '#default_value' => $mark->settings['unmark_confirm_text'],
    '#description' => t('Message shown to a user who has just un-marked an item.'),
  );
  $form['settings']['global'] = array(
    '#title' => t('Global'),
    '#type' => 'checkbox',
    '#default_value' => isset($mark->settings['global']) ? $mark->settings['global'] : FALSE,
    '#description' => t('By default a mark is specific to a particular user. Making a mark global shares the same mark among all users'),
  );
  $form['new']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submission handler for mark editing.
 */
function mark_edit_form_submit($form, $form_state) {
  $mark = $form_state['values'];
  if ($mark['export_type'] & EXPORT_IN_DATABASE) {
    if (drupal_write_record('mark', $mark, 'name') == SAVED_UPDATED) {
      $updated = true;
    }
  }
  else {
    if (drupal_write_record('mark', $mark) == SAVED_NEW) {
      $updated = true;
    }
  }

  if ($updated) {
    // Clear the views cache so our changes are visible.
    views_invalidate_cache();
    drupal_set_message(t('Updated mark: %name', array('%name' => $mark['name'])));
  }
  else {
    drupal_set_message(t('Failed to write mark update to the database.'), 'error');
  }
}

/**
 * Mark delete/revert form
 */
function mark_delete_form($form, &$form_state, $mark) {
  if ($mark->export_type & EXPORT_IN_CODE) {
    $action = t('revert');
    $message = '';
  }
  else {
    $action = t('delete');
    $message = t("This action cannot be undone.");
  }

  $form = array();
  $form['mark'] = array('#type' => 'value', '#value' => $mark);
  $form['action'] = array('#type' => 'value', '#value' => $action);
  $form = confirm_form($form,
    t('Are you sure you want to !action the mark: %title?', array('%title' => $mark->name, '!action' => $action)),
    'admin/structure/mark',
    $message,
    drupal_ucfirst($action), t('Cancel')
  );
  return $form;
}

/**
 * Submission handler for mark deletion/reversion.
 */
function mark_delete_form_submit($form, &$form_state) {
  db_query("DELETE FROM {mark} WHERE name = :name", array(':name' => $form_state['values']['mark']->name));
  if ($form_state['values']['action'] == t('revert')) {
    drupal_set_message(t('%mark has been reverted', array('%mark' => $form_state['values']['mark']->name)));
  }
  else {
    drupal_set_message(t('%mark has been deleted', array('%mark' => $form_state['values']['mark']->name)));
  }
  $form_state['redirect'] = 'admin/structure/mark';
}

/**
 * Mark disable/enable form
 */
function mark_status_form($form_state, $mark) {
  if (empty($mark->disabled)) {
    $action = t('disable');
    $message = '';
  }
  else {
    $action = t('enable');
    $message = '';
  }
  $form = array();
  $form['mark'] = array('#type' => 'value', '#value' => $mark);
  $form['action'] = array('#type' => 'value', '#value' => $action);
  $form = confirm_form($form,
    t('Are you sure you want to !action the mark: %title?', array('%title' => $mark->name, '!action' => $action)),
    'admin/structure/mark',
    $message,
    drupal_ucfirst($action), t('Cancel')
  );
  return $form;
}

/**
 * Submission handler for mark disabling/enabline.
 */
function mark_status_form_submit($form, &$form_state) {
  if ($form_state['values']['action'] == t('disable')) {
    ctools_export_set_object_status($context, FALSE);
    drupal_set_message(t('%mark has been disabled', array('%mark' => $form_state['values']['mark']->name)));
  }
  else {
    ctools_export_set_object_status($context);
    drupal_set_message(t('%mark has been enabled', array('%mark' => $form_state['values']['mark']->name)));
  }
  $form_state['redirect'] = 'admin/structure/mark';
}
