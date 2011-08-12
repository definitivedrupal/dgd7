<?php
/**
 * Memcache cache support.
 *
 * We just piggy-back on the memcache module, and load it via $conf['cache_inc'].
 */
require_once variable_get('cache_inc', FALSE);

/**
 * Get a cached user object.
 */
function chatroom_get_cached_user() {
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_user_' . $_COOKIE[session_name()];
  return cache_get($cache_id);
}

/**
 * Get cached messages for the given chat newer than $client_latest_message_id.
 */
function chatroom_get_cached_messages($chat_id) {
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_' . $chat_id;
  $cache = cache_get($cache_id);
  return $cache ? $cache : array();
}

/**
 * Save cached messages for the given chat.
 */
function chatroom_save_cached_messages($chat_id, $cached_messages) {
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_' . $chat_id;
  return cache_set($cache_id, $cached_messages, variable_get('chatroom_cache_ttl', 3600));
}

/**
 * Save a user object to the cache.
 */
function chatroom_save_cached_user($cached_user) {
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_user_' . $_COOKIE[session_name()];
  return cache_set($cache_id, $cached_user, variable_get('chatroom_cache_ttl', 3600));
}

/**
 * Delete the user stored under the given $session_id.
 */
function chatroom_delete_cached_user($session_id) {
  $cache_id = variable_get('chatroom_cache_prefix', 'chatroom_cache_prefix') . '_user_' . $session_id;
  return cache_clear_all($cache_id, 'cache');
}

