<?php
/**
 * @file
 * This file contains no working PHP code; it exists to provide additional documentation
 * for doxygen as well as to document hooks in the standard Drupal manner.
 */

/**
 * @defgroup skinr_context Skinr Context API Manual
 *
 * Topics:
 * - @ref skinr_context_hooks
 */

/**
 * @defgroup skinr_context_hooks Skinr Context's hooks
 * @{
 * Hooks that can be implemented by other modules in order to implement the
 * Skinr Context API.
 */

/**
 * Alter the default skin settings group objects.
 *
 * @param $default_groups
 *   An array of skin settings group objects from code.
 */
function hook_skinr_context_group_defaults_alter(&$default_groups) {
  // Rename a group.
  $default_groups['block:system__navigation:standard']['title'] = t('Fancy title');
}

/**
 * Alter the skin settings group object before it is imported from code.
 *
 * @param $group
 *   A skin settings group object.
 */
function hook_skinr_context_group_import_alter(&$group) {
  // Add in custom variable.
  $group->custom = 'Something special';
}

/**
 * Alter the skin settings group object before it is output as exported code.
 *
 * @param $group
 *   A skin settings group object.
 * @param $prefix
 *   A string to prefix the code with, used to indent the resulting code.
 */
function hook_skinr_context_group_export_alter(&$group, &$prefix) {
  // Remove custom variable.
  unset($skin->custom);
}

/**
 * @}
 */
