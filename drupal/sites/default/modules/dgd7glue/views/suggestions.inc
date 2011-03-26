<?php
$view = new view;
$view->name = 'suggestions';
$view->description = 'Suggestions';
$view->tag = 'DGD7';
$view->base_table = 'node';
$view->human_name = '';
$view->core = 0;
$view->api_version = '3.0-alpha1';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '20';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['expose']['items_per_page_options_all'] = 0;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'node';
$handler->display->display_options['row_options']['links'] = 1;
$handler->display->display_options['row_options']['comments'] = 0;
/* Sort criterion: Content: Nid */
$handler->display->display_options['sorts']['nid']['id'] = 'nid';
$handler->display->display_options['sorts']['nid']['table'] = 'node';
$handler->display->display_options['sorts']['nid']['field'] = 'nid';
$handler->display->display_options['sorts']['nid']['order'] = 'DESC';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'suggestion' => 'suggestion',
);
$handler->display->display_options['filters']['type']['expose']['operator'] = FALSE;

/* Display: Not in yet page */
$handler = $view->new_display('page', 'Not in yet page', 'page_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Suggestions to Use';
$handler->display->display_options['display_description'] = 'Suggestions with a status of slated to go in or if there\'s room.';
$handler->display->display_options['defaults']['filters'] = FALSE;
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'suggestion' => 'suggestion',
);
$handler->display->display_options['filters']['type']['expose']['operator'] = FALSE;
/* Filter criterion: Taxonomy: Term */
$handler->display->display_options['filters']['tid']['id'] = 'tid';
$handler->display->display_options['filters']['tid']['table'] = 'taxonomy_index';
$handler->display->display_options['filters']['tid']['field'] = 'tid';
$handler->display->display_options['filters']['tid']['value'] = array(
  19 => '19',
  20 => '20',
);
$handler->display->display_options['filters']['tid']['reduce_duplicates'] = 0;
$handler->display->display_options['filters']['tid']['type'] = 'select';
$handler->display->display_options['filters']['tid']['vocabulary'] = 'status';
$handler->display->display_options['path'] = 'suggestions/not-used-yet';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Not Used Yet';
$handler->display->display_options['menu']['description'] = 'Suggestions of type "Slated to go in" or "If there\'s room"';
$handler->display->display_options['menu']['weight'] = '0';

/* Display: All Suggestions */
$handler = $view->new_display('page', 'All Suggestions', 'all');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'All Suggestions';
$handler->display->display_options['defaults']['filters'] = FALSE;
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'suggestion' => 'suggestion',
);
$handler->display->display_options['filters']['type']['expose']['operator'] = FALSE;
$handler->display->display_options['path'] = 'suggestions/all';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'All Suggestions';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['tab_options']['type'] = 'normal';
$handler->display->display_options['tab_options']['title'] = 'Suggestions';
$handler->display->display_options['tab_options']['weight'] = '0';
$handler->display->display_options['tab_options']['name'] = 'main-menu';
$translatables['suggestions'] = array(
  t('Defaults'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('Not in yet page'),
  t('Suggestions to Use'),
  t('Suggestions with a status of slated to go in or if there\'s room.'),
  t('All Suggestions'),
);

$views[$view->name] = $view;
