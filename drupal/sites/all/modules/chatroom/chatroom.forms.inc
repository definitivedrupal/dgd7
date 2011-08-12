<?php

/**
 * @file
 * Form functions for chatroom.module.
 */

/**
 * Implements hook_forms().
 */
function chatroom_forms($form_id, $args) {
  // Look for the admin buttons form.
  if (strpos($form_id, 'chatroom_chat_admin_buttons_form_') === 0) {
    // Route to the right function.
    $forms[$form_id] = array(
      'callback' => 'chatroom_chat_admin_buttons_form',
    );
  }
  $forms['chatroom_chat_form'] = array(
    'callback' => 'chatroom_chat_interface',
  );
  $forms['chatroom_chat_management_form'] = array(
    'callback' => 'chatroom_chat_management_form',
  );
  $forms['chatroom_create_chat_form'] = array(
    'callback' => 'chatroom_create_chat_form',
  );
  $forms['chatroom_archive_chat_form'] = array(
    'callback' => 'chatroom_archive_chat_form',
  );
  $forms['chatroom_unarchive_chat_form'] = array(
    'callback' => 'chatroom_unarchive_chat_form',
  );
  return $forms;
}

/**
 * Implements hook_form().
 */
function chatroom_form(&$node, $form_state) {
  global $user;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => check_plain($node->title),
    '#required' => TRUE,
  );
  $body_element = array(
    '#type' => 'textarea',
    '#base_type' => 'textarea',
    '#parents' => array('body', 'body_filter'),
    '#title' => t('Description'),
    '#default_value' => (isset($node->body)) ? $node->body : '',
    '#rows' => 3,
    '#description' => t('Describe your chat room so other people will know if they want to join.'),
  );
  $body_element = filter_process_format($body_element);
  $form['body_filter']['body'] = $body_element['value'];
  $form['body_filter']['format'] = $body_element['format'];
  $form['kicked_out_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Chat room kicked out message'),
    '#default_value' => isset($node->chatroom->kicked_out_message) ? $node->chatroom->kicked_out_message : null,
    '#rows' => 3,
    '#description' => t('This text will appear on the page kicked out users are sent to. Defaults to, "You have been kicked out of %chat for misbehaving."', array('%chat' => t('chat-name'))),
  );
  $form['banned_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Chat room banned message'),
    '#default_value' => isset($node->chatroom->banned_message) ? $node->chatroom->banned_message : null,
    '#rows' => 3,
    '#description' => t('This text will appear on the page banned users are sent to. Defaults to, "You have been banned from %chatroom."', array('%chatroom' => t('chat-room'))),
  );
  if (!empty($node->chatroom->banned_users)) {
    $form['chatroom_banned_users'] = array(
      '#type' => 'fieldset',
      '#title' => t('Manage banned users'),
      '#collapsible' => TRUE,
    );
    foreach ($node->chatroom->banned_users as $banned_user) {
      $banned_users[$banned_user->uid] = check_plain($banned_user->name);
    }
    $form['chatroom_banned_users']['unban_list'] = array(
      '#type' => 'checkboxes',
      '#options' => $banned_users,
      '#description' => t('Check the users you would like to unban'),
    );
  }
  $form['chat_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chat settings'),
    '#collapsible' => TRUE,
  );
  $form['chat_settings']['poll_freq'] = array(
    '#type' => 'select',
    '#title' => t('Update frequency'),
    '#default_value' => empty($node->chatroom->poll_freq) ? 1 : $node->chatroom->poll_freq,
    '#options' => drupal_map_assoc(range(1, 10)),
    '#description' => t('How many seconds between each request for updates from the server.'),
  );
  $form['chat_settings']['idle_freq'] = array(
    '#type' => 'select',
    '#title' => t('Idle time'),
    '#default_value' => empty($node->chatroom->idle_freq) ? 60 : $node->chatroom->idle_freq,
    '#options' => drupal_map_assoc(array(20, 40, 60, 80, 100, 120, 140, 160, 180)),
    '#description' => t('How many seconds between each message before a last message time is shown in the chat.'),
  );
  $form['chat_settings']['max_users'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum users'),
    '#default_value' => isset($node->chatroom) ? $node->chatroom->max_users : 0,
    '#description' => t('The default maximum number of users for this chatroom. 0 is unlimited'),
    '#size' => 5,
  );
  $old_msg_range = array();
  for ($i = 1; $i <= 25; $i++) {
    $old_msg_range[$i] = $i * 10;
  }
  $form['chat_settings']['previous_messages_display_count'] = array(
    '#type' => 'select',
    '#title' => t('Old messages'),
    '#description' => t('How many old messages to show when entering a chat.'),
    '#default_value' => empty($node->chatroom->previous_messages_display_count) ? 20 : $node->chatroom->previous_messages_display_count,
    '#options' => drupal_map_assoc($old_msg_range),
  );
  if (variable_get('user_pictures', FALSE)) {
    $form['chatroom_profile_picture_support'] = array(
      '#type' => 'fieldset',
      '#title' => t('User profile picture support'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['chatroom_profile_picture_support']['profile_picture'] = array(
      '#type' => 'checkbox',
      '#title' => t('User profile picture.'),
      '#description' => t('Enable User profile picture for chatroom.'),
      '#default_value' => isset($node->chatroom) ? $node->chatroom->profile_picture : '',
    );
    if (module_exists('imagecache')) {
      $presets = array('');
      foreach (imagecache_presets() as $preset) {
        $presets[$preset['presetname']] = check_plain($preset['presetname']);
      }
      $form['chatroom_profile_picture_support']['imagecache_preset'] = array(
        '#type' => 'select',
        '#title' => t('Profile picture preset'),
        '#default_value' => $node->chatroom->imagecache_preset,
        '#options' => $presets,
        '#description' => t("This will set the picture size."),
      );
    }
  }
  return $form;
}

/**
 * Menu callback; display site-wide chat room settings.
 */
function chatroom_admin_settings($form, &$form_state) {
  $form['chatroom_auto_archive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically archive old messages.'),
    '#description' => t('If there are a lot of old messages, archiving will improve chat performance.'),
    '#default_value' => variable_get('chatroom_auto_archive', FALSE),
  );
  $form['chatroom_block_update_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Chat room block update interval'),
    '#default_value' => variable_get('chatroom_block_update_interval', 5),
    '#description' => t('Determines how often blocks should update active chat rooms, active chats, and on-line users.'),
    '#size' => 2,
  );
  $form['chatroom_online_list_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Chat room user online timeout length'),
    '#default_value' => variable_get('chatroom_online_list_timeout', 10),
    '#description' => t('In seconds, sets length of time since use was last seen in chat before being removed from chat user list'),
    '#size' => 3,
  );
  $form['chatroom_guest_user_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Guest user prefix'),
    '#description' => t('Prefixed to guest ID to provide user name for anonymous users.'),
    '#default_value' => variable_get('chatroom_guest_user_prefix', t('guest-')),
    '#size' => 20,
  );
  $form['chatroom_chat_message_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Chat message help text'),
    '#description' => t('The text to be displayed next to the chat message form field.'),
    '#default_value' => variable_get('chatroom_chat_message_text', t('Type your message and hit enter')),
    '#size' => 20,
  );
  $form['chatroom_chat_date_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Chat date format'),
    '#attributes' => array('class' => array('custom-format')),
    '#default_value' => variable_get('chatroom_chat_date_format', '* \S\e\n\t \a\t G:i'),
    '#description' => t('Format for system time messages in chats. See the <a href="@url">PHP manual</a> for available options. This format is currently set to display as <span>%date</span>.', array('@url' => 'http://php.net/manual/function.date.php', '%date' => format_date(REQUEST_TIME, 'custom', variable_get('chatroom_chat_date_format', '* \S\e\n\t \a\t G:i')))),
  );
  $form['chatroom_allow_anon_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide a name field for anonymous users?'),
    '#description' => t('Like other chat type services, this adds a name textfield for anonymous users.'),
    '#default_value' => variable_get('chatroom_allow_anon_name', TRUE),
  );
  $form['chatroom_backend'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chatroom backend'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $options = array('0' => t('AJAX'), '1' => t('Node.js'));
  $form['chatroom_backend']['chatroom_custom_polling_backend'] = array(
    '#type' => 'radios',
    '#title' => t('Chatroom backend options'),
    '#default_value' =>  (variable_get('chatroom_custom_polling_backend', FALSE))?1:0,
    '#options' => $options,
    '#description' => t('The log.'),
    '#disabled' => !module_exists('chatroom_nodejs'),
  );  
  if (module_exists('smileys')) {
    $form['chatroom_smileys_support'] = array(
      '#type' => 'fieldset',
      '#title' => t('Smileys module support'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['chatroom_smileys_support']['chatroom_smileys_enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Smileys module support.'),
      '#default_value' => variable_get('chatroom_smileys_enabled', FALSE),
    );
    $form['chatroom_smileys_support']['chatroom_smileys_showtextentry'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show smileys in text entry box.'),
      '#default_value' => variable_get('chatroom_smileys_enabled', FALSE) && variable_get('chatroom_smileys_showtextentry', FALSE),
      '#disabled' => !variable_get('chatroom_smileys_enabled', FALSE),
    );
  }
  $form['chatroom_alerts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chat alerts'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['chatroom_alerts']['chatroom_alerts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable chat alerts.'),
    '#description' => t('Checking this box will allow users to turn on alerts for chat events.'),
    '#default_value' => variable_get('chatroom_alerts', FALSE),
  );
  $form['chatroom_alerts']['chatroom_alerts_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Turn alerts on by default.'),
    '#description' => t('Check this box if you want chats to open with alerts on.'),
    '#default_value' => variable_get('chatroom_alerts', FALSE) && variable_get('chatroom_alerts_default', FALSE),
    '#disabled' => !variable_get('chatroom_alerts', FALSE),
  );
  $form['chatroom_alerts']['chatroom_custom_sounds'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use custom sounds for chat alerts.'),
    '#description' => t('Check this box if you want to replace default chat alert sounds with your own MP3s.'),
    '#default_value' => variable_get('chatroom_alerts', FALSE) && variable_get('chatroom_custom_sounds', FALSE),
    '#disabled' => !variable_get('chatroom_alerts', FALSE),
  );

  $form['chatroom_desk']  = array(
    '#type' => 'fieldset',
    '#title' => t('Chatroom "desk" options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['chatroom_desk']['chatroom_desk_nid'] = array(
    '#type' => 'textfield',
    '#title' => t('Chatroom node ID to use for desk mode.'),
    '#description' => t('Desk mode creates private chats inside a chat room. Leave blank to disable.'),
    '#default_value' => variable_get('chatroom_desk_nid', ''),
  );

  $roles = user_roles();
  foreach ($roles as $rid => $role) {
    $options[$rid] = check_plain($role);
  }

  $form['chatroom_desk']['chatroom_desk_staff_rids'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Staff roles'),
    '#description' => t('Mark which roles are considered to be "staff" for your desk.'),
    '#options' => $options,
    '#default_value' => variable_get('chatroom_desk_staff_rids', array()),
  );

  $form['chatroom_desk']['chatroom_desk_seconds_online'] = array(
    '#type' => 'textfield',
    '#title' => t('Auto-close desk when staff are inactive'),
    '#default_value' => variable_get('chatroom_desk_minutes_online', 15),
    '#description' => t('Desk will change to a closed state after the staff member has been idle for the minutes indicated.'),
  );

  $form['chatroom_desk']['chatroom_desk_open_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Desk open message'),
    '#default_value' => variable_get('chatroom_desk_open_message', ''),
    '#description' => t('A message to show when staff are active on the site.'),
  );

  $form['chatroom_desk']['chatroom_desk_closed_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Desk closed message'),
    '#default_value' => variable_get('chatroom_desk_closed_message', 'Our help desk is closed, please try again later.'),
    '#description' => t('A message to show when staff are not active on the site.'),
  );

  return system_settings_form($form);
}

