<?php

/**
 * Implements hook_skinr_api_VERSION().
 */
function skinr_ui_test_skinable_skinr_api_2() {
}

/**
 * Implements hook_skinr_skin_info().
 */
function skinr_ui_test_skinable_skinr_skin_info() {
  $skins['skinr_ui_test_skinable'] = array(
    'title' => t('Color'),
    'type' => 'checkboxes',
    'group' => 'general',
    'theme hooks' => array('block', 'comment_wrapper__page', 'node__page'),
    'default status' => 1,
    'options' => array(
      'color_white' => array(
        'title' => 'White',
        'class' => array('color-white'),
      ),
    ),
  );
  return $skins;
}
