<?php

/**
 * @file
 * Page callbacks for the Skinr Context UI module related to editing skins and groups.
 */


/**
 * Menu callback; prepares some variables and displays a Skinr edit form.
 *
 * @param $module
 *   The module that we're editing settings of.
 * @param $element
 *   The element of the object we're editing settings of.
 * @param $group
 *   Skinr settings group object.
 */
function skinr_context_ui_edit($module, $element, $group) {
  module_load_include('inc', 'skinr_ui', 'skinr_ui.edit');

  // Set defaults.
  $defaults = skinr_context_ui_form_get_defaults($module, $element, $group);
  return drupal_get_form('skinr_ui_form', array('module' => $module, 'element' => $element, 'gid' => $group->gid), $defaults);
}

/**
 * Helper function to get the default values for the skinr edit form.
 *
 * @param $module
 *   The module that we're editing settings of.
 * @param $element
 *   The element of the object we're editing settings of.
 * @param $group
 *   Skinr settings group object.
 */
function skinr_context_ui_form_get_defaults($module, $element, $group) {
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
      'gid' => $group->gid,
    );
    // Don't nest the call to skinr_context_group_get_sids() in skinr_skin_load_multiple().
    // If the prior function returns no results, the second function will load
    // ALL skins.
    if ($sids = skinr_context_group_get_sids($params)) {
      $skins = skinr_skin_load_multiple($sids);
      foreach ($skins as $skin) {
        $defaults[$theme->name][$skin->skin] = $skin->options;
      }
    }
  }

  return $defaults;
}

/**
 * Menu callback; adds a skin settings group to an element.
 *
 * @param $module
 *   The module that we're adding a group to.
 * @param $element
 *   The element of the object we're adding a group to.
 */
function skinr_context_ui_group_add($module, $element) {
  return drupal_get_form('skinr_context_ui_group_add_form', $module, $element);
}

function skinr_context_ui_group_add_form($form, $form_state, $module, $element) {
  $form['module'] = array(
    '#type' => 'hidden',
    '#value' => $module,
  );
  $form['element'] = array(
    '#type' => 'hidden',
    '#value' => $element,
  );
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Group title'),
    '#required' => TRUE,
    '#description' => t('Descriptive title for this skin settings group.'),
  );
  $form['gid'] = array(
    '#type' => 'machine_name',
    '#title' => t('Group name'),
    // '#maxlength' => MENU_MAX_MENU_NAME_LENGTH_UI,
    '#description' => t('A unique name to identify this group. It must only contain lowercase letters, numbers, hyphens and underscores.'),
    '#field_prefix' => $module . ':' . $element . ':',
    '#machine_name' => array(
      'exists' => 'skinr_context_ui_group_name_exists',
      'source' => array('title'),
      'replace_pattern' => '[^a-z0-9-_]+',
    ),
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('A description for this group.'),
  );

  $form['actions'] = array(
    '#tree' => FALSE,
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add group'),
  );

  return $form;
}

/**
 * Menu callback; allows adding a new skin settings group.
 */
function skinr_context_ui_group_add_form_submit($form, $form_state) {
  $group = (object) array(
    'gid' => $form_state['values']['module'] . ':' . $form_state['values']['element'] . ':' . $form_state['values']['gid'],
    'module' => $form_state['values']['module'],
    'element' => $form_state['values']['element'],
    'title' => $form_state['values']['title'],
    'description' => $form_state['values']['description'],
    'conditions' => array('sitewide' => array('values' => array(1 => 1))),
    'condition_mode' => CONTEXT_CONDITION_MODE_OR,
    'weight' => 0,
    'status' => 1,
  );
  skinr_context_group_save($group);
  drupal_goto('admin/structure/skinr/edit/' . $group->module . '/' . $group->element . '/' . $group->gid);
}

/**
 * Form builder for the skinr settings group delete confirmation form.
 *
 * @param $group
 *   Skinr settings group object.
 *
 * @ingroup forms
 */
function skinr_context_ui_group_delete_confirm($form, &$form_state, $group) {
  $form['#group'] = $group;
  $form['gid'] = array(
    '#type' => 'value',
    '#value' => $group->gid,
  );

  return confirm_form($form,
    t('Are you sure you want to delete this skin settings group?'),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/skinr/edit/' . $group->module . '/' . $group->element,
    t('This action cannot be undone.<br />Module: %module<br />Element: %element<br />Group: %group', array('%module' => $group->module, '%element' => $group->element, '%group' => $group->title)),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Form submission handler for skinr_context_ui_group_delete_confirm().
 */
function skinr_context_ui_group_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    skinr_context_group_delete($form_state['values']['gid']);
    watchdog('skinr', 'Deleted a skin settings group.');
    drupal_set_message(t('A skin settings group has been deleted.'));
  }

  $group = $form['#group'];
  $form_state['redirect'] = 'admin/structure/skinr/edit/' . $group->module . '/' . $group->element;
}

/**
 * Menu callback; lists skin settings groups for an element.
 *
 * @param $module
 *   The module that we're editing settings of.
 * @param $element
 *   The element of the object we're editing settings of.
 */
function skinr_context_ui_group_list($module, $element) {
  // Output list of groups.
  $gids = skinr_context_group_get_gids(array(
    'module' => $module,
    'element' => $element,
  ));
  $groups = skinr_context_group_load_multiple($gids);

  return drupal_get_form('skinr_context_ui_group_list_form', $groups);
}