/**
 * Validate site-wide chat room settings.
 */
function chatroom_admin_settings_validate($form, &$form_state) {
  // Check for a valid update interval.
  $interval = $form_state['values']['chatroom_block_update_interval'];
  if (is_numeric($interval)) {
    if ($interval == 0) {
      form_set_error('chatroom_block_update_interval', t('The block update interval cannot be zero.'));
    }
    elseif ($interval < 0) {
      form_set_error('chatroom_block_update_interval', t('The block update interval cannot be negative.'));
    }
  }
  else {
    form_set_error('chatroom_block_update_interval', t('The block update interval must be a number.'));
  }
  $timeout = $form_state['values']['chatroom_online_list_timeout'];
  if (!is_numeric($timeout) || $timeout < 10 || $timeout > 999) {
    form_set_error('chatroom_online_list_timeout', t('The online timeout length must be a a valid number from 10 to 999.'));
  }
  // Validate that the entered desk node is indeed a chatroom.
  if ($form_state['values']['chatroom_desk_nid']!="") {
    $chatroom_desk_node = node_load($form_state['values']['chatroom_desk_nid']);
    if (empty($chatroom_desk_node->type) || $chatroom_desk_node->type != 'chatroom') {
      form_set_error('chatroom_desk_nid', t('The chatroom desk node id must be for a chatroom'));
    }
  }
}

