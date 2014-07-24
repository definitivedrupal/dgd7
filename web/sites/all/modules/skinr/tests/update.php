<?php

/**
 * @file
 * Simulates cache clearing while running update script.
 */

/**
 * Global flag indicating that update.php is being run.
 *
 * When this flag is set, various operations do not take place, such as invoking
 * hook_init() and hook_exit(), css/js preprocessing, and translation.
 */
define('MAINTENANCE_MODE', 'update');

/**
 * Defines the root directory of the Drupal installation.
 */
define('DRUPAL_ROOT', $_GET['drupal_root']);

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

skinr_cache_reset();
skinr_implements_api();
