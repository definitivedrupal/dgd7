<?php

/**
 * @file
 * mailchimp_lists module admin settings.
 */

/**
 * Administrative display showing existing lists and allowing edits/adds.
 */
function mailchimp_lists_overview_page() {
  $lists = mailchimp_lists_load_multiple();
  $rows = array();
  $roles = user_roles();
  foreach ($lists as $list) {
    $mc_list = mailchimp_get_list($list->mc_list_id);
    $actions = array(
      l(t('Edit'), 'admin/config/services/mailchimp/lists/' . $list->id . '/edit'),
      l(t('Delete'), 'admin/config/services/mailchimp/lists/' . $list->id . '/delete'),
    );
    if (isset($list->settings['cron']) && $list->settings['cron']) {
      $actions[] = l(t('Queue existing'), 'mailchimp/lists/' . $list->id . '/queue_existing');
    }
    $rolelist = $list->settings['roles'];
    unset($rolelist[DRUPAL_ANONYMOUS_RID]);
    $role_display = array();
    foreach ($rolelist as $role_id) {
      $role_display[] = $roles[$role_id];
    }
    $role_display = implode(', ', $role_display);
    $rows[] = array(
      l($list->label(), 'admin/config/services/mailchimp/lists/' . $list->id . '/edit'),
      l($mc_list['name'], 'https://admin.mailchimp.com/lists/dashboard/overview?id=' . $mc_list['web_id']),
      $list->description,
      $list->settings['required'] ? $role_display : '-No-',
      $list->settings['allow_anonymous'] ? '-Yes-' : '-No-',
      implode(' | ', $actions),
    );
  }
  $table = array(
    'header' => array(
      t('Name'),
      t('MailChimp List'),
      t('Description'),
      t('Required'),
      t('Anonymous Allowed'),
      t('Actions'),
    ),
    'rows' => $rows,
  );
  $mc_lists = mailchimp_get_lists();
  if (empty($mc_lists)) {
    drupal_set_message(t('You don\'t have any lists configured in your MailChimp account, (or you haven\'t configured your API key correctly on the Global Settings tab). Head over to !link and create some lists, then come back here and click "Refresh lists from MailChimp!"',
      array('!link' => l(t('MailChimp'), 'https://admin.mailchimp.com/lists/'))), 'warning');
  }
  else {
    $options = 'Currently Available MailChimp lists:<i>';
    foreach ($mc_lists as $mc_list) {
      $options .= ' ' . $mc_list['name'] . ',';
    }
    $options = rtrim($options, ',');
    $options .= ".</i>";
    $table['caption'] = $options;
  }

  return theme('table', $table);
}

/**
 * Page for the refresh mailchimp_lists function.
 */
function mailchimp_lists_refresh_page() {
  cache_clear_all('mailchimp_lists', 'cache');
  drupal_set_message(t('Mailchimp Lists refreshed'), 'status');
  // Return render array.
  return array(
    '#markup' => t('Back to !link.',
      array('!link' => l(t('my lists'), 'admin/config/services/mailchimp/lists'))),
  );
}

/**
 * Return a form for adding/editing a mailchimp list.
 */
