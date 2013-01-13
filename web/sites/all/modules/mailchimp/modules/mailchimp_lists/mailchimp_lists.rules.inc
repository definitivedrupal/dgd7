<?php
/**
 * @file Rules integration for the mailchimp module.
 */

/**
 * Implements hook_rules_action_info().
 */
function mailchimp_lists_rules_condition_info() {

  $items['mailchimp_lists_mail_is_subscribed'] = array(
    'label' => t('E-Mail is subscribed to a mailchimp list'),
    'parameter' => array(
      'mail' => array(
        'type' => 'mail',
        'label' => t('E-Mail address'),
        'description' => t('The e-mail address for which to check whether it is subscribed to a list.'),
        'default mode' => 'selector',
      ),
      'list' => array(
        'type' => 'mailchimp_list',
        'label' => t('Mailchimp list'),
        'default mode' => 'input',
      ),
    ),
    'group' => t('Mailchimp'),
    'access callback' => 'mailchimp_lists_rules_access_callback',
    'base' => 'mailchimp_lists_rules_condition_mail_is_subscribed',
  );
  return $items;
}

/**
 * Condition callback: User is subscribed to a list.
 */
function mailchimp_lists_rules_condition_mail_is_subscribed($mail, $list) {
  return mailchimp_is_subscribed($list->mc_list_id, $mail);
}

/**
 * Implements hook_rules_action_info().
 */
function mailchimp_lists_rules_action_info() {

  $items['mailchimp_lists_user_subscribe_list'] = array(
    'label' => t('Subscribe user to a mailchimp list'),
    'parameter' => array(
      'account' => array(
        'type' => 'user',
        'label' => t('User'),
        'description' => t('The user to subscribe to a list.'),
      ),
      'list' => array(
        'type' => 'mailchimp_list',
        'label' => t('Mailchimp list'),
        'default mode' => 'input',
      ),
    ),
    'group' => t('Mailchimp'),
    'access callback' => 'mailchimp_lists_rules_access_callback',
    'base' => 'mailchimp_lists_rules_action_user_subscribe_list',
  );
  $items['mailchimp_lists_unsubscribe_list'] = array(
    'label' => t('Unsubscribe mail address from a mailchimp list'),
    'parameter' => array(
      'mail' => array(
        'type' => 'text',
        'label' => t('E-Mail address'),
        'description' => t('The mail address to unsubscribe from the given list.'),
        'default mode' => 'selector',
      ),
      'list' => array(
        'type' => 'mailchimp_list',
        'label' => t('Mailchimp list'),
        'default mode' => 'input',
      ),
    ),
    'group' => t('Mailchimp'),
    'access callback' => 'mailchimp_lists_rules_access_callback',
    'base' => 'mailchimp_lists_rules_action_unsubscribe_list',
  );
  return $items;
}

/**
 * Action callback: Subscribe a user to a list.
 */
function mailchimp_lists_rules_action_user_subscribe_list($account, $list) {
  $merge_variables = mailchimp_lists_load_user_mergevars($account, $list);
  mailchimp_subscribe_user($list, $account->mail, $merge_variables, FALSE);
}

/**
 * Action callback: Unsubscribe a user to a list.
 */
function mailchimp_lists_rules_action_unsubscribe_list($mail, $list) {
  mailchimp_unsubscribe_user($list, $mail, FALSE);
}

/**
 * Access callback for the rules integration.
 */
function mailchimp_lists_rules_access_callback() {
  return user_access('administer mailchimp');
}
