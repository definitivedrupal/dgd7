<?php

/**
 * @file
 * mailchimp_lists module admin settings.
 */

function mailchimp_lists_overview_page() {
  $lists = mailchimp_lists_load_multiple();
  $rows = array();
  foreach ($lists as $list) {
    $mc_list = mailchimp_get_list($list->mc_list_id);
    $actions = array(
      l(t('Edit'), 'admin/config/services/mailchimp/lists/' . $list->id . '/edit'),
      l(t('Delete'), 'admin/config/services/mailchimp/lists/' . $list->id . '/delete')
    );
    if ($list->list_type !== MAILCHIMP_LISTTYPE_FREEFORM && isset($list->settings['cron']) && $list->settings['cron']) {
      $actions[] = l(t('Queue existing'), 'mailchimp/lists/' . $list->id . '/queue_existing');
    }
    $rows[] = array(
      l($list->label, 'admin/config/services/mailchimp/lists/' . $list->id . '/edit'),
      l($mc_list['name'], 'https://admin.mailchimp.com/lists/dashboard/overview?id=' . $mc_list['web_id']),
      $list->description,
      $list->list_type,
      implode(' | ', $actions)
    );
  }
  $table = array(
    'header' => array(
      t('Name'),
      t('MailChimp List'),
      t('Description'),
      t('Type'),
      t('Actions')
    ),
    'rows' => $rows
  );

  return theme('table', $table);
}

/**
 * @file
 * mailchimp_lists refresh page.
 */

function mailchimp_lists_refresh_page() {
  cache_clear_all('mailchimp_lists', 'cache');
  drupal_set_message(t('Mailchimp Lists refreshed'), 'status');
  // return render array.
  return array(
    '#markup' => t('Back to !link.',
      array('!link' => l('my lists', 'admin/config/services/mailchimp/lists')))
  );
}

/**
 * Return a form for adding/editing a mailchimp list.
 *
 * @param array $form
 * @param array $form_state
 * @param object $list
 */