/**
 * Implements hook_form().
 */
function chatroom_chat_form(&$node, $form_state) {
  global $user;

  // Load type definition.
  $type = node_type_get_type($node);

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => check_plain($node->title),
    '#description' => t('The name for your chat.'),
    '#required' => TRUE,
  );
  $form['chatroom_nid'] = array(
    '#type' => 'hidden',
    '#value' => '',
    '#value' => isset($node->chat) && $node->chat->chatroom ? $node->chat->chatroom->nid : 0,
  );
  $form['kicked_out_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Chat kicked out message'),
    '#default_value' => isset($node->chat) ? $node->chat->kicked_out_message : '',
    '#rows' => 3,
    '#description' => t('This text will appear on the page kicked out users are sent to. Defaults to, "You have been kicked out of %chat for misbehaving."', array('%chat' => t('chat-name'))),
  );
  $form['banned_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Chat room banned message'),
    '#default_value' => isset($node->chat) ? $node->chat->banned_message : '',
    '#rows' => 3,
    '#description' => t('This text will appear on the page banned users are sent to. Defaults to, "You have been banned from %chatroom."', array('%chatroom' => t('chat-room'))),
  );
  if (!empty($node->chat->banned_list)) {
    $form['chatroom_banned_users'] = array(
      '#type' => 'fieldset',
      '#title' => t('Manage banned users'),
      '#collapsible' => TRUE,
    );
    foreach ($node->chat->banned_list as $banned_user) {
      $banned_users[$banned_user->uid] = check_plain(user_load($banned_user->uid)->name);
    }
    $form['chatroom_banned_users']['unban_list'] = array(
      '#type' => 'checkboxes',
      '#options' => $banned_users,
      '#description' => t('Check the users you would like to unban'),
    );
  }
  $form['chat_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chat settings'),
    '#collapsible' => TRUE,
  );
  $form['chat_settings']['poll_freq'] = array(
    '#type' => 'select',
    '#title' => t('Update frequency'),
    '#default_value' => isset($node->chat) ? $node->chat->poll_freq : 1,
    '#options' => drupal_map_assoc(range(1, 10)),
    '#description' => t('How many seconds between each request for updates from the server.'),
  );
  $form['chat_settings']['private'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make the chat private?'),
    '#default_value' => isset($node->chat) ? $node->chat->private : 0,
    '#description' => t('Do you want this chat to be private?'),
  );
  $form['chat_settings']['idle_freq'] = array(
    '#type' => 'select',
    '#title' => t('Idle time'),
    '#default_value' => isset($node->chat) ? $node->chat->idle_freq : 60,
    '#options' => drupal_map_assoc(array(20, 40, 60, 80, 100, 120, 140, 160, 180)),
    '#description' => t('How many seconds between each message before a last message time is shown in the chat.'),
  );
  $form['chat_settings']['max_users'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum users'),
    '#default_value' => isset($node->chat) ? $node->chat->max_users : 0,
    '#description' => t('The maximum number of users for this chat. 0 is unlimited'),
    '#size' => 5,
  );
  $previous_msg_range = array();
  for ($i = 1; $i <= 25; $i++) {
    $previous_msg_range[$i] = $i * 10;
  }
  $form['chat_settings']['previous_messages_display_count'] = array(
    '#type' => 'select',
    '#title' => t('Old messages'),
    '#description' => t('How many old messages to show when entering a chat.'),
    '#default_value' => isset($node->chat) ? $node->chat->previous_messages_display_count : 20,
    '#options' => drupal_map_assoc($previous_msg_range),
  );
  return $form;
}

