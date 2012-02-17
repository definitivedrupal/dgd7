<?php

define('DRUPAL_ROOT', dirname(__FILE__));

/**
 * @file
 * Process chatroom polling requests.
 *
 * This file is a performance hack, aimed at making moderately sized rooms
 * possible with this module.
 *
 * The intent is to handle the polling requests from chatrooms without doing
 * a full Drupal bootstrap unless absolutely necessary. All simple polling
 * requests are handled by this script, as they are by far the most common
 * type of request, and are very easy to check against a cache.
 *
 * First, we check to see if the last message seen in a given chat client-side
 * is older than the latest message in that chat server-side.
 *
 * Second, we check a message cache, and if we can get some messages from it,
 * we send them, and exit. This is the real killer performance win from this
 * file for busy rooms.
 *
 * Only if the chat being polled has messages newer than what the requesting
 * client has seen do we bootstrap Drupal.
 *
 * This module doesn't use the full Form API in general because it would make
 * the chats too slow. Form token handling is implemented manually by the
 * module to deal with security issues related to handling $_POST data
 * directly.
 */

// We need the $latest_msg_id, $chat_id and $chat_cache_file to check the
// cache for this chat.
if (!isset($_POST['latest_msg_id']) || !preg_match('/^\d+$/', $_POST['latest_msg_id'])) {
  exit;
}

if (!isset($_POST['chat_id']) || !preg_match('/^\d+$/', $_POST['chat_id'])) {
  exit;
}

if (!isset($_POST['chat_cache_directory']) || !is_dir($_POST['chat_cache_directory'])) {
  exit;
}

if (!isset($_POST['skip_cache'])) {
  exit;
}

// Bootstrap configuration to make sure we get the right
// value from session_name() and some paths for caching.
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_CONFIGURATION);

// Do this after the bootstrap so we don't loose these variables.
$client_latest_msg_id = $_POST['latest_msg_id'];
$chat_id = $_POST['chat_id'];
$chat_cache_file = $_POST['chat_cache_directory'] . '/chatroom.chat.' . $chat_id . '.cache';
$skip_cache = $_POST['skip_cache'] == 1 ? TRUE : FALSE;

// We let the client signal that we should skip the cache. Right now we're
// using this to make sure users last-seen time is updated, and there may
// be more uses for it down the track.
if (!$skip_cache) {

  if (file_exists($chat_cache_file)) {
    // Do a quick DoS check - we don't validate the path, so we have to make
    // sure we're not reading arbitrarily big files into memory. Our cache file
    // should contain a single numeric id. So, if the file is bigger than 25
    // bytes, something is fishy, and we should just bail out.
    $file_stats = stat($chat_cache_file);
    if ($file_stats['size'] > 25) {
      exit;
    }

    $server_latest_msg_id = trim(file_get_contents($chat_cache_file));
    if ($server_latest_msg_id == $client_latest_msg_id) {
      print json_encode(array('data' => array('cacheHit' => 1, 'messages' => array())));
      exit;
    }
    unset($server_latest_msg_id);
  }

  $chatroom_module_dir = variable_get('chatroom_module_dir', 'sites/all/modules/chatroom');
  require_once "./$chatroom_module_dir/chatroom.cache." . variable_get('chatroom_cache_backend', 'apc') . '.inc';

  if ($chat_user = chatroom_get_cached_user()) {
    if ($chat_user->is_chat_admin || in_array($chat_id, $chat_user->allowed_chats)) {
      if ($cached_messages = chatroom_get_cached_messages($chat_id)) {
        $valid_cache_messages = array();
        foreach ($cached_messages as $message) {
          if ($message->cmid > $client_latest_msg_id) {
            if ($message->type == 'private_message' && $chat_user->uid != $message->recipient_uid) {
              continue;
            }
            $valid_cache_messages[] = chatroom_theme_cached_message($message, $chat_user);
          }
        }
        if ($valid_cache_messages) {
          print json_encode(array('data' => array('cacheHit' => 1, 'messages' => $valid_cache_messages)));
          exit;
        }
      }
    }
  }
}

// Make this look like a normal request to Drupal, then execute index.php.
$_GET['q'] = "chatroom/chat/get/latest/messages/$chat_id/$client_latest_msg_id";
require_once './index.php';

/**
 * Theme a cached message.
 *
 * We need to do this because when the message was first built and themed, it
 * could have been for a different user than the one that will pull it out of
 * the cache. So, things like timezone and the username link need to be
 * adjusted.
 *
 * @param mixed $message
 */
function chatroom_theme_cached_message($message, $chat_user) {
  // Only need to make adjustments if this message is being viewed by a
  // different user now than when it was cached.
  if ($message->viewed_uid != $chat_user->uid) {
    // We can't look up whether the current user can see user profiles, so we
    // check the value stored in the cached user object.
    $username = $chat_user->can_access_user_profiles ? $message->themed_username : $message->name;

    // We can't look up the css classes, so we use the values stored in the
    // message when it was cached.
    $class = "new-message $message->public_css_class" . ($message->type == 'private_message' ? " $message->private_css_class" : '');
    if (variable_get('chatroom_debug', FALSE)) {
      $class .= ' chatroom-message-cache-hit';
    }

    $offset_in_hours = $chat_user->chat_timezone_offset / 60 / 60;
    $offset_in_hours = $offset_in_hours >= 0 ? '+' . $offset_in_hours : $offset_in_hours;
    $date = new DateTime('@' . ($message->modified + $chat_user->chat_timezone_offset), new DateTimeZone('Etc/GMT' . $offset_in_hours));
    $message->html = '<div class="' . $class . '">';
    $message->html .= '(' . $date->format($message->date_format) . ') <strong>' . $username . ':</strong> ';
    $message->html .= $message->themed_message;
    $message->html .= "</div>";
  }
  return $message;
}