function mailchimp_lists_list_form($form, &$form_state, $list = NULL) {
  $form = array();

  // store the existing list for updating on submit
  if (isset($list)) {
    $form_state['list'] = $list;
  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t('The label for this list that will appear on forms.'),
    '#size' => 35,
    '#maxlength' => 32,
    '#default_value' => $list ? $list->label : '',
    '#required' => TRUE
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
    // '#text_format'    => NULL, // @todo: should store this value
    '#description' => t('This description will be shown to the user on the
      list signup and user account settings pages')
  );

  $form['list_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('List Settings'),
    '#collapsible' => TRUE,
  );

  $form['list_settings']['list_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of list'),
    '#multiple' => FALSE,
    '#description' => t('Required and optional lists are automatically syncronized with the
      sites users. Optional lists provide a checkbox allowing users to
      subscribe during registration or when updating their account. Free form
      lists, the only type allowed for the Anonymous role, present a signup form
      with all MailChimp merge fields displayed. Default values are allowed for
      authenticated users based on token mappings.'),
    '#options' => array(
      '' => t('-- Select --'),
      MAILCHIMP_LISTTYPE_REQUIRED => t('Required'),
      MAILCHIMP_LISTTYPE_OPTIONAL => t('Optional'),
      MAILCHIMP_LISTTYPE_FREEFORM => t('Free form')
    ),
    '#default_value' => $list ? $list->list_type : -1,
    '#required' => TRUE,
  );

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
    '#states' => array(
      // Hide this option for required lists
      'invisible' => array(
        ':input[name="list_type"]' => array('value' => MAILCHIMP_LISTTYPE_REQUIRED),
      ),
    ),
  );

  $form['list_settings']['settings']['show_register_form'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show subscription options on the user registration form.'),
    '#description' => t('This will only apply for lists granted to an authenticated role.'),
    '#default_value' => isset($list->settings['show_register_form']) ? $list->settings['show_register_form'] : FALSE,
    '#states' => array(
      // only show for optional lists
      'visible' => array(
        ':input[name="list_type"]' => array('value' => MAILCHIMP_LISTTYPE_OPTIONAL),
      ),
    ),
  );
  $form['list_settings']['settings']['show_account_form'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Subscription Options on User Edit Screen'),
    '#description' => t('If set, a tab will be presented for managing
      newsletter subscriptions when editing an account.'),
    '#default_value' => isset($list->settings['show_account_form']) ? $list->settings['show_account_form'] : FALSE,
    '#states' => array(
      // only show for optional lists
      'visible' => array(
        ':input[name="list_type"]' => array('value' => MAILCHIMP_LISTTYPE_OPTIONAL),
      ),
    ),
  );
  $form['list_settings']['settings']['include_interest_groups'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include interest groups on subscription form.'),
    '#default_value' => isset($list->settings['include_interest_groups']) ?
      $list->settings['include_interest_groups'] : FALSE,
    '#description' => t('If set, users will be able to select applicable
      interest groups when registering or editing their accounts.'),
    '#states' => array(
      // hide for required groups
      'invisible' => array(
        ':input[name="list_type"]' => array('value' => MAILCHIMP_LISTTYPE_REQUIRED),
      ),
    ),
  );
  $form['list_settings']['settings']['interest_groups_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Interest groups label'),
    '#default_value' => isset($list->settings['interest_groups_label']) ?
      $list->settings['interest_groups_label'] : t('Interest Groups'),
    '#description' => t('Label to appear above the interest group options.'),
    '#size' => 40,
    '#maxlength' => 255,
    '#states' => array(
      'visible' => array(
        ':input[name="include_interest_groups"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['list_settings']['settings']['cron'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sync List During Cron'),
    '#default_value' => isset($list->settings['cron']) ? $list->settings['cron'] : FALSE,
    '#description' => t('If this is set, users will be subscribed to the
      required list during cron runs. Otherwise subscription will take place when a user is added/edited.'),
    '#states' => array(
      // Hide this option for freeform lists.
      'invisible' => array(
        ':input[name="list_type"]' => array('value' => MAILCHIMP_LISTTYPE_FREEFORM),
      ),
    ),
  );
  $form['list_settings']['settings']['webhooks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable MailChimp webhooks for this list'),
    '#default_value' => isset($list->settings['webhooks']) ? $list->settings['webhooks'] : FALSE,
    '#description' => t('When a user unsubscribes from a list or updates
      their profile outside of Drupal, MailChimp will trigger an event to
      update the user\'s cached MailChimp member information. This will
      <em>not</em> update any of their Drupal user information.'),
    '#states' => array(
      // Hide this option for freeform lists
      'invisible' => array(
        ':input[name="list_type"]' => array('value' => MAILCHIMP_LISTTYPE_FREEFORM),
      ),
    ),
  );

  $form['list_settings']['roles_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Roles')
  );
  $form['list_settings']['roles_fieldset']['roles'] = array(
    '#type' => 'checkboxes',
    '#options' => user_roles(),
    '#description' => t('Choose which roles may subscribe to this list. All
      users will see the lists they\'re eligble for at the !subscribe and in
      the subscription block. Lists assigned to the Authenticated role may
      also apear in the registration form if that option is selected below.
      Authenticated user may also manage their list settings from their profile.
      The Anonymous role may <em>only</em> be set for free form lists.',
      array(
        '!subscribe' => l(t('newsletter subscription page'),
          'mailchimp/subscribe')
      )),
    '#default_value' => ($list && !empty($list->settings['roles'])) ? $list->settings['roles'] : array(),
  );

  $form['mc_list'] = array(
    '#type' => 'fieldset',
    '#title' => t('MailChimp List Settings'),
    '#collapsible' => TRUE,
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
      array('!MailChimp' => l('MailChimp', 'https://admin.mailchimp.com'))),
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
        'message' => t('Retrieving merge fields for this list.')
      )
    )
  );

  $form['mc_list']['mergefields'] = array(
    '#prefix' => '<div id="mergefields-wrapper">',
    '#suffix' => '</div>'
  );

  // show merge fields if changing list field or editing existing list
  if (!empty($form_state['values']['mc_list_id']) || isset($list)) {
    $form['mc_list']['mergefields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Merge Fields'),
      '#id' => 'mergefields-wrapper',
      '#tree' => TRUE
    );

    $mc_list_id = !empty($form_state['values']['mc_list_id']) ?
      $form_state['values']['mc_list_id'] : $list->mc_list_id;
    $mc_list = mailchimp_get_list($mc_list_id);

    if (isset($mc_list['mergevars']) && !empty($mc_list['mergevars'])) {
      foreach ($mc_list['mergevars'] as $mergevar) {
        $default_value = isset($list->settings['mergefields'][$mergevar['tag']]) ?
          $list->settings['mergefields'][$mergevar['tag']] : -1;
        $disabled = FALSE;
        $description = '';
        if ($mergevar['tag'] == 'EMAIL') {
          $default_value = 'mail';
          $disabled = TRUE;
          $description = t('Email is required and must map to a Drupal user\'s email.');
        }
        $form['mc_list']['mergefields'][$mergevar['tag']] = array(
          '#type' => 'select',
          '#title' => $mergevar['name'],
          '#description' => $description,
          '#default_value' => $default_value,
          '#disabled' => $disabled,
          '#required' => $mergevar['req'],
          '#options' => mailchimp_lists_get_merge_tokens()
        );
      }
    }
    else {
      $form['mc_list']['mergefields']['message'] = array(
        '#markup' => t('There are no merge fields configured for this list.')
      );
    }
  }

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
    '#href' => 'admin/config/services/mailchimp/lists'
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
 * mailchimp_lists_list_form() validation handler.
 */
