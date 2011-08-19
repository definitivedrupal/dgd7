<?php
/**
 * Whether or not we have APC functionality.
 */
define('CHATROOM_APC_ENABLED', function_exists('apc_fetch'));

/**
 * Get a cached user object.
 */
function chatroom_get_cached_user() {
  if (!CHATROOM_APC_ENABLED) {
    return FALSE;
  }
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_user_' . $_COOKIE[session_name()];
  return apc_fetch($cache_id);
}

/**
 * Get cached messages for the given chat newer than $client_latest_message_id.
 */
function chatroom_get_cached_messages($chat_id) {
  if (!CHATROOM_APC_ENABLED) {
    return FALSE;
  }
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_' . $chat_id;
  $cache = apc_fetch($cache_id);
  return $cache ? unserialize($cache) : array();
}

/**
 * Save cached messages for the given chat.
 */
function chatroom_save_cached_messages($chat_id, $cached_messages) {
  if (!CHATROOM_APC_ENABLED) {
    return FALSE;
  }
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_' . $chat_id;
  return apc_store($cache_id, serialize($cached_messages), variable_get('chatroom_cache_ttl', 3600));
}

/**
 * Save a user object to the cache.
 */
function chatroom_save_cached_user($cached_user) {
  if (!CHATROOM_APC_ENABLED) {
    return FALSE;
  }
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_user_' . $_COOKIE[session_name()];
  return apc_store($cache_id, $cached_user, variable_get('chatroom_cache_ttl', 3600));
}

/**
 * Delete the user stored under the given $session_id.
 */
function chatroom_delete_cached_user($session_id) {
  if (!CHATROOM_APC_ENABLED) {
    return FALSE;
  }
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_user_' . $session_id;
  return apc_delete($cache_id);
}