/**
 * Return a create chat form.
 */
function chatroom_create_chat_form($form, &$form_state, $room) {
  $form_state['chatroom'] = $room;
  $form['chatroom_create_chat'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create a new chat'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['chatroom_create_chat']['chat_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Chat name'),
    '#size' => 30,
    '#required' => TRUE,
    '#description' => t('Enter the name for the chat'),
  );
  $form['chatroom_create_chat']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create chat'),
  );
  $form['#validate'][] = 'chatroom_create_chat_form_validate';
  $form['#submit'][] = 'chatroom_create_chat_form_submit';
  return $form;
}

/**
 * Create a chat from the chatroom list page.
 */
function chatroom_create_chat_form_submit($form, &$form_state) {
  global $user;

  $chatroom = $form_state['chatroom'];
  $values = (object) $form_state['values'];
  return chatroom_create_chatroom_chat($chatroom, $values, $user);
}

/**
 * Create a chat that belongs in a chatroom.
 *
 * @param mixed $chatroom
 * @param mixed $values
 * @access public
 * @return StdClass
 */
function chatroom_create_chatroom_chat($chatroom, $values, $chat_user) {
  $node = new StdClass();
  $node->type = 'chat';
  $node->title = $values->chat_name;
  $node->uid = $chat_user->uid;
  $node->created = isset($values->created) ? $values->created : REQUEST_TIME;
  $node->chatroom_nid = $chatroom->nid;
  $node->idle_freq = $chatroom->chatroom->idle_freq;
  $node->poll_freq = $chatroom->chatroom->poll_freq;
  $node->kicked_out_message = $chatroom->chatroom->kicked_out_message;
  $node->banned_message = $chatroom->chatroom->banned_message;
  $node->max_users = $chatroom->chatroom->max_users;
  $node->previous_messages_display_count = $chatroom->chatroom->previous_messages_display_count;
  $node->when_archived = isset($data->when_archived) ? $data->when_archived : NULL;
  $node->comment = isset($data->comment) ? $data->comment : COMMENT_NODE_DISABLED;
  $node->promote = isset($data->promote) ? $data->promote : 0;
  $node->sticky = isset($data->sticky) ? $data->sticky : 0;
  node_save($node);
  return $node;
}

