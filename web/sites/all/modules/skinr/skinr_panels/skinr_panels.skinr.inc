<?php
/**
 * @file
 * Implements Skinr hooks for panels.module.
 */

/**
 * Implements hook_skinr_config_info().
 */
function skinr_panels_skinr_config_info() {
  return array('panels');
}

/**
 * Implements hook_skinr_ui_element_options().
 */
function skinr_panels_skinr_ui_element_options($theme_name = NULL) {
  $options = array('panels' => array());

  // Load displays for pages.
  if (module_exists('page_manager')) {
    module_load_include('inc', 'page_manager', 'page_manager.admin');
    ctools_include('plugins', 'panels');

    $tasks = page_manager_get_tasks_by_type('page');
    $pages = array('operations' => array());

    page_manager_get_pages($tasks, $pages);

    foreach ($pages['rows'] as $task_name => $page) {
      // Skip disabled pages.
      if ($pages['disabled'][$task_name]) {
        continue;
      }

      list($task_id, $subtask_id) = page_manager_get_task_id($task_name);

      $task = $tasks[$task_id];
      if (empty($task)) {
        continue;
      }

      if ($subtask_id) {
        $subtask = page_manager_get_task_subtask($task, $subtask_id);
        if (empty($subtask)) {
          continue;
        }
      }
      else {
        $subtask = $task;
        $subtask['name'] = '';
      }

      $handlers = page_manager_load_sorted_handlers($task, $subtask_id);
      $dids = array();
      foreach ($handlers as $handler) {
        if ($handler->handler != 'panel_context') {
          continue;
        }
        if (!isset($handler->conf['did'])) {
          // In code.
          $display = $handler->conf['display'];
        }
        else {
          $display = panels_load_display($handler->conf['did']);
        }

        // Output display.
        /*
        // No use in outputting display if we can't add classes to it. It's
        // only possible with a custom template override.
        $name = 'display__' . $display->uuid;
        $title = t('Display');
        $options['panels']['&#160;&#160;&#160;' . $display->title][$name] = $title;
        */

        // Output regions.
        /*
        // No use in outputting regions if we can't add classes to them.
        $layout = panels_get_layout($display->layout);
        foreach ($layout['regions'] as $region_id => $title) {
          $name = 'region__' . $display->uuid . '__' . $region_id;
          $title = t('Region: @region', array('@region' => $title));
          $options['panels']['&#160;&#160;&#160;' . $display->title][$name] = $title;
        }
        */

        // Output panes.
        foreach ($display->content as $pane) {
          if (!isset($pane->uuid)) {
            // Old version of panels; skip.
            $message = 'Pane @pane_id in display @display is missing a UUID property.';
            drupal_set_message(t($message, array('@pane_id' => $pane->pid, '@display' => $display->title)), 'warning');
            watchdog('skinr_panels', $message, array('@pane_id' => $pane->pid, '@display' => $display->title), WATCHDOG_WARNING);
            continue;
          }
          $name = 'pane__' . $pane->uuid;
          $title = t('Pane: @pane_id', array('@pane_id' => $pane->uuid));
          $options['panels']['&#160;&#160;&#160;' . $display->title][$name] = $title;
        }
      }
    }
  }
  ksort($options['panels']);

  // Load displays for panel nodes.
  if (module_exists('panels_node')) {
    $query = new EntityFieldQuery;
    $nids = $query
      ->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'panel')
      ->propertyCondition('status', '1')
      ->propertyOrderBy('title', 'ASC')
      ->execute();
    if ($nids) {
      foreach (node_load_multiple(array_keys($nids['node'])) as $node) {
        $display = panels_load_display($node->panels_node['did']);
        $category = t('Panel node: @title', array('@title' => $node->title));

        // Output display.
        /*
        // No use in outputting display if we can't add classes to it. It's
        // only possible with a custom template override.
        $name = 'display__' . $display->uuid;
        $title = t('Display');
        $options['panels']['&#160;&#160;&#160;' . $category][$name] = $title;
        */

        // Output regions.
        /*
        // No use in outputting regions if we can't add classes to them.
        $layout = panels_get_layout($display->layout);
        foreach ($layout['regions'] as $region_id => $title) {
          $name = 'region__' . $display->uuid . '__' . $region_id;
          $title = t('Region: @region', array('@region' => $title));
          $options['panels']['&#160;&#160;&#160;' . $category][$name] = $title;
        }
        */

        // Output panes.
        foreach ($display->content as $pane) {
          $name = 'pane__' . $pane->uuid;
          $title = t('Pane: @pane_id', array('@pane_id' => $pane->uuid));
          $options['panels']['&#160;&#160;&#160;' . $category][$name] = $title;
        }
      }
    }
  }

  // Load displays for mini panels.
  if (module_exists('panels_mini')) {

  }

  return $options;
}

/**
 * Implements hook_skinr_ui_element_title().
 */
function skinr_panels_skinr_ui_element_title($module, $element, $theme_name) {
  if ($module == 'panels') {
    list($hook, $uuid) = explode('__', $element);
    $did = _skinr_panels_display_from_uuid($uuid);
    if ($display = panels_load_display($did)) {
      $title = $display->title;
      if (!$title && module_exists('panels_node')) {
        $nid = db_query("SELECT nid FROM {panels_node} WHERE did = :did", array(
          ':did' => $did,
        ))->fetchField();
        if ($node = node_load($nid)) {
          $title = $node->title;
        }
      }
      return t('Pane %pane on %panel', array('%panel' => $title ? $title : t('display !did', array('!did' => $did)), '%pane' => $uuid));
    }
  }
}

/**
 * Helper function to retrieve a pane's display ID.
 *
 * @param $uuid
 *
 * @return
 *   The pane's display ID.
 */
function _skinr_panels_display_from_uuid($uuid) {
  return db_query("SELECT did FROM {panels_pane} WHERE uuid = :uuid", array(
    ':uuid' => $uuid,
  ))->fetchField();
}

/**
 * Implements hook_skinr_theme_hooks().
 */
function skinr_panels_skinr_theme_hooks($module, $element) {
  $theme_hooks = array();

  if ($module == 'panels') {
    if (strpos($element, 'region') === 0) {
      $theme_hooks[] = 'panels_region';
    }
    elseif (strpos($element, 'pane') === 0) {
      $theme_hooks[] = 'panels_pane';
    }
    else {
      $theme_hooks[] = 'panels_display';
    }
  }

  return $theme_hooks;
}

/**
 * Implements hook_skinr_elements().
 */
function skinr_panels_skinr_elements($variables, $hook) {
  $elements = array();
  if ($hook == 'panels_pane') {
    $elements['panels'] = array();
    if (!empty($variables['pane']->uuid)) {
      $elements['panels'][] = 'pane__' . $variables['pane']->uuid;
    }
    else {
      watchdog('skinr', 'Skinr can\'t apply your skin settings to this panel due to missing UUIDs. Re-export your panels to have their UUIDs added.', array(), WATCHDOG_WARNING);
    }
  }
  return $elements;
}
