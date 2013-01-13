<?php
/**
 * @file
 * Implements Skinr hooks for block.module.
 */

/**
 * Implements hook_skinr_config_info().
 */
function block_skinr_config_info() {
  return array('block');
}

/**
 * Implements hook_skinr_ui_element_options().
 */
function block_skinr_ui_element_options($theme_name = NULL) {
  $options = array('block' => array());

  $themes = list_themes();
  $used_themes = array();
  if ($theme_name) {
    // Only process the given theme.
    $used_themes[] = $theme_name;
  }
  else {
    // Process all enabled themes.
    $options = array();
    foreach ($themes as $theme) {
      if (!$theme->status) {
        continue;
      }
      $used_themes[] = $theme->name;
    }
  }

  // Load all enabled blocks.
  foreach ($used_themes as $theme) {
    $blocks = _block_skinr_load_blocks($theme);

    $theme_title = isset($themes[$theme]->info['name']) ? $themes[$theme]->info['name'] : $theme;
    $theme_key = t('@theme_name blocks', array('@theme_name' => $theme_title));

    foreach ($blocks as $name => $block) {
      $options['block'][$theme_key][$name] = $block->info;
    }
  }

  return $options;
}

/**
 * Implements hook_skinr_ui_element_title().
 */
function block_skinr_ui_element_title($module, $element, $theme_name) {
  if ($module == 'block') {
    //list($module, $delta) = explode('__', $element, 2);
    $blocks = _block_skinr_load_blocks($theme_name);
    if (!empty($blocks[$element]->info)) {
      return $blocks[$element]->info;
    }
  }
}

/**
 * Returns a list of enabled blocks for a given theme.
 *
 * Based on _block_rehash(), but without the blocks table rebuild part.
 *
 * @return
 *   An array of blocks.
 *
 * @see _block_rehash()
 */
function _block_skinr_load_blocks($theme) {
  $cache = &drupal_static(__FUNCTION__, array());

  if (!isset($cache['code_blocks'])) {
    $cache['code_blocks'] = array();
    // Gather the blocks defined by modules.
    foreach (module_implements('block_info') as $module) {
      $module_blocks = module_invoke($module, 'block_info');
      foreach ($module_blocks as $delta => $block) {
        // Add identifiers.
        $block['module'] = $module;
        $block['delta']  = $delta;
        $block['theme']  = $theme;
        $cache['code_blocks'][$module][$delta] = $block;
      }
    }
  }

  if (!isset($cache['blocks'][$theme])) {
    $regions = system_region_list($theme);

    $current_blocks = $cache['code_blocks'];
    // These are {block}.bid values to be kept.
    $bids = array();
    $or = db_or();

    foreach ($cache['code_blocks'] as $module => $module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        // Compile a condition to retrieve this block from the database.
        $condition = db_and()
          ->condition('module', $module)
          ->condition('delta', $delta);
        $or->condition($condition);
      }
    }

    $database_blocks = db_select('block', 'b')
      ->fields('b')
      ->condition($or)
      ->condition('theme', $theme)
      ->execute();
    foreach ($database_blocks as $block) {
      // Preserve info which is not in the database.
      $block->info = $current_blocks[$block->module][$block->delta]['info'];
      // The cache mode can only by set from hook_block_info(), so that has
      // precedence over the database's value.
      if (isset($current_blocks[$block->module][$block->delta]['cache'])) {
        $block->cache = $current_blocks[$block->module][$block->delta]['cache'];
      }
      // Blocks stored in the database override the blocks defined in code.
      $current_blocks[$block->module][$block->delta] = get_object_vars($block);
      // Preserve this block.
      $bids[$block->bid] = $block->bid;
    }
    drupal_alter('block_info', $current_blocks, $theme, $cache['code_blocks']);

    foreach ($current_blocks as $module => $module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        if (!empty($block['region']) && $block['region'] != BLOCK_REGION_NONE && !isset($regions[$block['region']])) {
          // Disabled modules are moved into the BLOCK_REGION_NONE later so no
          // need to move the block to another region.
          $current_blocks[$module][$delta]['status'] = 0;
        }
        // Set region to none if not enabled and make sure status is set.
        if (empty($block['status'])) {
          $current_blocks[$module][$delta]['status'] = 0;
          $current_blocks[$module][$delta]['region'] = BLOCK_REGION_NONE;
        }
      }
    }

    $cache['blocks'][$theme] = array();
    foreach ($current_blocks as $module => $module_blocks) {
      foreach ($module_blocks as $delta => $block) {
        if (!$block['status']) {
          continue;
        }
        $cache['blocks'][$theme][$block['module'] . '__' . $block['delta']] = (object) $block;
      }
    }
    asort($cache['blocks'][$theme]);
  }

  return $cache['blocks'][$theme];
}

/**
 * Implements hook_skinr_theme_hooks().
 */
function block_skinr_theme_hooks($module, $element) {
  $theme_hooks = array();

  if ($module == 'block') {
    list($module, $delta) = explode('__', $element, 2);
    $result = db_select('block', 'b')
      ->fields('b')
      ->distinct()
      ->condition('b.module', $module)
      ->condition('b.delta', $delta)
      ->range(0, 1)
      ->execute();
    foreach ($result as $block) {
      $theme_hooks[] = 'block__'. $block->module;
    }
    $theme_hooks[] = 'block';
  }

  return $theme_hooks;
}

/**
 * Implements hook_skinr_elements().
 */
function block_skinr_elements($variables, $hook) {
  $elements = array();
  if ($hook == 'block') {
    $elements['block'] = array($variables['block']->module . '__' . $variables['block']->delta);
  }
  return $elements;
}
