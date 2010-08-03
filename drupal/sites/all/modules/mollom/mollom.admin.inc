<?php
// $Id: mollom.admin.inc,v 1.25 2010/05/31 21:27:06 dries Exp $

/**
 * @file
 * Administrative page callbacks for Mollom module.
 */

/**
 * Menu callback; Displays a list of forms configured for Mollom.
 */
function mollom_admin_form_list() {
  $modes = array(
    MOLLOM_MODE_DISABLED => t('No protection'),
    MOLLOM_MODE_CAPTCHA => t('CAPTCHA only'),
    MOLLOM_MODE_ANALYSIS => t('Text analysis and CAPTCHA backup'),
  );

  $header = array(
    t('Form'),
    t('Protection mode'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();
  $result = db_query('SELECT form_id FROM {mollom_form}')->fetchCol();
  foreach ($result as $form_id) {
    $mollom_form = mollom_form_load($form_id);
    $rows[] = array(
      $mollom_form['title'],
      $modes[$mollom_form['mode']],
      l(t('Configure'), 'admin/config/content/mollom/manage/' . $form_id),
      l(t('Unprotect'), 'admin/config/content/mollom/unprotect/' . $form_id),
    );
  }

  $build['forms'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => l(t('Add form'), 'admin/config/content/mollom/add'),
  );

  return $build;
}

/**
 * Return registered forms as an array suitable for a 'checkboxes' form element #options property.
 */
function mollom_admin_form_options() {
  // Retrieve all registered forms.
  $form_list = mollom_form_list();

  // Remove already configured form ids.
  $result = db_query('SELECT form_id FROM {mollom_form}')->fetchCol();
  foreach ($result as $form_id) {
    unset($form_list[$form_id]);
  }
  // If all registered forms are configured already, output a message, and
  // redirect the user back to overview.
  if (empty($form_list)) {
    drupal_set_message(t('All available forms are protected already.'));
    drupal_goto('admin/config/content/mollom');
  }

  // Load module information.
  $modules = module_implements('mollom_form_info');
  $modules = db_query("SELECT name, info FROM {system} WHERE type = :type AND name IN (:name)", array(':type' => 'module', ':name' => $modules))->fetchAllKeyed();
  foreach ($modules as $name => $info) {
    $module_info = unserialize($info);
    $modules[$name] = t($module_info['name']);
  }

  // Transform form information into an associative array suitable for #options.
  foreach ($form_list as $form_id => $info) {
    $form_list[$form_id] = $modules[$info['module']] . ': ' . $info['title'];
  }
  // Sort form options by title.
  asort($form_list);

  return $form_list;
}

/**
 * Form builder; Configure Mollom protection for a form.
 */
function mollom_admin_configure_form($form, &$form_state, $mollom_form = NULL) {
  // If no $mollom_form was passed, then we are adding a new form configuration.
  if (!isset($mollom_form)) {
    if (!isset($form_state['storage']['mollom_form'])) {
      $form_state['storage']['step'] = 'select';
    }
    else {
      $form_state['storage']['step'] = 'configure';
      $mollom_form = $form_state['storage']['mollom_form'];
    }
  }
  // Otherwise, we are editing an existing form configuration.
  else {
    $form_state['storage']['step'] = 'configure';
    $form_state['storage']['mollom_form'] = $mollom_form;
  }

  $form['#tree'] = TRUE;
  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 100,
  );

  switch ($form_state['storage']['step']) {
    case 'select':
      drupal_add_js(drupal_get_path('module', 'mollom') . '/mollom.js');

      $form['mollom']['form_id'] = array(
        '#type' => 'select',
        '#title' => t('Form'),
        '#options' => mollom_admin_form_options(),
        '#required' => TRUE,
      );
      $form['actions']['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#submit' => array('mollom_admin_configure_form_next_submit'),
      );
      break;

    case 'configure':
      // Display a list of fields for textual analysis (last step).
      $form['mollom']['form_id'] = array(
        '#type' => 'value',
        '#value' => $mollom_form['form_id'],
      );
      $form['mollom']['form_title'] = array(
        '#type' => 'item',
        '#title' => t('Form'),
        '#markup' => $mollom_form['title'],
      );
      $form['mollom']['mode'] = array(
        '#type' => 'select',
        '#title' => t('Spam blocking'),
        '#options' => array(
          MOLLOM_MODE_CAPTCHA => t('CAPTCHA only'),
        ),
        '#default_value' => $mollom_form['mode'],
      );

      // Expose any individual form options from $mollom_form['data'].
      $form['mollom']['data']['#tree'] = TRUE;
      $form['mollom']['data']['profanity'] = array(
        '#type' => 'radios',
        '#title' => t('Profanity blocking (beta)'),
        '#options' => array(t('Disabled'), t('Enabled')),
        '#default_value' => isset($mollom_form['data']['profanity']) ? $mollom_form['data']['profanity'] : 0,
        '#states' => array(
          'visible' => array(
            'select[name="mollom[mode]"]' => array('value' => (string) MOLLOM_MODE_ANALYSIS),
          ),
        ),
      );

      if (!empty($mollom_form['elements'])) {
        // Add the text analysis protection mode option.
        $form['mollom']['mode']['#options'][MOLLOM_MODE_ANALYSIS] = t('Text analysis and CAPTCHA backup');
        $form['mollom']['mode']['#description'] = t('If <em>Text analysis and CAPTCHA backup</em> is selected, the user will only see a CAPTCHA until after the form has been submitted and Mollom\'s textual analysis has determined it is unsure about the content.');

        // Form elements defined by hook_mollom_form_info() use the
        // 'parent][child' syntax, which Form API also uses internally for
        // form_set_error(), and which allows us to recurse into nested fields
        // during processing of submitted form values. However, since we are using
        // those keys also as internal values to configure the fields to use for
        // textual analysis, we need to encode them. Otherwise, a nested field key
        // would result in the following checkbox attribute:
        //   '#name' => 'mollom[enabled_fields][parent][child]'
        // This would lead to a form validation error, because it is a valid key.
        // By encoding them, we prevent this from happening:
        //   '#name' => 'mollom[enabled_fields][parent%5D%5Bchild]'
        // @todo Use PHP5 functions in D7 (e.g. array_combine()).
        $elements = array();
        foreach ($mollom_form['elements'] as $key => $value) {
          $elements[rawurlencode($key)] = $value;
        }
        $enabled_fields = array();
        foreach ($mollom_form['enabled_fields'] as $value) {
          $enabled_fields[] = rawurlencode($value);
        }
        $form['mollom']['enabled_fields'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Fields to analyze for spam or profanity'),
          '#options' => $elements,
          '#default_value' => $enabled_fields,
          '#required' => $mollom_form['mode'] == MOLLOM_MODE_ANALYSIS,
          '#states' => array(
            'invisible' => array(
              'select[name="mollom[mode]"]' => array('value' => '1'),
            ),
          ),
        );

        if (!empty($form_state['input']['mollom']['mode'])) {
          // If the user is changing the current protection mode to textual
          // analysis, then make the fields checkboxes required.
          $form['mollom']['enabled_fields']['#required'] = $form_state['input']['mollom']['mode'] == MOLLOM_MODE_ANALYSIS;
        }
      }

      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
      );
      break;
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/content/mollom',
  );

  return $form;
}

