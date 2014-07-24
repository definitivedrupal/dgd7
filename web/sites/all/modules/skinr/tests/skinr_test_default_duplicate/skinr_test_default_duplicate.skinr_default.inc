<?php
/**
 * @file
 * skinr_test_default_duplicate.skinr.inc
 */

/**
 * Implements hook_skinr_skin_defaults().
 */
function skinr_test_default_duplicate_skinr_skin_defaults() {
  $skin = new stdClass();
  $skin->status = TRUE; /* Edit this to false to make a default skin disabled initially */
  $skin->api_version = 2;
  $skin->uuid = '501ff0c3-db03-0944-9910-3a788f38097a';
  $skin->theme = 'skinr_test_subtheme';
  $skin->module = 'block';
  $skin->element = 'system__user-menu';
  $skin->skin = 'skinr_test_subtheme';
  $skin->options = array(
    'option3' => 'option3',
  );
  $skins['501ff0c3-db03-0944-9910-3a788f38097a'] = $skin;

  return $skins;
}