/**
 * Form to unarchived a chat.
 *
 * @param array $form_state
 * @param integer $node
 * @return array
 */
function chatroom_unarchive_chat_form($form, &$form_state, $node) {
  $form_state['chat'] = $node;
  $form['unarchive'] = array(
    '#type' => 'submit',
    '#value' => t('Unarchive this chat'),
  );
  return $form;
}

/**
 * Unarchive an open chat.
 *
 * @param array $form
 * @param array $form_state
 * @return string
 */
function chatroom_unarchive_chat_form_submit($form, &$form_state) {
  chatroom_chat_unarchive_chat($form_state['chat']);
  cache_clear_all(TRUE, 'cache_block');
}

/**
 * Archive an open chat.
 *
 * @param array $form_id
 * @param array $form_values
 * @return string
 */
function chatroom_chat_management_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  if ($op == t('Make chat private')) {
    chatroom_chat_make_chat_private($form_state['chat']);
  }
  else if ($op == t('Make chat public')) {
    chatroom_chat_make_chat_public($form_state['chat']);
  }
  else if ($op == t('Archive this chat')) {
    chatroom_chat_archive_chat($form_state['chat']);
  }
  cache_clear_all(TRUE, 'cache_block');
}

/**
 * Return a widget to manage a chat - make it private, send invites, archive it.
 *
 * @param mixed $form_state
 * @param mixed $node
 * @return array
 */
