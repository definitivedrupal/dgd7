<?php

/**
 * @file
 * Admin page callbacks for the Skinr Context UI module.
 */


/**
 * Implements hook_skinr_context_ui_operations().
 */
function skinr_context_ui_skinr_context_ui_operations() {
  $operations = array(
    'enable' => array(
      'label' => t('Enable selected skin configuration group'),
      'callback' => 'skinr_context_ui_mass_update',
      'callback arguments' => array('updates' => array('status' => 1)),
    ),
    'disable' => array(
      'label' => t('Disable selected skin configuration group'),
      'callback' => 'skinr_context_ui_mass_update',
      'callback arguments' => array('updates' => array('status' => 0)),
    ),
    'delete' => array(
      'label' => t('Delete selected skin configuration group'),
      'callback' => NULL,
    ),
  );
  return $operations;
}

/**
 * List skinr administration filters that can be applied.
 *
 * @return
 *   An array of filters.
 */
function skinr_context_ui_filters() {
  // Type filter.
  $config = skinr_get_config_info();

  $options = array('[any]' => t('any'));
  foreach ($config as $type) {
    $options[$type] = $type;
  }

  $filters['module'] = array(
    'title' => t('type'),
    'options' => $options,
  );

  // Element filter.
  $elements = db_query("SELECT module, element FROM {skinr_groups} GROUP BY element, module");

  $options = array('[any]' => t('any'));
  foreach ($elements as $element) {
    $title = skinr_ui_get_element_title($element->module, $element->element);
    $options[$element->element] = $title ? strip_tags($title) : $element->element;
  }

  $filters['element'] = array(
    'title' => t('element'),
    'options' => $options,
  );

  // Status filter.
  $filters['status'] = array(
    'title' => t('status'),
    'options' => array(
      '[any]' => t('any'),
      '1' => t('enabled'),
      '0' => t('disabled'),
    ),
  );

  // Allow modules to add filters.
  drupal_alter('skinr_context_ui_filters', $filters);

  return $filters;
}

/**
 * Apply filters for skin configuration administration filters based on session.
 *
 * @param $query
 *   A SelectQuery to which the filters should be applied.
 */
function skinr_context_ui_build_filter_query(SelectQueryInterface $query) {
  // Build query
  $filter_data = isset($_SESSION['skinr_context_ui_filters']) ? $_SESSION['skinr_context_ui_filters'] : array();
  foreach ($filter_data as $index => $filter) {
    list($key, $value) = $filter;
    $query->condition('g.' . $key, $value);
  }
}

/**
 * Mass update skin configuration groups, updating all skin configuration grouos
 * in the $groups array with the field values in $updates.
 *
 * IMPORTANT NOTE: This function is intended to work when called
 * from a form submit handler. Calling it outside of the form submission
 * process may not work correctly.
 *
 * @param array $groups
 *   Array of skin configuration gids to update.
 * @param array $updates
 *   Array of key/value pairs with skin configuration group field names and the
 *   value to update that field to.
 */
function skinr_context_ui_mass_update($groups, $updates) {
  // We use batch processing to prevent timeout when updating a large number
  // of groups.
  if (count($groups) > 10) {
    $batch = array(
      'operations' => array(
        array('_skinr_context_ui_mass_update_batch_process', array($groups, $updates))
      ),
      'finished' => '_skinr_context_ui_mass_update_batch_finished',
      'title' => t('Processing'),
      // We use a single multi-pass operation, so the default
      // 'Remaining x of y operations' message will be confusing here.
      'progress_message' => '',
      'error_message' => t('The update has encountered an error.'),
      // The operations do not live in the .module file, so we need to
      // tell the batch engine which file to load before calling them.
      'file' => drupal_get_path('module', 'skinr_context_ui') . '/skinr_context_ui.admin.inc',
    );
    batch_set($batch);
  }
  else {
    foreach ($groups as $gid) {
      _skinr_context_ui_mass_update_helper($gid, $updates);
    }
    drupal_set_message(t('The update has been performed.'));
  }
}

/**
 * Helper function for skin configuration group mass updates.
 */