function mailchimp_lists_list_form($form, &$form_state, MailchimpList $list = NULL) {
  $form = array();

  // Store the existing list for updating on submit.
  if (isset($list)) {
    $form_state['list'] = $list;
  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t('The label for this list that appears in the admin UI and the default Block Title.'),
    '#size' => 35,
    '#maxlength' => 32,
    '#default_value' => $list ? $list->label : '',
    '#required' => TRUE,
  );

  // Machine-readable list name.
  $status = isset($list->status) && $list->id && (($list->status & ENTITY_IN_CODE) || ($list->status & ENTITY_FIXED));
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($list->name) ? $list->name : '',
    '#maxlength' => 32,
    '#disabled' => $status,
    '#machine_name' => array(
      'exists' => 'mailchimp_lists_load_multiple_by_name',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this list. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#title' => 'Description',
    '#type' => 'textarea',
    '#default_value' => $list ? $list->description : '',
    '#rows' => 2,
    '#maxlength' => 255,
    '#description' => t('This description will be shown to the user on the list signup and user account settings pages. (255 characters or less)'),
  );

  $form['mc_list'] = array(
    '#type' => 'fieldset',
    '#title' => t('MailChimp List & Merge Fields'),
    '#collapsible' => TRUE,
    '#collapsed' => isset($list),
  );

  $form['list_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
  );

  $form['form_option_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form & Subscribe Block Options'),
  );
 
  $lists = mailchimp_get_lists();
  $options = array('' => t('-- Select --'));
  foreach ($lists as $mc_list) {
    $options[$mc_list['id']] = $mc_list['name'];
  }
  $form['mc_list']['mc_list_id'] = array(
    '#type' => 'select',
    '#title' => t('MailChimp List'),
    '#multiple' => FALSE,
    '#description' => t('Available MailChimp lists. If this field is empty,
      create a list at !MailChimp first.',
      array('!MailChimp' => l(t('MailChimp'), 'https://admin.mailchimp.com'))),
    '#options' => $options,
    '#default_value' => $list ? $list->mc_list_id : -1,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'mailchimp_lists_mergefields_callback',
      'wrapper' => 'mergefields-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
      'progress' => array(
        'type' => 'throbber',
        'message' => t('Retrieving merge fields for this list.'),
      ),
    ),
  );

  $form['mc_list']['mergefields'] = array(
    '#prefix' => '<div id="mergefields-wrapper">',
    '#suffix' => '</div>',
  );

  // Show merge fields if changing list field or editing existing list.
  if (!empty($form_state['values']['mc_list_id']) || isset($list)) {
    $form['mc_list']['mergefields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Merge Fields'),
      '#id' => 'mergefields-wrapper',
      '#tree' => TRUE,
    );
    $form['form_option_settings']['mergefields_display'] = array(
      '#type' => 'fieldset',
      '#title' => t('Merge Field Form Display Settings'),
      '#description' => t('Select the merge fields to show on anonymous registration forms. Required fields are automatically displayed.'),
      '#id' => 'mergefields-display-wrapper',
      '#tree' => TRUE,
      '#weight' => 20,
      '#states' => array(
        'visible' => array(
          ':input[name="allow_anonymous"]' => array('checked' => TRUE),
        ),
      ),
    );
    $mc_list_id = !empty($form_state['values']['mc_list_id']) ? $form_state['values']['mc_list_id'] : $list->mc_list_id;
    $mc_list = mailchimp_get_list($mc_list_id);

    if (isset($mc_list['mergevars']) && !empty($mc_list['mergevars'])) {
      foreach ($mc_list['mergevars'] as $mergevar) {
        $default_value = isset($list->settings['mergefields'][$mergevar['tag']]) ? $list->settings['mergefields'][$mergevar['tag']] : -1;
        $disabled = FALSE;
        $description = '';
        if ($mergevar['tag'] == 'EMAIL') {
          $default_value = 'mail';
          $disabled = TRUE;
          $description = t("Email is required and must map to a Drupal user's email.");
        }
        $form['mc_list']['mergefields'][$mergevar['tag']] = array(
          '#type' => 'select',
          '#title' => $mergevar['name'],
          '#description' => $description,
          '#default_value' => $default_value,
          '#disabled' => $disabled,
          '#required' => $mergevar['req'],
          '#options' => mailchimp_lists_get_merge_tokens(),
        );
        if (!$mergevar['req']) {
          $form['form_option_settings']['mergefields_display'][$mergevar['tag']] = array(
            '#type' => 'checkbox',
            '#title' => $mergevar['name'],
            '#default_value' => isset($list->settings['mergefields_display'][$mergevar['tag']]) ? $list->settings['mergefields_display'][$mergevar['tag']] : TRUE,
            '#required' => $mergevar['req'],
          );
        }
        else {
          $form['form_option_settings']['mergefields_display'][$mergevar['tag']] = array(
            '#type' => 'hidden',
            '#value' => TRUE,
          );
        }
      }
    }
    else {
      $form['mc_list']['mergefields']['message'] = array(
        '#markup' => t('There are no merge fields configured for this list.'),
      );
    }
  }

  $form['list_settings']['allow_anonymous'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow anonymous registrations. <em>(Formerly "Freeform List")</em>'),
    '#description' => t('This allows site visitors who do not have an account on your site to register for this list using a form block.'),
    '#default_value' => ($list && !empty($list->settings['allow_anonymous'])) ? $list->settings['allow_anonymous'] : FALSE,
  );

  $form['list_settings']['required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically add all eligible users. <em>(Formerly "Required List")</em>'),
    '#description' => t('Adds all registered users with appropriate Roles to the list automatically. This will block them from controlling their subscription status through Drupal. (Email confirmation from Mailchimp may still be required.)'),
    '#default_value' => ($list && !empty($list->settings['required'])) ? $list->settings['required'] : FALSE,
  );

  $form['list_settings']['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Roles'),
    '#tree' => TRUE,
    '#description' => t('<p>Choose which roles may subscribe to this list. All
      users will see the lists they\'re eligible for at the !subscribe and in
      the subscription block. Lists assigned to the Authenticated role may
      also apear in the registration form if that option is selected below.
      Authenticated user may also manage their list settings from their profile.
      The Anonymous role may <em>only</em> be set for free form lists.</p>',
      array(
        '!subscribe' => l(t('newsletter subscription page'),
          'mailchimp/subscribe'),
      )),
  );
  $roles = user_roles();
  foreach ($roles as $rid => $role) {
    $form['list_settings']['roles'][$rid] = array(
      '#type' => 'checkbox',
      '#tree' => TRUE,
      '#title' => $role,
      '#return_value' => $rid,
      '#default_value' => ($list && !empty($list->settings['roles'][$rid])) ? $list->settings['roles'][$rid] : FALSE,
    );
  }
  // We don't show the anonymous role: instead we use the "allow anonymous"
  // setting:
  unset($form['list_settings']['roles'][DRUPAL_ANONYMOUS_RID]);

  $form['list_settings']['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
  );

  $form['list_settings']['settings']['doublein'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require subscribers to Double Opt-in'),
    '#description' => t('New subscribers will be sent a link with an email
      they must follow to confirm their subscription.'),
    '#default_value' => isset($list->settings['doublein']) ? $list->settings['doublein'] : FALSE,
  );

  $form['list_settings']['settings']['show_register_form'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show subscription options on the user registration form.'),
    '#description' => t('This will only apply for lists granted to an authenticated role. <em>(If the list is set to "Automatically add", only the title and Interest Group options will appear.)</em>'),
    '#default_value' => isset($list->settings['show_register_form']) ? $list->settings['show_register_form'] : FALSE,
  );
  $form['list_settings']['settings']['show_account_form'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show subscription options on user edit screen'),
    '#description' => t('If set, a tab will be presented for managing newsletter subscriptions when editing an account. <em>(If the list is set to "Automatically add", the option to unsubscribe will be disabled, but users can configure their Interest Groups.)</em>'),
    '#default_value' => isset($list->settings['show_account_form']) ? $list->settings['show_account_form'] : FALSE,
  );
  $form['list_settings']['settings']['cron'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sync List During Cron'),
    '#default_value' => isset($list->settings['cron']) ? $list->settings['cron'] : FALSE,
    '#description' => t('If this is set, users will be subscribed to the
      required list during cron runs. Otherwise subscription will take place when a user is added/edited.'),
  );
  $form['list_settings']['settings']['webhooks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable MailChimp webhooks for this list'),
    '#default_value' => isset($list->settings['webhooks']) ? $list->settings['webhooks'] : FALSE,
    '#description' => t("When a user unsubscribes from a list or updates
      their profile outside of Drupal, MailChimp will trigger an event to
      update the user's cached MailChimp member information. This will
      <em>not</em> update any of their Drupal user information."),
  );

  $form['form_option_settings']['form_label'] = array(
    '#type' => 'textfield',
    '#title' => t('List Label'),
    '#description' => t('The label for this list that will appear on forms, either next to a checkbox or above the list merge fields, depending on the type of list.'),
    '#size' => 40,
    '#maxlength' => 64,
    '#default_value' => isset($list->settings['form_label']) ? $list->settings['form_label'] : '',
  );
  $form['form_option_settings']['submit_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Submit Button Label'),
    '#description' => t('The label for the Submit button that will appear on subscription forms that include only this list: generally blocks only.'),
    '#size' => 20,
    '#maxlength' => 20,
    '#default_value' => isset($list->settings['submit_label']) ? $list->settings['submit_label'] : 'Submit',
  );
  $form['form_option_settings']['default_register_form_optin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default to opt-in on registration form.'),
    '#description' => t('This will only apply for lists appearing on the register form.'),
    '#default_value' => isset($list->settings['default_register_form_optin']) ? $list->settings['default_register_form_optin'] : FALSE,
    '#states' => array(
      // Hide for required lists.
      'disabled' => array(
        ':input[name="show_register_form"]' => array('checked' => FALSE),
      ),
      'checked' => array(
        ':input[name="required"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['form_option_settings']['include_interest_groups'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include interest groups on subscription form.'),
    '#default_value' => isset($list->settings['include_interest_groups']) ? $list->settings['include_interest_groups'] : FALSE,
    '#description' => t('If set, users will be able to select applicable interest groups when registering or editing their accounts.'),
  );
  $form['form_option_settings']['opt_in_interest_groups'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default all groups to opt-in.'),
    '#default_value' => isset($list->settings['opt_in_interest_groups']) ? $list->settings['opt_in_interest_groups'] : FALSE,
    '#description' => t('If set, the registration form will check all interest groups by default.'),
    '#states' => array(
      'visible' => array(
        ':input[name="include_interest_groups"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['form_option_settings']['interest_groups_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Interest groups label'),
    '#default_value' => isset($list->settings['interest_groups_label']) ? $list->settings['interest_groups_label'] : t('Interest Groups'),
    '#description' => t('Label to appear above the interest group options.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#states' => array(
      'enabled' => array(
        ':input[name="include_interest_groups"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#access' => isset($list),
    '#submit' => array('mailchimp_lists_list_delete_submit'),
  );
  $form['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/services/mailchimp/lists',
  );

  return $form;
}

/**
 * AJAX callback to return fields for a given type.
 */
function mailchimp_lists_mergefields_callback($form, $form_state) {
  return $form['mc_list']['mergefields'];
}

/**
 * Validation handler for mailchimp_lists_list_form().
 */
function mailchimp_lists_list_form_validate($form, &$form_state) {
  // Required lists must have a role selected (other than anon).
  if ($form_state['values']['required']) {
    // Check if there was a role selected:
    $roles = array_filter($form_state['values']['roles']);
    if (empty($roles)) {
      form_set_error('roles',
        t('Required lists must have a role selected.'));
    }
  }

  // Ensure mail merge field is set correctly:
  if (!isset($form_state['values']['mergefields']['EMAIL']) ||
    $form_state['values']['mergefields']['EMAIL'] != 'mail'
  ) {
    form_set_error('EMAIL',
      t('The email merge field must be set to the user mail token.'));
  }
}

/**
 * Submit handler for mailchimp_lists_list_form().
 */
function mailchimp_lists_list_form_submit($form, &$form_state) {
  $list = isset($form_state['list']) ? $form_state['list'] : mailchimp_list_create();
  $is_new = isset($list->is_new) && $list->is_new;

  // Set this value here for use in webhook call below:
  $list->mc_list_id = $form_state['values']['mc_list_id'];

  $mcapi = mailchimp_get_api_object();
  $webhook_ret = FALSE;
  // If the webhook was enabled before, check to see if it should be disabled:
  if (!empty($list->settings['webhooks'])) {
    if (!$is_new && !$form_state['values']['webhooks']) {
      $webhook_ret = $mcapi->listWebhookDel($list->mc_list_id, mailchimp_webhook_url());
    }
  }
  // If it wasn't enabled, check to see if it should be:
  else {
    if ($form_state['values']['webhooks']) {
      $webhook_ret = $mcapi->listWebhookAdd($list->mc_list_id,
        mailchimp_webhook_url(),
        array('unsubscribe', 'profile', 'cleaned', 'upemail')
      );

      // Handle errors in setting webooks:
      if (!$webhook_ret) {
        drupal_set_message(t('Unable to connect webhooks: @msg',
          array('@msg' => $mcapi->errorMessage)), 'error');
      }
    }
  }
  // Check for altered roles.
  $added_roles = FALSE;
  $removed_roles = FALSE;
  // We intuit the anonymous RID based on the Allow Anonymous setting:
  $form_state['values']['roles'][DRUPAL_ANONYMOUS_RID] = $form_state['values']['allow_anonymous'];
  if (!$is_new) {
    foreach ($form_state['values']['roles'] as $role_id => $role) {
      $was_on = !empty($form_state['list']->settings['roles'][$role_id]);
      $set_on = !empty($role);
      if ($was_on && !$set_on) {
        $removed_roles = TRUE;
      }
      if (!$was_on && $set_on) {
        $added_roles = TRUE;
      }
      if ($added_roles && $removed_roles) {
        break;
      }
    }
  }
  if ($added_roles || $removed_roles) {
    mailchimp_lists_queue_existing($list, $removed_roles);
  }

  $list->label = $form_state['values']['label'];
  $list->name = $form_state['values']['name'];
  $list->description = $form_state['values']['description'];
  $list->settings = array(
    'roles' => array_filter($form_state['values']['roles']),
    'mergefields' => isset($form_state['values']['mergefields']) ? $form_state['values']['mergefields'] : NULL,
    'mergefields_display' => isset($form_state['values']['mergefields_display']) ? $form_state['values']['mergefields_display'] : NULL,
    'doublein' => $form_state['values']['doublein'],
    'show_register_form' => $form_state['values']['show_register_form'],
    'default_register_form_optin' => $form_state['values']['default_register_form_optin'],
    'show_account_form' => $form_state['values']['show_account_form'],
    'form_label' => $form_state['values']['form_label'],
    'submit_label' => $form_state['values']['submit_label'],
    'include_interest_groups' => $form_state['values']['include_interest_groups'],
    'opt_in_interest_groups' => $form_state['values']['opt_in_interest_groups'],
    'interest_groups_label' => $form_state['values']['interest_groups_label'],
    'cron' => $form_state['values']['cron'],
    'webhooks' => $webhook_ret & $form_state['values']['webhooks'],
    'allow_anonymous' => $form_state['values']['allow_anonymous'],
    'required' => $form_state['values']['required'],
  );

  if ($ret = mailchimp_lists_save($list)) {
    drupal_set_message(t('List @name has been saved.',
      array('@name' => $list->label)));
    $form_state['redirect'] = 'admin/config/services/mailchimp/lists';
  }
  else {
    drupal_set_message(t('There has been an error saving your list.'), 'error');
  }
}

/**
 * Submit function for the delete button on the menu item editing form.
 */
function mailchimp_lists_list_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/config/services/mailchimp/lists/' . $form_state['list']->id . '/delete';
}

/**
 * List deletion form.
 */
function mailchimp_lists_delete_list_form($form, &$form_state, $list) {
  $form_state['list'] = $list;
  return confirm_form($form,
    t('Are you sure you want to delete %name?', array('%name' => $list->label())),
    'admin/config/services/mailchimp/lists/' . $list->id . '/edit',
    t('This action cannot be undone, although it will not have any effect on the MailChimp list.'),
    t('Delete list'));
}

/**
 * Submit handler for mailchimp_lists_delete_list_form().
 */
function mailchimp_lists_delete_list_form_submit($form, &$form_state) {
  $list = $form_state['list'];
  mailchimp_lists_delete_multiple(array($list->id));
  drupal_set_message(t('%name has been deleted.', array('%name' => $list->label())));
  $form_state['redirect'] = 'admin/config/services/mailchimp/lists';
}

/**
 * Confirmation form for queing existing users.
 */
function mailchimp_lists_queue_existing_form($form, &$form_state, $list) {
  $form_state['list'] = $list;
  return confirm_form($form,
    t('Are you sure you want to queue existing users in %name?', array('%name' => $list->label())),
    'admin/config/services/mailchimp/lists',
    t('This action will queue all users who belong in this list based on roles
       for processing and cannot be undone. If you have lots of users, it could
       take some time and possibly timeout.'),
    t('Queue existing users'));
}

/**
 * Submit handler for mailchimp_lists_queue_existing_form().
 */
function mailchimp_lists_queue_existing_form_submit($form, &$form_state) {
  $list = $form_state['list'];
  $count = mailchimp_lists_queue_existing($list);
  drupal_set_message(t('@count users have been queued for update in %list.', array(
    '@count' => $count,
    '%list' => $list->label(),
  )));
  $form_state['redirect'] = 'admin/config/services/mailchimp/lists';
}