function chatroom_chat_management_form($form, &$form_state, $node) {
  $allowed_users_form = array();
  $settings = array(
    'userAddPath' => 'chatroom/ajax/user/add',
    'userInvitePath' => 'chatroom/ajax/user/invite',
    'userRemovePath' => 'chatroom/ajax/user/remove',
    'allowedUsers' => array(),
  );
  drupal_add_js(drupal_get_path('module', 'chatroom') . '/chatroom.user.add.widget.js');
  foreach (chatroom_chat_get_allowed_users($node) as $allowed_user) {
    $settings['allowedUsers'][] = $allowed_user;
  }
  drupal_add_js(array('chatroomChatForm' => $settings), array('type' => 'setting', 'scope' => JS_DEFAULT));

  $form_state['chat'] = $node;

  $form['chat_manager'] = array(
    '#type' => 'fieldset',
    '#title' => t('Manage this chat'),
    '#collapsible' => TRUE,
    '#collapsed' => !(isset($node->chat) && $node->chat->private),
  );
  if ($node->chat->private) {
    $form['chat_manager']['add_user'] = array(
      '#type' => 'textfield',
      '#title' => t('Add a user to this chat'),
      '#autocomplete_path' => "chatroom/chat/user/autocomplete/$node->nid",
      '#size' => 10,
      '#description' => t('Type in a username, then hit enter.'),
    );
    $form['chat_manager']['make_public'] = array(
      '#type' => 'submit',
      '#value' => t('Make chat public'),
    );
  }
  else {
    $form['chat_manager']['make_private'] = array(
      '#type' => 'submit',
      '#value' => t('Make chat private'),
    );
  }
  $form['chat_manager']['invite_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Invite a user to this chat'),
    '#autocomplete_path' => "chatroom/chat/user/autocomplete/$node->nid",
    '#size' => 10,
    '#size' => 10,
    '#description' => t('Type in a username, then hit enter.'),
  );
  $form['chat_manager']['archive'] = array(
    '#type' => 'submit',
    '#value' => t('Archive this chat'),
  );
  return $form;
}

/**
 * Return a form to manage this chat from the chatroom view.
 *
 * @param mixed $form_state
 * @param mixed $chat
 * @return array
 */
function chatroom_chat_admin_buttons_form(&$form_state, $chat) {
  $form_state['chat'] = $chat;
  if ($chat->when_archived) {
    $form = array(
      'unarchive' => array(
        '#type' => 'submit',
        '#value' => t('Unarchive'),
      ),
    );
  }
  else {
    $form = array(
      'archive' => array(
        '#type' => 'submit',
        '#value' => t('Archive'),
      ),
    );
  }
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['#submit'] = array(
    'chatroom_chat_admin_buttons_form_submit',
  );
  return $form;
}

