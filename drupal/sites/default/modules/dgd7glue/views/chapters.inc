<?php
$view = new view;
$view->name = 'chapters';
$view->description = 'Various chapter listings.';
$view->tag = 'DGD7';
$view->base_table = 'node';
$view->human_name = '';
$view->core = 0;
$view->api_version = '3.0-alpha1';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['title'] = 'Chapter list with authors and internal names';
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'title' => 'title',
  'entity_id_1' => 'entity_id_1',
  'entity_id' => 'entity_id',
  'changed' => 'changed',
  'edit_node' => 'entity_id_1',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
  'entity_id_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => ' - ',
  ),
  'entity_id' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
  'changed' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
  ),
  'edit_node' => array(
    'align' => '',
    'separator' => ' | ',
  ),
);
$handler->display->display_options['style_options']['override'] = 1;
$handler->display->display_options['style_options']['sticky'] = 0;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['title']['alter']['trim'] = 0;
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['title']['alter']['html'] = 0;
$handler->display->display_options['fields']['title']['hide_empty'] = 0;
$handler->display->display_options['fields']['title']['empty_zero'] = 0;
$handler->display->display_options['fields']['title']['link_to_node'] = 1;
/* Field: Fields: Internal name */
$handler->display->display_options['fields']['entity_id_1']['id'] = 'entity_id_1';
$handler->display->display_options['fields']['entity_id_1']['table'] = 'field_data_field_internal';
$handler->display->display_options['fields']['entity_id_1']['field'] = 'entity_id';
$handler->display->display_options['fields']['entity_id_1']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['entity_id_1']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['entity_id_1']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['entity_id_1']['alter']['trim'] = 0;
$handler->display->display_options['fields']['entity_id_1']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['entity_id_1']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['entity_id_1']['alter']['strip_tags'] = 1;
$handler->display->display_options['fields']['entity_id_1']['alter']['html'] = 0;
$handler->display->display_options['fields']['entity_id_1']['element_type'] = '0';
$handler->display->display_options['fields']['entity_id_1']['element_label_type'] = '0';
$handler->display->display_options['fields']['entity_id_1']['element_label_colon'] = 0;
$handler->display->display_options['fields']['entity_id_1']['element_wrapper_type'] = '0';
$handler->display->display_options['fields']['entity_id_1']['element_default_classes'] = 0;
$handler->display->display_options['fields']['entity_id_1']['hide_empty'] = 0;
$handler->display->display_options['fields']['entity_id_1']['empty_zero'] = 0;
$handler->display->display_options['fields']['entity_id_1']['type'] = 'text_plain';
/* Field: Fields: Author */
$handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
$handler->display->display_options['fields']['entity_id']['table'] = 'field_data_field_author';
$handler->display->display_options['fields']['entity_id']['field'] = 'entity_id';
$handler->display->display_options['fields']['entity_id']['label'] = 'Author(s)';
$handler->display->display_options['fields']['entity_id']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['entity_id']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['entity_id']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['entity_id']['alter']['trim'] = 0;
$handler->display->display_options['fields']['entity_id']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['entity_id']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['entity_id']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['entity_id']['alter']['html'] = 0;
$handler->display->display_options['fields']['entity_id']['element_label_colon'] = 1;
$handler->display->display_options['fields']['entity_id']['element_default_classes'] = 1;
$handler->display->display_options['fields']['entity_id']['hide_empty'] = 0;
$handler->display->display_options['fields']['entity_id']['empty_zero'] = 0;
$handler->display->display_options['fields']['entity_id']['click_sort_column'] = 'nid';
/* Field: Content: Updated date */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'node';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['changed']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['changed']['alter']['trim'] = 0;
$handler->display->display_options['fields']['changed']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['changed']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['changed']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['changed']['alter']['html'] = 0;
$handler->display->display_options['fields']['changed']['hide_empty'] = 0;
$handler->display->display_options['fields']['changed']['empty_zero'] = 0;
/* Field: Content: Edit link */
$handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['table'] = 'node';
$handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['alter']['alter_text'] = 0;
$handler->display->display_options['fields']['edit_node']['alter']['make_link'] = 0;
$handler->display->display_options['fields']['edit_node']['alter']['absolute'] = 0;
$handler->display->display_options['fields']['edit_node']['alter']['trim'] = 0;
$handler->display->display_options['fields']['edit_node']['alter']['word_boundary'] = 1;
$handler->display->display_options['fields']['edit_node']['alter']['ellipsis'] = 1;
$handler->display->display_options['fields']['edit_node']['alter']['strip_tags'] = 0;
$handler->display->display_options['fields']['edit_node']['alter']['html'] = 0;
$handler->display->display_options['fields']['edit_node']['element_type'] = '0';
$handler->display->display_options['fields']['edit_node']['element_label_type'] = '0';
$handler->display->display_options['fields']['edit_node']['element_label_colon'] = 0;
$handler->display->display_options['fields']['edit_node']['element_wrapper_type'] = '0';
$handler->display->display_options['fields']['edit_node']['element_default_classes'] = 0;
$handler->display->display_options['fields']['edit_node']['hide_empty'] = 1;
$handler->display->display_options['fields']['edit_node']['empty_zero'] = 0;
/* Sort criterion: Book: Hierarchy */
$handler->display->display_options['sorts']['p']['id'] = 'p';
$handler->display->display_options['sorts']['p']['table'] = 'book_menu_links';
$handler->display->display_options['sorts']['p']['field'] = 'p';
$handler->display->display_options['sorts']['p']['expose']['label'] = 'Book: Hierarchy';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status_1']['id'] = 'status_1';
$handler->display->display_options['filters']['status_1']['table'] = 'node';
$handler->display->display_options['filters']['status_1']['field'] = 'status';
$handler->display->display_options['filters']['status_1']['value'] = '1';
$handler->display->display_options['filters']['status_1']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type_1']['id'] = 'type_1';
$handler->display->display_options['filters']['type_1']['table'] = 'node';
$handler->display->display_options['filters']['type_1']['field'] = 'type';
$handler->display->display_options['filters']['type_1']['value'] = array(
  'book' => 'book',
);
$handler->display->display_options['filters']['type_1']['expose']['operator'] = FALSE;
/* Filter criterion: Book: Depth */
$handler->display->display_options['filters']['depth']['id'] = 'depth';
$handler->display->display_options['filters']['depth']['table'] = 'book_menu_links';
$handler->display->display_options['filters']['depth']['field'] = 'depth';
$handler->display->display_options['filters']['depth']['operator'] = 'between';
$handler->display->display_options['filters']['depth']['value']['min'] = '1';
$handler->display->display_options['filters']['depth']['value']['max'] = '3';
$handler->display->display_options['filters']['depth']['value']['value'] = '1';
$handler->display->display_options['filters']['depth']['expose']['operator'] = FALSE;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Book: Hierarchy */
$handler->display->display_options['sorts']['p']['id'] = 'p';
$handler->display->display_options['sorts']['p']['table'] = 'book_menu_links';
$handler->display->display_options['sorts']['p']['field'] = 'p';
$handler->display->display_options['path'] = 'authors-chapter-list';