/**
 * Form submit handler for 'Next' button on Mollom form configuration form.
 */
function mollom_admin_configure_form_next_submit($form, &$form_state) {
  $form_id = $form_state['values']['mollom']['form_id'];

  // Load form information into $form_state for configuration.
  $form_list = mollom_form_list();
  $mollom_form = mollom_form_info($form_id, $form_list[$form_id]['module']);

  // Enable all fields for textual analysis by default.
  if (!empty($mollom_form['mode']) && !empty($mollom_form['elements'])) {
    $mollom_form['enabled_fields'] = array_keys($mollom_form['elements']);
  }
  else {
    $mollom_form['enabled_fields'] = array();
  }
  $form_state['storage']['mollom_form'] = $mollom_form;

  $form_state['storage']['step'] = 'configure';
  $form_state['rebuild'] = TRUE;
}

/**
 * Form submit handler for Mollom form configuration form.
 */
function mollom_admin_configure_form_submit($form, &$form_state) {
  $mollom_form = $form_state['values']['mollom'];
  // Merge in form information from $form_state.
  $mollom_form += $form_state['storage']['mollom_form'];
  // Update form information in $form_state for potential rebuilds.
  $form_state['storage']['mollom_form'] = $mollom_form;

  // Prepare selected fields for storage.
  $enabled_fields = array();
  foreach (array_keys(array_filter($mollom_form['enabled_fields'])) as $field) {
    $enabled_fields[] = rawurldecode($field);
  }
  $mollom_form['enabled_fields'] = $enabled_fields;

  $status = mollom_form_save($mollom_form);
  if ($status === SAVED_NEW) {
    drupal_set_message('The form protection has been added.');
  }
  else {
    drupal_set_message('The form protection has been updated.');
  }

  $form_state['redirect'] = 'admin/config/content/mollom';
}

