<?php
// $Id: simplenews.api.php,v 1.2 2010/03/02 12:21:28 sutharsan Exp $

/**
 * @file
 * Hooks provided by the Simplenews module.
 */

/**
 * TODO
 */
function hook_simplenews_issue_operations() {
  
}

/**
 * TODO
 */
function hook_simplenews_subscription_operations() {
  
}

/**
 * Collect recipients for a simplenews newsletter.
 *
 * Get both subscribed and unusbscribed recipients per newsletter category.
 * Special care should be taken to handle unsubscribed users correctly.
 *
 * @param $recipients
 *   Array of recipient objects with the following as minimum:
 *   $recipients['mail@example.com'] 
 *     recipient->mail      Email address
 *     recipient->status    Status flag (1, 0)
 *                          1 = may receive email, is subscribed;
 *                          0 = should not receive email, is unsubscribed.
 *     recipient->language  Language code of preferred email language
 * @param $scid
 *   Newsletter category ID.
 */
// TODO Is 'uid' also required?
//      recipient->uid       User ID of recipient (0 for anonymous)
function hook_simplenews_recipients_alter(&$recipients, $scid) {
  $list = simplenews_get_mailing_list_by_category($scid);
  $recipients = simplenews_array_merge($recipients, simplenews_get_subscriptions_by_list($list->slid), 'simplenews_check_status');
}


/**
 * TODO
 */
function hook_simplenews_category_insert($category) {
  
}

/**
 * TODO
 */
function hook_simplenews_category_update($category) {
  
}

/**
 * TODO
 */
function hook_simplenews_category_delete($category) {
  
}

/**
 * TODO
 */
function hook_simplenews_mailing_list_update($list) {
  
}

/**
 * TODO
 */
function hook_simplenews_mailing_list_insert($list) {
  
}

/**
 * TODO
 */
function hook_simplenews_mailing_list_delete($list) {
  
}

/**
 * TODO
 */
function hook_simplenews_subscriber_update($subscriber) {
  
}

/**
 * TODO
 */
function hook_simplenews_subscriber_insert($subscriber) {
  
}

/**
 * TODO
 */
function hook_simplenews_subscriber_delete($category) {
  
}