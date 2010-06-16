<?php
// $Id: simplenews.views.inc,v 1.1 2010/03/29 21:13:44 sutharsan Exp $

/**
 * @file
 * Views interface for simplenews.
 */

/**
 * Implements hook_views_data().
 */
function simplenews_views_data() {
  // Basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['simplenews_newsletter']['table']['group']  = t('Simplenews newsletter');

  $data['simplenews_newsletter']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Node'),
    'help' => t('Using nodes as newsletter.'),
  );

  // joins
  $data['simplenews_newsletter']['table']['join'] = array(
    //...to the node table
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  // Fields

  // newsletter sent status
  $data['simplenews_newsletter']['status'] = array(
    'title' => t('Sent status'),
    // The item it appears as on the UI,
    'help' => t('Newsletter sent status: Not sent, Pending (being sent or waiting for cron to run), Sent.'),
    // The help that appears on the UI,
    'field' => array(
      'handler' => 'simplenews_handler_field_newsletter_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'simplenews_handler_filter_newsletter_status',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // ----------------------------------------------------------------------
  // Newsletter category table

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['simplenews_category']['table']['group']  = t('Simplenews category');

  $data['simplenews_category']['table']['base'] = array(
    'field' => 'scid',
    'title' => t('Newsletter category'),
    'help' => t('Newsletter categories are collections of Simplenews newsletters.'),
  );

  // joins
  $data['simplenews_category']['table']['join'] = array(
    // Category links directly to simplenews_newsletter via scid.
    'simplenews_newsletter' => array(
      'left_field' => 'scid',
      'field' => 'scid',
    ),
    // Category links to node via simplenews_newsletter
    'node' => array(
      'left_table' => 'simplenews_newsletter',
      'left_field' => 'scid',
      'field' => 'scid',
    ),
    // Category links to node_revision via simplenews_newsletter
    'node_revision' => array(
      'left_table' => 'simplenews_newsletter',
      'left_field' => 'scid',
      'field' => 'scid',
    ),
  );

  // Fields

  // Simplenews category name
  $data['simplenews_category']['scid'] = array(
    'title' => t('Category ID'),
    'help' => t('Newsletter category ID: Unique newsletter category ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
  );
  // Simplenews category name
  $data['simplenews_category']['name'] = array(
    'title' => t('Name'),
    'help' => t('Newsletter category name: Name of the newsletter category.'),
    'field' => array(
      'handler' => 'simplenews_handler_field_category_name',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_handlers().
 */
function simplenews_views_handlers() {
  return array(
    // field handlers
    'simplenews_handler_field_newsletter_status',
    'simplenews_handler_field_category_name',

    // filter handlers
    'simplenews_handler_filter_newsletter_status',
  );
}
