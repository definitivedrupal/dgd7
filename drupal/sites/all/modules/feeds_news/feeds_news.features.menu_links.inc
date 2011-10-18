<?php
/**
 * @file
 * feeds_news.features.menu_links.inc
 */

/**
 * Implementation of hook_menu_default_menu_links().
 */
function feeds_news_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: main-menu:feeds
  $menu_links['main-menu:feeds'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'feeds',
    'router_path' => 'feeds',
    'link_title' => 'Feeds',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '0',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Feeds');


  return $menu_links;
}
