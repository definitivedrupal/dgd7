<?php
// $Id: simplenews_access.admin.inc,v 1.2 2010/03/15 22:39:37 sutharsan Exp $

/**
 * @file simplenews_access.admin.inc
 * 
 * Provide administration for simplenews access.
 *
 * @ingroup simplenews_access
 */

/**
 * Menu callback: Simplenews Access administration by role.
 */
function simplenews_access_admin_by_role() {
  // Get stored permissions.
  $grants = simplenews_access_get_grants();
 
  // Retrieve simplenews newsletter categories.
  $categories = simplenews_get_categories();

  // Retrieve role names for columns.
  $role_names = user_roles();

  $permissions = array(
    'grant_view'   => t('View'),
    'grant_update' => t('Update'),
    'grant_delete' => t('Delete'),
  );

  // Store $role_names for use when saving the data.
  $form['role_names'] = array(
    '#type' => 'value',
    '#value' => $role_names,
  );

  // Render role/permission overview:
  $options = array();
  foreach ($categories as $scid => $category_name) {
    $form['permission'][] = array(
      '#markup' => $category_name,
      '#id' => $scid,
    );
    foreach ($permissions as $perm => $perm_name) {
      $perm_id = $scid . ':' . $perm;
      $options[$perm_id] = '';
      $form['permission'][$perm_id] = array(
        '#type' => 'item',
        '#markup' => $perm_name,
        '#checkbox-title' => check_plain($category_name) . ' - ' . $perm_name,
      );
      foreach ($role_names as $rid => $name) {
        // Builds arrays for checked boxes for each role
        if (isset($grants[$scid][$rid][$perm])) {
          $status[$rid][] = $grants[$scid][$rid][$perm] ? $perm_id : 0;
        }
      }
    }
  }

  // Have to build checkboxes here after checkbox arrays are built
  foreach ($role_names as $rid => $name) {
    $form['checkboxes'][$rid] = array('#type' => 'checkboxes', '#options' => $options, '#default_value' => isset($status[$rid]) ? $status[$rid] : array());
    $form['role_names'][$rid] = array('#markup' => $name, '#tree' => TRUE);
  }

  $form['actions'] = array('#type' => 'container', '#attributes' => array('class' => array('form-actions')));
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save permissions'));

  $form['#attached']['js'][] = drupal_get_path('module', 'user') . '/user.permissions.js';

  return $form;
}

/**
 * Process result from Simplenews Access permission administration form.
 */
function simplenews_access_admin_by_role_submit($form, &$form_state) {
  $values = $form_state['values'];
  $grants = array();
  
  // Convert the user input into a structured grants array.
  foreach ($values as $rid => $permissions) {
    if (is_numeric($rid)) {
      foreach ($permissions as $perm_id => $value) {
        list($scid, $perm) = explode(':', $perm_id);
        $grants[$scid][$rid][$perm] = $value ? 1 : 0;
      }
    }
  }

  // Save the grants in the simplenews_access table.
  foreach ($grants as $scid => $grant) {
    foreach ($grant as $rid => $perms) {
      $query = db_merge('simplenews_access')
        ->key(array('scid' => $scid, 'rid' => $rid))
        ->fields(array(
          'grant_view'   => $perms['grant_view'],
          'grant_update' => $perms['grant_update'],
          'grant_delete' => $perms['grant_delete'],
        ))
        ->execute();
    }
  }

  // Inform the user to rebuild the node access table when he is ready.
  node_access_needs_rebuild(TRUE);
}

/**
 * Format the Simplenews Access admin table.
 *
 * @ingroup themeable
 */
function theme_simplenews_access_admin_by_role($variables) {
  $form = $variables['form'];
  $roles = user_roles();
  foreach (element_children($form['permission']) as $key) {
    $row = array();
    // Category name
    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form['permission'][$key]), 'class' => array('module'), 'id' => 'module-' . $form['permission'][$key]['#id'], 'colspan' => count($form['role_names']['#value']) + 1);
    }
    else {
      // Permission row.
      $row[] = array(
        'data' => drupal_render($form['permission'][$key]),
        'class' => array('permission'),
      );
      foreach (element_children($form['checkboxes']) as $rid) {
        $row[] = array('data' => drupal_render($form['checkboxes'][$rid][$key]), 'class' => array('checkbox'), 'title' => $roles[$rid] . ' : ' . $form['permission'][$key]['#checkbox-title']);
      }
    }
    $rows[] = $row;
  }
  $header[] = (t('Access'));
  foreach (element_children($form['role_names']) as $rid) {
    $header[] = array('data' => drupal_render($form['role_names'][$rid]), 'class' => array('checkbox'));
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'permissions')));
  $output .= drupal_render_children($form);
  return $output;
}