function _skinr_context_ui_mass_update_helper($gid, $updates) {
  drupal_static_reset('skinr_context_group_load_multiple');
  $group = skinr_context_group_load($gid);
  // For efficiency manually store the original skin configuration group before
  // applying any changes.
  $group->original = clone $group;
  foreach ($updates as $name => $value) {
    $group->$name = $value;
  }
  skinr_context_group_save($group);
  return $group;
}

/**
 * Batch operation for skin configuration mass updates.
 */
function _skinr_context_ui_mass_update_batch_process($groups, $updates, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($groups);
    $context['sandbox']['groups'] = $groups;
  }

  // Process groups in batches of 5.
  $count = min(5, count($context['sandbox']['groups']));
  for ($i = 1; $i <= $count; $i++) {
    // For each sid, load the skin configuration, reset the values, and save it.
    $gid = array_shift($context['sandbox']['groups']);
    $group = _skinr_context_ui_mass_update_helper($gid, $updates);

    // Store result for post-processing in the finished callback.
    $context['results'][] = $group->title;

    // Update our progress information.
    $context['sandbox']['progress']++;
  }

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch 'finished' callback for skin configuration mass updates.
 */
function _skinr_context_ui_mass_update_batch_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('The update has been performed.'));
  }
  else {
    drupal_set_message(t('An error occurred and processing did not complete.'), 'error');
    $message = format_plural(count($results), '1 item successfully processed:', '@count items successfully processed:');
    $message .= theme('item_list', array('items' => $results));
    drupal_set_message($message);
  }
}

function skinr_context_ui_admin_group_storage($group) {
  $options = array(
    SKINR_STORAGE_IN_DATABASE => t('Normal'),
    SKINR_STORAGE_IN_CODE => t('Default'),
    SKINR_STORAGE_IN_CODE_OVERRIDDEN => t('Overridden'),
  );
  return $options[skinr_context_group_storage($group)];
}

/**
 * Menu callback: skin configuration groups administration.
 *
 * @ingroup forms
 */
function skinr_context_ui_admin_list($form, &$form_state) {
  module_load_include('inc', 'skinr_ui', 'skinr_ui.admin');

  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    return skinr_ui_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['skins']));
  }
  $form['filter'] = skinr_ui_filter_form('skinr_context_ui_filters', $form_state);
  $form['#submit'][] = 'skinr_ui_filter_form_submit';
  $form['admin'] = skinr_context_ui_admin_list_subform();

  return $form;
}

/**
 * Overrides skinr_ui_admin_skins().
 *
 * @see skinr_ui_admin_skins()
 */
function skinr_context_ui_admin_list_subform() {
  // Build the 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  foreach (module_invoke_all('skinr_context_ui_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'enable',
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#validate' => array('skinr_context_ui_admin_groups_validate'),
    '#submit' => array('skinr_context_ui_admin_groups_submit'),
  );

  $header = array(
    'type' => array('data' => t('Type'), 'field' => 'g.module'),
    'element' => array('data' => t('Element'), 'field' => 'g.element'),
    'title' => array('data' => t('Title'), 'field' => 'g.title'),
    'description' => array('data' => t('Description'), 'field' => 'g.description'),
    'status' => array('data' => t('Status'), 'field' => 'g.status', 'sort' => 'desc'),
    'storage' => array('data' => t('Storage')),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('skinr_groups', 'g')->extend('PagerDefault')->extend('TableSort');
  skinr_context_ui_build_filter_query($query);

  $gids = $query
    ->fields('g', array('gid'))
    ->limit(50)
    ->orderByHeader($header)
    ->orderBy('module', 'asc')
    ->execute()
    ->fetchCol();
  $groups = skinr_context_group_load_multiple($gids);

  $current_theme = skinr_current_theme();
  $destination = drupal_get_destination();
  $options = array();
  foreach ($groups as $group) {
    $operations = array();
    $operations['edit'] = array(
      'title' => t('edit'),
      'href' => 'admin/structure/skinr/edit/' . $group->module . '/' . $group->element . '/' . $group->gid,
      'query' => $destination,
    );
    $operations['status'] = array(
      'title' => $group->status ? t('disable') : t('enable'),
      'href' => 'admin/structure/skinr/group/' . $group->gid . '/' . ($group->status ? 'disable' : 'enable'),
      'query' => $destination + array(
        'token' => drupal_get_token('admin/structure/skinr/group/' . $group->gid . '/' . ($group->status ? 'disable' : 'enable')),
      ),
    );
    $storage = skinr_context_ui_admin_group_storage($group);
    if ($storage == t('Overridden')) {
      $operations['revert'] = array(
        'title' => t('revert'),
        'href' => 'admin/structure/skinr/group/' . $group->gid . '/revert',
        'query' => $destination,
      );
    }
    if ($storage == t('Normal')) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'admin/structure/skinr/edit/' . $group->module . '/' . $group->element . '/' . $group->gid . '/delete',
        'query' => $destination,
      );
    }

    $element = skinr_ui_get_element_title($group->module, $group->element);
    $options[$group->gid] = array(
      'type' => $group->module,
      'element' => $element ? $element : $group->element,
      'title' => t('@group (!gid)', array('@group' => $group->title, '!gid' => $group->gid)),
      'description' => $group->description,
      'status' => $group->status ? t('enabled') : t('disabled'),
      'storage' => array('data' => $storage, 'class' => array('skinr-ui-storage')),
      'operations' => array(
        'data' => array(
          '#theme' => 'links__skinr_ui_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      ),
    );

    if (!$group->status) {
      $options[$group->gid]['#attributes'] = array('class' => array('skinr-ui-disabled'));
    }
  }

  // Hide status row. Only used for sorting.
  unset($header['status']);

  drupal_add_css(drupal_get_path('module', 'skinr_ui') . '/css/skinr-ui-admin.css');
  $form['groups'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
  );

  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}