/* Display: Page: Teasers */
$handler = $view->new_display('page', 'Page: Teasers', 'teasers');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Chapters';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'node';
$handler->display->display_options['row_options']['links'] = 1;
$handler->display->display_options['row_options']['comments'] = 0;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
$handler->display->display_options['path'] = 'chapter-outlines';

/* Display: Page: Compact list */
$handler = $view->new_display('page', 'Page: Compact list', 'compact');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Chapters and Authors of the Definitive Guide to Drupal 7';
$handler->display->display_options['display_description'] = 'Uses the Compact view mode.';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'node';
$handler->display->display_options['row_options']['view_mode'] = 'compact';
$handler->display->display_options['row_options']['links'] = 0;
$handler->display->display_options['row_options']['comments'] = 0;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
$handler->display->display_options['path'] = 'chapters';
$translatables['chapters'] = array(
  t('Defaults'),
  t('Chapter list with authors and internal names'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Title'),
  t('Internal name'),
  t('Author(s)'),
  t('Updated date'),
  t('Edit link'),
  t('Book: Hierarchy'),
  t('Page'),
  t('Page: Teasers'),
  t('Chapters'),
  t('Page: Compact list'),
  t('Chapters and Authors of the Definitive Guide to Drupal 7'),
  t('Uses the Compact view mode.'),
);

$views[$view->name] = $view;
