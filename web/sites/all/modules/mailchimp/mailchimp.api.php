<?php

/**
 * @file
 * Mailchimp hook definitions.
 */

/**
 * Respond to an email being added to a list.
 *
 * @param $list
 *   MailChimp list object.
 * @param $email
 * @param $merge_vars
 */
function hook_mailchimp_subscribe_user($list, $email, $merge_vars) {

}

/**
 * Respond to an email being removed from a list.
 *
 * @param $list
 *   MailChimp list object.
 * @param $email
 */
function hook_mailchimp_unsubscribe_user($list, $email) {

}

/**
 * Return an array of additional merge tokens.
 *
 * @return array
 */
function hook_mailchimp_lists_merge_tokens() {
  $out = array('' => t('-- Select --'));

  // invoke hook to get all merge tokens
  $tokens = module_invoke_all('mailchimp_lists_merge_tokens');

  foreach ($tokens as $key => $token) {
    $out[$key] = t('!field', array('!field' => $token['name']));
  }

  return $out;
}

/**
 * Return an array of matching merge values.
 *
 * @param $mergevars
 * @param $account
 * @param $list
 *
 * @return array
 */
function hook_mailchimp_lists_merge_values($mergevars, $account, $list) {
  $values = array();

  // grab the saved list merge vars and filter out unset values
  if (!empty($list->settings['mergefields'])) {
    $mergevars = array_filter($list->settings['mergefields']);
    $mergevars = array_flip($mergevars);

    // match with token values
    $values = module_invoke_all('mailchimp_lists_merge_values', $mergevars, $account, $list);

    // always add email
    $values += array(
      'EMAIL' => $account->mail
    );
  }

  return $values;
}
