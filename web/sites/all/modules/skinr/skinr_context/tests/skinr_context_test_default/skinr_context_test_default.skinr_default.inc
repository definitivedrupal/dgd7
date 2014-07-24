<?php
/**
 * @file
 * skinr_context_test_default.skinr.inc
 */

/**
 * Implements hook_skinr_context_group_defaults().
 */
function skinr_context_test_default_skinr_context_group_defaults() {
  $group = new stdClass();
  $group->status = TRUE; /* Edit this to false to make a default group disabled initially */
  $group->api_version = 2;
  $group->gid = 'block:system__user-menu:default';
  $group->module = 'block';
  $group->element = 'system__user-menu';
  $group->title = 'Default';
  $group->description = 'A short desciption of this group.';
  $group->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $group->condition_mode = 0;
  $group->weight = 0;
  $groups['block:system__user-menu:default'] = $group;

  return $groups;
}

/**
 * Implements hook_skinr_skin_defaults().
 */
function skinr_context_test_default_skinr_skin_defaults() {
  $skin = new stdClass();
  $skin->status = TRUE; /* Edit this to false to make a default skin disabled initially */
  $skin->api_version = 2;
  $skin->uuid = '501ff0c3-db03-0944-9910-3a788f38097a';
  $skin->theme = 'skinr_test_subtheme';
  $skin->module = 'block';
  $skin->element = 'system__user-menu';
  $skin->skin = 'skinr_test_subtheme';
  $skin->options = array(
    'option1' => 'option1',
    'option2' => 'option2',
  );
  $skin->gid = 'block:system__user-menu:default';
  $skins['501ff0c3-db03-0944-9910-3a788f38097a'] = $skin;

  // Skin without gid. These exist for skin settings exported by sites without
  // skinr_context installed.
  $skin = new stdClass();
  $skin->status = TRUE; /* Edit this to false to make a default skin disabled initially */
  $skin->api_version = 2;
  $skin->uuid = '20cc8380-d1d5-11e3-9c1a-0800200c9a66';
  $skin->theme = 'skinr_test_subtheme';
  $skin->module = 'block';
  $skin->element = 'system__help';
  $skin->skin = 'skinr_test_subtheme';
  $skin->options = array(
    'option1' => 'option1',
    'option2' => 'option2',
  );
  $skins['20cc8380-d1d5-11e3-9c1a-0800200c9a66'] = $skin;

  return $skins;
}