function mailchimp_lists_list_form_validate($form, &$form_state) {
  // anonymous role can only be applied to free form lists
  if ($form_state['values']['list_type'] != MAILCHIMP_LISTTYPE_FREEFORM &&
    $form_state['values']['roles'][DRUPAL_ANONYMOUS_RID]
  ) {
    form_set_error('list_type',
      t('The anonymous role can only be set for free form lists.'));
  }

  // required lists must have a role selected (other than anon)
  if ($form_state['values']['list_type'] == MAILCHIMP_LISTTYPE_REQUIRED) {
    $roles = $form_state['values']['roles'];
    // unset anonymous role
    unset($roles[DRUPAL_ANONYMOUS_RID]);
    // check if there was a role selected
    $roles = array_filter($roles);
    if (empty($roles)) {
      form_set_error('roles',
        t('Required lists must have a role (other than Anonymous) selected.'));
    }
  }

  // ensure mail merge field is set correctly
  if (!isset($form_state['values']['mergefields']['EMAIL']) ||
    $form_state['values']['mergefields']['EMAIL'] != 'mail'
  ) {
    form_set_error('EMAIL',
      t('The email merge field must be set to the user mail token.'));
  }
}

/**
 * mailchimp_lists_list_form() submit handler.
 */
function mailchimp_lists_list_form_submit($form, &$form_state) {
  $list = new stdClass();
  $is_new = TRUE;
  if (isset($form_state['list'])) {
    $list = $form_state['list'];
    $is_new = FALSE;
  }

  // set this value here for use in webhook call below
  $list->mc_list_id = $form_state['values']['mc_list_id'];

  $mcapi = mailchimp_get_api_object();
  $webhook_ret = FALSE;
  // remove the webhook from this list if unchecked or list type changed to freeform
  if (!$is_new && isset($list->settings['webhooks']) &&
    (!$form_state['values']['webhooks'] ||
      $form_state['values']['list_type'] == MAILCHIMP_LISTTYPE_FREEFORM)
  ) {
    $webhook_ret = $mcapi->listWebhookDel($list->mc_list_id,
      mailchimp_webhook_url());
  }
  // enable the web hook
  else {
    if (
      (!isset($list->settings['webhooks']) || !$list->settings['webhooks']) &&
      $form_state['values']['webhooks']
    ) {
      $webhook_ret = $mcapi->listWebhookAdd($list->mc_list_id,
        mailchimp_webhook_url(),
        array('unsubscribe', 'profile', 'cleaned', 'upemail')
      );

      // handle errors in setting webooks
      if (!$webhook_ret) {
        drupal_set_message(t('Unable to connect webhooks: @msg',
          array('@msg' => $mcapi->errorMessage)), 'error');
      }
    }
  }

  $list->label = $form_state['values']['label'];
  $list->name = $form_state['values']['name'];
  $list->description = $form_state['values']['description'];
  $list->list_type = $form_state['values']['list_type'];
  $list->settings = array(
    'roles' => array_filter($form_state['values']['roles']),
    'mergefields' => isset($form_state['values']['mergefields']) ?
      $form_state['values']['mergefields'] : NULL,
    'doublein' => $form_state['values']['doublein'],
    'show_register_form' => $form_state['values']['show_register_form'],
    'show_account_form' => $form_state['values']['show_account_form'],
    'include_interest_groups' => $form_state['values']['include_interest_groups'],
    'interest_groups_label' => $form_state['values']['interest_groups_label'],
    'cron' => $form_state['values']['cron'],
    'webhooks' => $webhook_ret & $form_state['values']['webhooks'],
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
 *
 * @param string $form
 * @param string $form_state
 * @param object $list
 */
function mailchimp_lists_delete_list_form($form, &$form_state, $list) {
  $form_state['list'] = $list;
  return confirm_form($form,
    t('Are you sure you want to delete %name?', array('%name' => $list->label)),
    'admin/config/services/mailchimp/lists/' . $list->id . '/edit',
    t('This action cannot be undone, although it will not have any effect on the MailChimp list.'),
    t('Delete list'));
}

/**
 * Submit hanler for mailchimp_lists_delete_list_form();
 *
 * @param string $form
 * @param string $form_state
 */
function mailchimp_lists_delete_list_form_submit($form, &$form_state) {
  $list = $form_state['list'];
  mailchimp_lists_delete_multiple(array($list->id));
  drupal_set_message(t('%name has been deleted.', array('%name' => $list->label)));
  $form_state['redirect'] = 'admin/config/services/mailchimp/lists';
}

/**
 * Confirmation form for queing existing users.
 *
 * @param string $form
 * @param string $form_state
 * @param object $list
 */
function mailchimp_lists_queue_existing_form($form, &$form_state, $list) {
  $form_state['list'] = $list;
  return confirm_form($form,
    t('Are you sure you want to queue existing users in %name?', array('%name' => $list->label)),
    'admin/config/services/mailchimp/lists',
    t('This action will queue all users who belong in this list based on roles
       for processing and cannot be undone. If you have lots of users, it could
       take some time and possibly timeout.'),
    t('Queue existing users'));
}

/**
 * Submit handler for mailchimp_lists_queue_existing_form();
 *
 * @param string $form
 * @param string $form_state
 */
function mailchimp_lists_queue_existing_form_submit($form, &$form_state) {
  $list = $form_state['list'];
  $count = mailchimp_lists_queue_existing($list);
  drupal_set_message(t('@count users have been queued for update in %list.', array(
    '@count' => $count,
    '%list' => $list->label
  )));
  $form_state['redirect'] = 'admin/config/services/mailchimp/lists';
}