/**
 * Form validation handler for skinr_context_ui_admin_list().
 *
 * Check if any skin configuration groups have been selected to perform the
 * chosen 'Update option' on.
 */
function skinr_context_ui_admin_groups_validate($form, &$form_state) {
  // Error if there are no items to select.
  if (!is_array($form_state['values']['groups']) || !count(array_filter($form_state['values']['groups']))) {
    form_set_error('', t('No items selected.'));
  }
}

/**
 * Form submission handler for skinr_context_ui_admin_list().
 *
 * Execute the chosen 'Update option' on the selected skin configuration groups.
 */
function skinr_context_ui_admin_groups_submit($form, &$form_state) {
  $operations = module_invoke_all('skinr_context_ui_operations');
  $operation = $operations[$form_state['values']['operation']];
  // Filter out unchecked nodes
  $groups = array_filter($form_state['values']['groups']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($groups), $operation['callback arguments']);
    }
    else {
      $args = array($groups);
    }
    call_user_func_array($function, $args);

    cache_clear_all();
  }
  else {
    // We need to rebuild the form to go to a second step. For example, to
    // show the confirmation form for the deletion of nodes.
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Apply filters for skin configuration administration filters based on session.
 *
 * @param $query
 *   A SelectQuery to which the filters should be applied.
 */
function skinr_context_ui_build_skin_filter_query(SelectQueryInterface $query) {
  // Build query
  $filter_data = isset($_SESSION['skinr_ui_filters']) ? $_SESSION['skinr_ui_filters'] : array();
  foreach ($filter_data as $index => $filter) {
    list($key, $value) = $filter;
    if ($key == 'gid') {
      // Make exception for gid, which is in a different table.
      $query->condition('gs.' . $key, $value);
    }
    else {
      $query->condition('s.' . $key, $value);
    }
  }
}

/**
 * Overrides skinr_ui_list().
 *
 * @see skinr_ui_list()
 */
function skinr_context_ui_admin_skin_list($form, &$form_state) {
  module_load_include('inc', 'skinr_ui', 'skinr_ui.admin');

  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    return skinr_ui_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['skins']));
  }
  $form['filter'] = skinr_ui_filter_form('skinr_ui_filters', $form_state);
  $form['#submit'][] = 'skinr_ui_filter_form_submit';

  $header = array(
    'theme' => array('data' => t('Theme'), 'field' => 's.theme'),
    'type' => array('data' => t('Type'), 'field' => 's.module'),
    'element' => array('data' => t('Element'), 'field' => 's.element'),
    'group' => array('data' => t('Group'), 'field' => 'gs.gid'),
    'skin' => array('data' => t('Skin'), 'field' => 's.skin'),
    'status' => array('data' => t('Status'), 'field' => 's.status', 'sort' => 'desc'),
    'storage' => array('data' => t('Storage')),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('skinr_skins', 's')->extend('PagerDefault')->extend('TableSort');
  $query->join('skinr_group_skins', 'gs', 's.sid = gs.sid');
  skinr_context_ui_build_skin_filter_query($query);

  $query
    ->fields('s', array('sid'))
    ->limit(50)
    ->orderByHeader($header);
  if (empty($_GET['order'])) {
    // Default sort.
    $query
      ->orderBy('s.status', 'desc')
      ->orderBy('s.theme')
      ->orderBy('s.module')
      ->orderBy('s.element')
      ->orderBy('gs.gid');
  }

  $sids = $query->execute()->fetchCol();
  $skins = skinr_skin_load_multiple($sids);

  $form['admin'] = skinr_ui_admin_skins($header, $skins);

  // Add in groups.
  $gids = array();
  foreach ($skins as $skin) {
    $gids[$skin->gid] = $skin->gid;
  }
  $groups = skinr_context_group_load_multiple($gids);

  foreach ($form['admin']['skins']['#options'] as $key => $option) {
    $gid = isset($option['#skin']->gid) ? $option['#skin']->gid : t('none');
    $form['admin']['skins']['#options'][$key]['group'] = $gid;
    if (isset($groups[$gid])) {
      $form['admin']['skins']['#options'][$key]['#group'] = $groups[$gid];
      $form['admin']['skins']['#options'][$key]['group'] = t('@group (!gid)', array('@group' => $groups[$gid]->title, '!gid' => $gid));

      // Alter edit link.
      $form['admin']['skins']['#options'][$key]['operations']['data']['#links']['edit']['href'] = 'admin/structure/skinr/edit/' . $option['#skin']->module . '/' . $option['#skin']->element . '/' . $option['#skin']->gid;
    }
  }

  return $form;
}