/**
 * Form builder for the skin settings group listing.
 */
function skinr_context_ui_group_list_form($form, $form_state, $groups) {
  // Weights range from -delta to +delta, so delta should be at least half
  // of the amount of blocks present. This makes sure all blocks in the same
  // region get an unique weight.
  $weight_delta = round(count($groups) / 2);

  $form['groups'] = array();
  $form['#tree'] = TRUE;
  $form['#empty_text'] = t("You don't have any groups for this element.");

  foreach ($groups as $gid => $group) {
    $group = (array) $group;

    $form['groups'][$gid]['info'] = array(
      '#markup' => theme('skinr_context_ui_group_summary', array('title' => $group['title'], 'description' => $group['description'])),
    );
    $form['groups'][$gid]['status'] = array(
      '#type' => 'checkbox',
      '#default_value' => $group['status'],
      '#title_display' => 'invisible',
      '#title' => t('Enable @group group', array('@group' => $group['title'])),
    );
    $form['groups'][$gid]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $group['weight'],
      '#delta' => $weight_delta,
      '#title_display' => 'invisible',
      '#title' => t('Weight for @group group', array('@group' => $group['title'])),
    );

    $destination = array();
    if (isset($_GET['destination'])) {
      $path = $_GET['q'];
      $query = drupal_http_build_query(drupal_get_query_parameters());
      if ($query != '') {
        $path .= '?' . $query;
      }
      $destination = array('destination' => $path);
    }

    $operations = array();
    $operations['edit'] = array(
      '#type' => 'link',
      '#title' => t('edit'),
      '#href' => 'admin/structure/skinr/edit/' . $group['module'] . '/' . $group['element'] . '/' . $group['gid'],
      '#options' => array('query' => $destination),
    );
    $operations['delete'] = array(
      '#type' => 'link',
      '#title' => t('delete'),
      '#href' => 'admin/structure/skinr/edit/' . $group['module'] . '/' . $group['element'] . '/' . $group['gid'] . '/delete',
      '#options' => array('query' => $destination),
    );
    $form['groups'][$gid]['operations'] = $operations;
  }

  // Prepare cancel link.
  if (isset($_GET['destination'])) {
    $options = drupal_parse_url(urldecode($_GET['destination']));
  }
  else {
    $options = array('path' => 'admin/structure/skinr');
  }

  $form['actions'] = array(
    '#tree' => FALSE,
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save groups'),
  );
  $form['actions']['done'] = array(
    '#type' => 'link',
    '#title' => t('Done'),
    '#href' => $options['path'],
    '#options' => $options,
  );

  return $form;
}

/**
 * Form submission handler for skinr_context_ui_group_list_form().
 */
function skinr_context_ui_group_list_form_submit($form, $form_state) {
  foreach ($form_state['values']['groups'] as $gid => $data) {
    // Load an uncached version of the skin settings group object.
    $group = skinr_context_group_load_unchanged($gid);
    // Let's save some time in skinr_context_group_save() by setting $group->original here.
    $group->original = clone($group);

    // Update status and weight.
    $group->status = $data['status'];
    $group->weight = $data['weight'];
    skinr_context_group_save($group);
  }
}


// -----------------------------------------------------------------------
// Theme functions.

/**
 * Returns HTML for the menu overview form into a table.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_skinr_context_ui_group_list_form($variables) {
  $form = $variables['form'];

  drupal_add_tabledrag('skinr-context-ui-group-list', 'order', 'sibling', 'skinr-context-ui-group-weight');

  $header = array(
    t('Skin settings group'),
    array('data' => t('Enabled'), 'class' => array('checkbox')),
    t('Weight'),
    array('data' => t('Operations'), 'colspan' => '2'),
  );

  $rows = array();
  foreach (element_children($form['groups']) as $gid) {
    if (isset($form['groups'][$gid]['status'])) {
      $element = &$form['groups'][$gid];
      // Build a list of operations.
      $operations = array();
      foreach (element_children($element['operations']) as $op) {
        $operations[] = array('data' => drupal_render($element['operations'][$op]), 'class' => array('skinr-context-ui-group-operations'));
      }

      // Add special classes to be used for tabledrag.js.
      $element['weight']['#attributes']['class'] = array('skinr-context-ui-group-weight');

      $row = array();
      $row[] = drupal_render($element['info']);
      $row[] = array('data' => drupal_render($element['status']), 'class' => array('checkbox', 'skinr-context-ui-group-enabled'));
      $row[] = drupal_render($element['weight']);
      $row = array_merge($row, $operations);

      $row = array_merge(array('data' => $row), $element['#attributes']);
      $row['class'][] = 'draggable';
      $rows[] = $row;
    }
  }
  $output = '';
  if (empty($rows)) {
    $rows[] = array(array('data' => $form['#empty_text'], 'colspan' => '7'));
  }
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'skinr-context-ui-group-list')));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Returns HTML for title and descrition info for the skin settings groups listing.
 *
 * @param $variables
 *   An associative array containing:
 *   - title: The group's title.
 *   - description: The group's description.
 *
 * @ingroup themeable
 */
function theme_skinr_context_ui_group_summary($variables) {
  $info = '<strong class="title">' . $variables['title'] . '</strong>';
  if (!empty($variables['description'])) {
    $info .= '<div class="description">' . $variables['description'] . '</div>';
  }
  return '<div class="skin-group-summary">' . $info . '</div>';
}