/**
 * Form builder; Remove Mollom protection from a form.
 */
function mollom_admin_unprotect_form($form, &$form_state, $mollom_form) {
  $form['#tree'] = TRUE;
  $form['form'] = array(
    '#type' => 'item',
    '#title' => t('Form'),
    '#markup' => $mollom_form['title'],
  );
  $form['mollom']['form_id'] = array(
    '#type' => 'value',
    '#value' => $mollom_form['form_id'],
  );

  return confirm_form($form,
    t('Are you sure you want to unprotect this form?'),
    'admin/config/content/mollom',
    t('Mollom will no longer protect this form from spam.')
  );
}

/**
 * Form submit handler for mollom_admin_unprotect_form().
 */
function mollom_admin_unprotect_form_submit($form, &$form_state) {
  db_delete('mollom_form')->condition('form_id', $form_state['values']['mollom']['form_id'])->execute();

  $form_state['redirect'] = 'admin/config/content/mollom';
}

/**
 * Menu callback; The blacklist administration page.
 */
function mollom_admin_blacklist() {
  drupal_set_title(t('Mollom blacklist'));
  return drupal_get_form('mollom_admin_blacklist_form');
}

/**
 * Form builder; Declares the text blacklist form.
 */
function mollom_admin_blacklist_form($form, &$form_state) {
  $form['#tree'] = TRUE;

  // Select options and translation of internal values for rendering.
  $contexts = array(
    'everything' => t('All fields'),
    'links' => t('Links'),
    'author' => t('Author'),
  );
  $reasons = array(
    'spam' => t('Spam'),
    'profanity' => t('Profanity'),
    'unwanted' => t('Unwanted'),
  );

  $form['blacklist'] = array();
  // Do not retrieve the current blacklist when submitting the form.
  $blacklist = (empty($form_state['input']) ? mollom('mollom.listBlacklistText') : array());
  if (is_array($blacklist)) {
    foreach ($blacklist as $id => $entry) {
      $row = array(
        'context' => array('#markup' => check_plain($contexts[$entry['context']])),
        'text' => array('#markup' => check_plain($entry['text'])),
        'reason' => array('#markup' => check_plain($reasons[$entry['reason']])),
        'text' => array('#markup' => check_plain($entry['text'])),
      );
      $row['actions']['delete'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => 'admin/config/content/mollom/blacklist/delete/' .  base64_encode($entry['text']),
      );
      $form['blacklist'][$id] = $row;
    }
  }

  $form['entry']['context'] = array(
    '#type' => 'select',
    '#options' => $contexts,
    '#default_value' => 'everything',
    '#required' => TRUE,
  );
  $form['entry']['text'] = array(
    '#type' => 'textfield',
    '#size' => 40,
    '#required' => TRUE,
    '#maxlength' => 64,
  );
  $form['entry']['reason'] = array(
    '#type' => 'select',
    '#options' => $reasons,
    '#default_value' => 'spam',
    '#required' => TRUE,
  );
  $form['entry']['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#tree' => FALSE,
  );
  $form['entry']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * Form submit handler to save a text string to the Mollom blacklist.
 */
function mollom_admin_blacklist_form_submit($form, &$form_state) {
  $result = mollom('mollom.addBlacklistText', $form_state['values']['entry']);

  if ($result === TRUE) {
    drupal_set_message(t('The entry was added to the blacklist.'));
  }
  else {
    drupal_set_message(t('An error occurred upon trying to add the text to the blacklist.'), 'error');
  }
}

/**
 * Formats the blacklist form as table to embed the form.
 */
function theme_mollom_admin_blacklist_form($variables) {
  $form = $variables['form'];
  $header = array(
    t('Context'),
    t('Text'),
    t('Reason'),
    '',
  );
  $rows = array();

  foreach (element_children($form['blacklist']) as $id) {
    $rows[] = array(
      drupal_render($form['blacklist'][$id]['context']),
      drupal_render($form['blacklist'][$id]['text']),
      drupal_render($form['blacklist'][$id]['reason']),
      drupal_render($form['blacklist'][$id]['actions']),
    );
  }

  $rows[] = array(
    drupal_render($form['entry']['context']),
    drupal_render($form['entry']['text']),
    drupal_render($form['entry']['reason']),
    drupal_render($form['entry']['actions']),
  );

  // This table is never empty due to the form.
  $output  = theme('table', array('header' => $header, 'rows' => $rows));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Form builder; Builds the confirmation form for deleting a blacklist item.
 *
 * @ingroup forms
 * @see mollom_admin_blacklist_delete_submit()
 */
function mollom_admin_blacklist_delete($form, &$form_state, $key) {
  $form['#mollom-blacklist-text'] = base64_decode($key);

  return confirm_form(
    $form,
    t('Are you sure you want to delete %text from the blacklist?', array('%text' => $form['#mollom-blacklist-text'])),
    'admin/config/content/mollom/blacklist',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
}

/**
 * Form submit handler to delete an entry from the blacklist.
 */
function mollom_admin_blacklist_delete_submit($form, &$form_state) {
  $result = mollom('mollom.removeBlacklistText', array('text' => $form['#mollom-blacklist-text']));

  if ($result === TRUE) {
    drupal_set_message(t('The entry was removed from the blacklist.'));
  }
  else {
    drupal_set_message(t('An error occurred upon trying to remove the item from the blacklist.'), 'error');
  }

  $form_state['redirect'] = 'admin/config/content/mollom/blacklist';
}

/**
 * Form builder; Global Mollom settings form.
 */
function mollom_admin_settings($form, &$form_state) {
  // When a user visits the Mollom administration page, automatically verify the
  // keys and output any error messages.
  if (empty($form_state['input'])) {
    $status = _mollom_status();
    if ($status === TRUE) {
      // Output a positive status message, since users keep on asking whether
      // Mollom should work or not.
      drupal_set_message(t('We contacted the Mollom servers to verify your keys: the Mollom services are operating correctly. We are now blocking spam.'));
    }
  }

  $form['server'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fallback strategy'),
    '#description' => t('When the Mollom servers are down or otherwise unreachable, no text analysis is performed and no CAPTCHAs are generated. If this occurs, your site will use the configured fallback strategy. Subscribers to <a href="@pricing-url">Mollom Plus</a> receive access to <a href="@sla-url">Mollom\'s high-availability backend infrastructure</a>, not available to free users, reducing potential downtime.', array(
      '@pricing-url' => 'http://mollom.com/pricing',
      '@sla-url' => 'http://mollom.com/standard-service-level-agreement',
    )),
  );
  $form['server']['mollom_fallback'] = array(
    '#type' => 'radios',
    // Default to treating everything as inappropriate.
    '#default_value' => variable_get('mollom_fallback', MOLLOM_FALLBACK_BLOCK),
    '#options' => array(
      MOLLOM_FALLBACK_BLOCK => t('Block all submissions of protected forms until the server problems are resolved'),
      MOLLOM_FALLBACK_ACCEPT => t('Leave all forms unprotected and accept all submissions'),
    ),
  );

  $form['access-keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mollom access keys'),
    '#description' => t('To use Mollom, you need a public and private key. To obtain your keys, <a href="@mollom-login-url">register and login on mollom.com</a>, and <a href="@mollom-manager-add-url">create a subscription</a> for your site. Once you created a subscription, copy your private and public access keys from the <a href="@mollom-manager-url">site manager</a> into the form fields below, and you are ready to go.', array(
      '@mollom-login-url' => 'http://mollom.com/user',
      '@mollom-manager-add-url' => 'http://mollom.com/site-manager/add',
      '@mollom-manager-url' => 'http://mollom.com/site-manager',
    )),
  );
  $form['access-keys']['mollom_public_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Public key'),
    '#default_value' => variable_get('mollom_public_key', ''),
    '#description' => t('The public key is used to uniquely identify you.'),
  );
  $form['access-keys']['mollom_private_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Private key'),
    '#default_value' => variable_get('mollom_private_key', ''),
    '#description' => t('The private key is used to prevent someone from hijacking your requests. Similar to a password, it should never be shared with anyone.'),
  );

  $form['mollom_privacy_link'] = array(
    '#type' => 'checkbox',
    '#title' => t("Link to Mollom's privacy policy on forms protected by textual analysis"),
    '#return_value' => 1,
    '#default_value' => variable_get('mollom_privacy_link', 1),
    '#description' => t('Displays a link to the recommended <a href="@privacy-policy-url">privacy policy on mollom.com</a> on all forms that are protected via <a href="@help-url">textual analysis</a>. When disabling this option, you are required to inform visitors about data privacy through other means, as stated in the <a href="@terms-of-service-url">terms of service</a> applying to your subscription.', array(
      '@privacy-policy-url' => 'http://mollom.com/web-service-privacy-policy',
      '@help-url' => url('admin/help/mollom'),
      '@terms-of-service-url' => 'http://mollom.com/terms-of-service')),
  );

  $form['mollom_developer_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t("Activate developer mode"),
    '#return_value' => 1,
    '#default_value' => variable_get('mollom_developer_mode', 0),
    '#description' => t("Allows Mollom to be tested. When developer mode is activated, you can type 'ham', 'unsure' or 'spam' in the post body of a Mollom protected form to trigger the corresponding Mollom behavior. When enabling this option, Mollom stops working correctly so it should never be used in production environments."),
  );

  return system_settings_form($form);
}

/**
 * Menu callback; Displays the administrative reports page.
 */
function mollom_reports_page($form, &$form_state) {
  $embed_attributes = array(
    'src' => 'http://mollom.com/statistics.swf?key=' . check_plain(variable_get('mollom_public_key', '')),
    'quality' => 'high',
    'width' => '100%',
    'height' => '430',
    'name' => 'Mollom',
    'align' => 'middle',
    'play' => 'true',
    'loop' => 'false',
    'allowScriptAccess' => 'sameDomain',
    'type' => 'application/x-shockwave-flash',
    'pluginspage' => 'http://www.adobe.com/go/getflashplayer',
  );
  $form['chart'] = array(
    '#type' => 'item',
    '#title' => t('Statistics'),
    '#markup' => '<embed' . drupal_attributes($embed_attributes) . '></embed>',
  );

  return $form;
}