/**
 * Menu callback; Updates the skin configuration group's status.
 */
function skinr_context_ui_group_status_set($group, $status) {
  // We require a token in the query string to prevent CSFR attacks.
  if (!isset($_GET['token']) || !drupal_valid_token($_GET['token'], 'admin/structure/skinr/group/' . $group->gid . '/' . ($status ? 'enable' : 'disable'))) {
    return MENU_ACCESS_DENIED;
  }

  // Load an uncached version of the skin configuration object.
  $group = skinr_context_group_load_unchanged($group->gid);
  // Let's save some time in skinr_context_group_save() by setting
  // $group->original here.
  $group->original = clone($group);

  // Update the status and save the skin configuration.
  $group->status = $status ? 1 : 0;
  skinr_context_group_save($group);

  // Some modules may prevent enabling a group in some cases. So check for it.
  if ($group->status == $status) {
    if ($status) {
      drupal_set_message(t('Skin configuration group has been enabled.'));
    }
    else {
      drupal_set_message(t('Skin configuration group has been disabled.'));
    }
  }
  else {
    drupal_set_message(t('Changing skin configuration group status failed.'), 'warning');
  }

  // Return to the skin configuration group overview page.
  drupal_goto('admin/structure/skinr/list/groups');
}

/**
 * Form builder for the skinr settings revert confirmation form.
 *
 * @param $skin
 *   Tje skin configuration object to delete.
 *
 * @ingroup forms
 */
function skinr_context_ui_revert_confirm($form, &$form_state, $group) {
  $form['#group'] = $group;
  // Always provide skin configuration gid in the same form key as in the skin
  // configuration group edit form.
  $form['gid'] = array(
    '#type' => 'value',
    '#value' => $group->gid,
  );

  return confirm_form($form,
    t('Are you sure you want to permanently remove any customizations made to this skin configuration group?'),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/skinr/list/groups',
    t('This action cannot be undone.<br />Type: %type<br />Element: %element<br />Group: %group', array('%type' => $group->module, '%element' => $group->element, '%group' => $group->title)),
    t('Revert'),
    t('Cancel')
  );
}

/**
 * Form submission handler for skinr_context_ui_revert_confirm().
 */
function skinr_context_ui_revert_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    skinr_context_group_revert($form_state['values']['gid']);
    watchdog('skinr', 'Reverted a skin configuration group.');
    drupal_set_message(t('A skin configuration group has been reverted.'));
  }

  $form_state['redirect'] = 'admin/structure/skinr/list/groups';
}
