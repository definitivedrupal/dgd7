<?php
// $Id: simplenews.subscription.inc,v 1.21 2010/02/25 23:19:09 sutharsan Exp $

/**
 * @file
 * (Un)subscription and (un)subscription confirmation
 *
 * @ingroup simplenews
 */

/**
 * Menu callback: Generates the subscription form for users.
 *
 * @see simplenews_subscription_manager_form_validate()
 * @see simplenews_subscription_manager_form_submit()
 */
function simplenews_subscription_manager_form($form, &$form_state, $snid = NULL) {
  global $user;

  if (isset($snid)) {
    $account = new stdClass();
    $account->snid = $snid;
    $subscription = simplenews_get_subscription($account);
  }
  else {
    $subscription = simplenews_get_subscription($user);
  }

  // If non-admin is trying to edit someone else's subscription, access denied.
  if ($user->uid && $user->uid != $subscription->uid && !user_access('administer simplenews subscriptions')) {
    drupal_access_denied();
    return;
  }
  $form = _simplenews_subscription_manager_form($subscription);
  $form['#validate'][] = 'simplenews_subscription_manager_form_validate';
  $form['#submit'][] = 'simplenews_subscription_manager_form_submit';
  $form['#redirect'] = ''; //Return to home page after (un)subscribe

  return $form;
}

function simplenews_subscription_manager_form_validate($form, &$form_state) {
  $valid_email = valid_email_address($form_state['values']['mail']);
  if (!$valid_email) {
    form_set_error('mail', t('The email address you supplied is not valid.'));
  }
  $checked_newsletters = array_filter($form_state['values']['newsletters']);
  $account = new stdClass();
  $account->mail = $form_state['values']['mail'];
  if (!count($checked_newsletters) && !simplenews_get_subscription($account)) {
    form_set_error('newsletters', t('You must select at least one newsletter.'));
  }
}

function simplenews_subscription_manager_form_submit($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case t('Update'):
      // We first subscribe, then unsubscribe. This prevents deletion of subscriptions
      // when unsubscribed from the
      arsort($form_state['values']['newsletters'], SORT_NUMERIC);
      foreach ($form_state['values']['newsletters'] as $slid => $checked) {
        if ($checked) {
          simplenews_subscribe_user($form_state['values']['mail'], $slid, FALSE, 'website');
        }
        else {
          simplenews_unsubscribe_user($form_state['values']['mail'], $slid, FALSE, 'website');
        }
      }
      drupal_set_message(t('The newsletter subscriptions for %mail have been updated.', array('%mail' => $form_state['values']['mail'])));
      break;
    case t('Subscribe'):
      foreach ($form_state['values']['newsletters'] as $slid => $checked) {
        if ($checked) {
          simplenews_subscribe_user($form_state['values']['mail'], $slid, TRUE, 'website');
        }
      }
      drupal_set_message(t('You will receive a confirmation email shortly containing further instructions on how to complete your subscription.'));
      break;
    case t('Unsubscribe'):
      foreach ($form_state['values']['newsletters'] as $slid => $checked) {
        if ($checked) {
          simplenews_unsubscribe_user($form_state['values']['mail'], $slid, TRUE, 'website');
        }
      }
      drupal_set_message(t('You will receive a confirmation email shortly containing further instructions on how to complete the unsubscription process.'));
      break;
  }
}

/**
 * Menu callback: confirm the user's (un)subscription request
 *
 * This function is called by clicking the confirm link in the confirmation
 * email or the unsubscribe link in the footer of the newsletter. It handles
 * both subscription addition and subscription removal.
 *
 * @see simplenews_confirm_add_form()
 * @see simplenews_confirm_removal_form()
 *
 * TODO Add parameter description here.
 */
