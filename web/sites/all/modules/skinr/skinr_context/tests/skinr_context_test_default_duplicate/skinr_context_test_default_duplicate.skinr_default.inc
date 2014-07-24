<?php
/**
 * @file
 * skinr_context_test_default_duplicate.skinr.inc
 */

/**
 * Implements hook_skinr_context_group_defaults().
 */
function skinr_context_test_default_duplicate_skinr_context_group_defaults() {
  $group = new stdClass();
  $group->status = TRUE; /* Edit this to false to make a default group disabled initially */
  $group->api_version = 2;
  $group->gid = 'block:system__user-menu:default';
  $group->module = 'block';
  $group->element = 'system__user-menu';
  $group->title = 'Default';
  $group->description = 'A short desciption of this group.';
  $group->conditions = array();
  $group->condition_mode = 0;
  $group->weight = 0;
  $groups['block:system__user-menu:default'] = $group;

  return $groups;
}