/**
 * Handle submission of chatroom chat admin button.
 *
 * @param mixed $form
 * @param mixed $form_state
 * @return void
 */
function chatroom_chat_admin_buttons_form_submit($form, &$form_state) {
  $chat = $form_state['chat'];
  switch ($form_state['values']['op']) {
    case t('Archive'):
      if (chatroom_chat_archive_chat($chat)) {
        drupal_set_message(t("Chat '%chat' archived.", array('%chat' => $chat->title)));
      }
      else {
        drupal_set_message(t("There as an error archiving chat '%chat'.", array('%chat' => $chat->title)));
      }
      break;

    case t('Unarchive'):
      if (chatroom_chat_unarchive_chat($chat)) {
        drupal_set_message(t("Chat '%chat' unarchived.", array('%chat' => $chat->title)));
      }
      else {
        drupal_set_message(t("There as an error unarchiving chat '%chat'.", array('%chat' => $chat->title)));
      }
      break;

    case t('Delete'):
      // Yeah, who needs a return value anyway? Oh, and node_delete does a
      // drupal_set_message() for us.
      node_delete($chat->nid);
      break;
  }
}


/**
 * Return a form to open a chat desk.
 * @param $form_state
 */
function chatroom_desk_open_form($form_state) {
  global $user;

  $form = array();
  if (!user_access('access chat rooms') || !user_access('use chat desk')) {
    // If this is an anonymous user, show them the feature and give them a call to action.
    if (!$user->uid) {
      $form['chatroom_chatdesk']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Chat with staff'),
        '#disabled' => TRUE,
      );

      $destination = drupal_get_destination();
      if (array_key_exists(DRUPAL_AUTHENTICATED_RID, user_roles(TRUE, 'use chat desk')) && variable_get('user_register', 1)) {
        // Users can register themselves.
        $help = t('<a href="@login">Login</a> or <a href="@register">register</a> to chat', array('@login' => url('user/login', array('query' => $destination)), '@register' => url('user/register', array('query' => $destination))));
      }
      else {
        // Only admins can add new users, no public registration.
        $help = t('<a href="@login">Login</a> to chat', array('@login' => url('user/login', array('query' => $destination))));
      }
      $form['chatroom_chatdesk']['help'] = array(
        '#value' => '<p>' . $help . '</p>',
      );
    }
  }
  else {
    $form['chatroom_chatdesk']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Chat with staff'),
    );
  }
  return $form;
}

/**
 * Handle submission of form to open a chat desk.
 *
 * @param mixed $form
 * @param mixed $form_state
 * @return void
 */
function chatroom_desk_open_form_submit($form, &$form_state) {
  global $user;

  // Load the chatroom and ensure it exists and this user can access it.
  $chatroom = node_load(variable_get('chatroom_desk_nid', ''));
  if (empty($chatroom->nid) || !node_access('view', $chatroom)) {
    drupal_set_message(t("Contact your site administrator to configure the chat desk."));
    return;
  }

  // Set a unique title and create the chatroom.
  $i = 0;
  do {
    $i++;
    $name = t('Question from @name - @number', array('@name' => $user->name, '@number' => $i));
    $nid = db_query("SELECT nid FROM {node} WHERE title = :title AND type = :type", array(':title' => $name, ':type' => 'chat'))->fetchField();
  } while (!empty($nid));

  $values['chat_name'] = $name;
  $values = (object) $values;

  $chat = chatroom_create_chatroom_chat($chatroom, $values, $user);
  db_update('chatroom_chat')
  ->fields(array(
    'private' => 1,
  ))
  ->condition('nid', $chat->nid)
  ->execute();
  cache_clear_all('chatroom_chat_list', 'cache');

  $_GET['destination'] = 'node/' . $chat->nid;

  return;
}