function simplenews_confirm_subscription() {
  $arguments = func_get_args();
  $op = array_shift($arguments);
  $code = array_shift($arguments);
  
  $md5 = drupal_substr($code, 0, 10);
  list($snid, $slid) = explode('t', drupal_substr($code, 10));

  $subscriber = simplenews_subscriber_load($snid);
  if (!$subscriber) {
    drupal_not_found();
    return;
  }

  if ($md5 == drupal_substr(md5($subscriber->mail . simplenews_private_key()), 0, 10)) {
    $list = simplenews_mailing_list_load($slid);
    // The confirmation page called with two arguments will display a confirmation question.
    // When called with three of more arguments the user will be directed to the
    // (un)subscribe confirmation page. The additional arguments will be passed on
    // to the confirmation page.
    if (empty($arguments)) {
      if ($op1 == 'remove') {
        return drupal_get_form('simplenews_confirm_removal_form', $subs->mail, $list);
      }
      elseif ($op1 == 'add') {
        return drupal_get_form('simplenews_confirm_add_form', $subs->mail, $list);
      }
    }
    else {
      if ($op1 == 'remove') {
        simplenews_unsubscribe_user($subs->mail, $tid, FALSE, 'website');
        if ($path = variable_get('simplenews_confirm_unsubscribe_page', '')) {
          $path = $path .'/'. implode('/', $arguments);
          drupal_goto($path);
        }
        drupal_set_message(t('%user was unsubscribed from the %newsletter mailing list.', array('%user' => $subs->mail, '%newsletter' => $list->name)));
        drupal_goto(variable_get('site_frontpage', 'node'));
      }
      elseif ($op1 == 'add') {
        simplenews_subscribe_user($subs->mail, $tid, FALSE, 'website');
        if ($path = variable_get('simplenews_confirm_subscribe_page', '')) {
          $path = $path .'/'. implode('/', $arguments);
          drupal_goto($path);
        }
        drupal_set_message(t('%user was added to the %newsletter mailing list.', array('%user' => $subs->mail, '%newsletter' => $list->name)));
        drupal_goto(variable_get('site_frontpage', 'node'));
      }
    }
  }

  // If md5 didn't match, do a not found.
  drupal_not_found();
  return;
}

/**
 * Generate the confirm subscription form.
 *
 * @see simplenews_confirm_add_form_submit()
 * @todo Add $form to drupal_get_form() calback functions: http://drupal.org/node/224333#hook_forms_signature
 */
function simplenews_confirm_add_form($form, &$form_state, $mail, $newsletter) {
  $form = array();
  $form['question'] = array(
    '#markup' => '<p>' . t('Are you sure you want to add %user to the %newsletter mailing list?', array('%user' => $mail, '%newsletter' => $newsletter->name)) . "<p>\n",
  );
  $form['mail'] = array(
    '#type' => 'value',
    '#value' => $mail,
  );
  $form['newsletter'] = array(
    '#type' => 'value',
    '#value' => $newsletter,
  );
  $form['#redirect'] = variable_get('simplenews_confirm_subscribe_page', '');

  return confirm_form($form, 
    t('Confirm subscription'), 
    '',
    t('You can always unsubscribe later.'), 
    t('Subscribe'), 
    t('Cancel')
  );
}

function simplenews_confirm_add_form_submit($form, &$form_state) {
  simplenews_subscribe_user($form_state['values']['mail'], $form_state['values']['newsletter']->slid, FALSE, 'website');
  
  // Display message if user is directed to the front page.
  if (!$form['#redirect']) {
    drupal_set_message(t('%user was added to the %newsletter mailing list.', array('%user' => $form_state['values']['mail'], '%newsletter' => $form_state['values']['newsletter']->name)));
  }
}

/**
 * Generate the confirm unsubscription form.
 *
 * @see simplenews_confirm_removal_form_submit()
 * @todo Add $form to drupal_get_form() calback functions: http://drupal.org/node/224333#hook_forms_signature
 */
function simplenews_confirm_removal_form($form, &$form_state, $mail, $newsletter) {
  $form = array();
  $form['question'] = array(
    '#markup' => '<p>' . t('Are you sure you want to remove %user from the %newsletter mailing list?', array('%user' => $mail, '%newsletter' => $newsletter->name)) . "<p>\n",
  );
  $form['mail'] = array(
    '#type' => 'value',
    '#value' => $mail,
  );
  $form['newsletter'] = array(
    '#type' => 'value',
    '#value' => $newsletter,
  );
  $form['#redirect'] = variable_get('simplenews_confirm_unsubscribe_page', '');

  return confirm_form($form, 
    t('Confirm remove subscription'), 
    '',
    t('This action will unsubscribe you from the newsletter mailing list.'), 
    t('Unsubscribe'), 
    t('Cancel')
  );
}

function simplenews_confirm_removal_form_submit($form, &$form_state) {
  simplenews_unsubscribe_user($form_state['values']['mail'], $form_state['values']['newsletter']->slid, FALSE, 'website');

  // Display message if user is directed to the front page.
  if (!$form['#redirect']) {
    drupal_set_message(t('%user was unsubscribed from the %newsletter mailing list.', array('%user' => $form_state['values']['mail'], '%newsletter' => $form_state['values']['newsletter']->name)));
  }
}

/**
 * Menu callback: handle the edit subscription page and a subscription
 * page for anonymous users.
 */
function simplenews_admin_users_form($form, &$form_state, $snid = NULL) {
  $form = simplenews_subscription_manager_form($form_state, $snid);
  $form['#redirect'] = 'admin/content/simplenews/users';
  return $form;
}
