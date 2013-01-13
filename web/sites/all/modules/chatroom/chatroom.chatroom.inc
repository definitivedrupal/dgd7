<?php

/**
 * @file
 * Chatroom class definition.
 */

/**
 * Represents a chatroom.
 */
class Chatroom {

  protected $changedAttributes = array();

  /**
   * Format to be used when filtering chat message output.
   */
  protected $format = '';

  /**
   * List of roles allowed to in this chat.
   */
  protected $allowedRids = array();

  /**
   * List of user uids allowed in this chat.
   */
  protected $allowedUids = array();

  protected $archived = 0;

  protected $cid = 0;

  protected $uid = 0;

  protected $public = FALSE;

  protected $title = '';

  protected $latestMessage = FALSE;

  protected $onlineUsers = array();

  public function __construct($cid = FALSE) {
    if ($cid) {
      $this->load($cid);
    }
  }

  public function load($cid) {
    $data = db_query('SELECT * FROM {chatroom} WHERE cid = :cid', array(':cid' => $cid))->fetchObject();
    if ($data) {
      $this->cid = $data->cid;
      $this->public = $data->public;
      $this->archived = $data->archived;
      $this->title = $data->title;
      $this->format = $data->format ? $data->format : filter_fallback_format();
      $this->loadAllowedRoles();
      $this->loadAllowedUids();
      $this->loadLatestMessage();
    }
  }

  public function getOnlineUsers() {
    $info = nodejs_get_content_channel_users('chatroom_' . $this->get('cid'));
    $users = array();
    if (!empty($info['uids'])) {
      $users = user_load_multiple($info['uids']);
    }
    return $users;
  }

  public function set($key, $value) {
    if (isset($this->$key) && $this->$key !== $value) {
      $this->$key = $value;
      if (!in_array($key, $this->changedAttributes)) {
        $this->changedAttributes[] = $key;
      }
    }
  }

  public function get($key) {
    if (isset($this->$key)) {
      return $this->$key;
    }
  }

  protected function loadLatestMessage() {
    $messages = $this->loadLatestMessages(1);
    $this->latestMessage = $messages ? current($messages) : FALSE;
    return $this->latestMessage;
  }

  protected function loadAllowedRoles() {
    $this->allowedRids = array();
    $roles = db_query('SELECT rid, read_only FROM {chatroom_role} WHERE cid = :cid', array(':cid' => $this->cid))->fetchAll();
    foreach ($roles as $role) {
      $this->allowedRids[$role->rid] = $role;
    }
    return $this->allowedRids;
  }

  protected function loadAllowedUids() {
    $this->allowedUids = array();
    $uids = db_query('SELECT uid, read_only FROM {chatroom_user} WHERE cid = :cid', array(':cid' => $this->cid))->fetchAll();
    foreach ($uids as $uid) {
      $this->allowedUids[] = $uid;
    }
    return $this->allowedUids;
  }

  /**
   * Write a message into a chat, and allow any interested modules to react.
   *
   * @param array $message
   * @return boolean
   */
  public function saveMessage($message) {
    drupal_alter('chatroom_msg', $message);
    if (drupal_write_record('chatroom_msg', $message)) {
      $this->latestMessage = (object) $message;
      foreach (module_implements('chat_msg_saved') as $module) {
        $function = $module . '_chat_msg_saved';
        $function($message, $this);
      }
      return $message['cmid'];
    }
    return FALSE;
  }

  /**
   * Check to see if the given user has access to this chat.
   *
   * @param mixed $account
   * @param mixed $access_type
   * @return boolean
   */
  public function userHasAccess($account, $access_type) {
    $admin_role_rid = variable_get('user_admin_role', FALSE);
    if ($account->uid == 1 || ($admin_role_rid && isset($account->roles[$admin_role_rid]))) {
      return TRUE;
    }

    $params = array(
      ':cid' => $this->get('cid'),
      ':uid' => $account->uid,
    );
    $sql = 'SELECT cid, read_only FROM {chatroom_user} WHERE cid = :cid AND uid = :uid';
    if ($user_access = db_query($sql, $params)->fetchObject()) {
      if (!$user_access->read_only || $access_type == 'read') {
        return TRUE;
      }
    }

    if ($allowed_roles = $this->get('allowedRids')) {
      foreach ($allowed_roles as $rid => $role_access) {
        if (isset($account->roles[$rid])) {
          if (!$role_access->read_only || $access_type == 'read') {
            return TRUE;
          }
        }
      }
    }

    if ($access_type == 'read' && ($this->get('public') || user_access('access all chatrooms', $account))) {
      return TRUE;
    }
  }

  /**
   * Make the given chat private.
   *
   * @return boolean
   */
  public function makePrivate() {
    return db_query('UPDATE {chatroom} SET public = 0 WHERE cid = :cid', array(':cid' => $this->get('cid')));
  }

  /**
   * Remove a user from a private chat.
   *
   * @param mixed $user_to_be_removed
   * @param mixed $user_doing_the_removing
   * @return boolean
   */
  public function removeUser($uid) {
    $params = array(
      ':cid' => $this->get('cid'),
      ':uid' => $uid,
    );
    return db_query('DELETE FROM {chatroom_user} WHERE cid = :cid AND uid = :uid', $params);
  }

  /**
   * Make the given chat public.
   *
   * @return boolean
   */
  public function makePublic() {
    return db_query('UPDATE {chatroom} SET public = 1 WHERE cid = :cid', array(':cid' => $this->get('cid')));
  }

  protected function hasChanged($attribute) {
    return in_array($attribute, $this->changedAttributes);
  }

  /**
   * Update the uids allowed to access a chat.
   *
   * @param mixed $uids
   * @return void
   */
  protected function updateUids() {
    $sql = "DELETE
            FROM {chatroom_user}
            WHERE cid = :cid";
    db_query($sql, array(':cid' => $this->get('cid')));

    foreach ($this->allowedUids as $uid) {
      $data = array('uid' => $uid->uid, 'cid' => $this->get('cid'), 'read_only' => $uid->read_only);
      drupal_write_record('chatroom_user', $data);
    }
  }

  /**
   * Update the roles allowed to access a chat.
   *
   * @param mixed $rids
   * @return void
   */
  protected function updateRids() {
    $sql = "DELETE
            FROM {chatroom_role}
            WHERE cid = :cid";
    db_query($sql, array(':cid' => $this->get('cid')));

    foreach ($this->allowedRids as $rid) {
      $data = array('rid' => $rid->rid, 'cid' => $this->get('cid'), 'read_only' => $rid->read_only);
      drupal_write_record('chatroom_role', $data);
    }
  }

  /**
   * Get the message count for a chat.
   */
  public function getMessageCount() {
    $sql = 'SELECT COUNT(cmid) FROM {chatroom_message} WHERE cid = :cid';
    return db_query($sql, array(':cid' => $this->get('cid')))->fetchField();
  }

  public function save() {
    $transaction = db_transaction();
    try {
      $data = array(
        'title' => $this->title,
        'public' => $this->public,
        'uid' => $this->uid,
        'archived' => $this->archived,
        'format' => $this->format,
      );
      $op = $this->cid ? 'update' : 'insert';
      if ($op == 'update') {
        $data['cid'] = $this->cid;
        drupal_write_record('chatroom', $data, 'cid');
      }
      else {
        drupal_write_record('chatroom', $data);
        $this->cid = $data['cid'];
      }
      if ($this->hasChanged('allowedRids')) {
        $this->updateRids();
      }
      if ($this->hasChanged('allowedUids')) {
        $this->updateUids();
      }
      module_invoke_all('chatroom_' . $op, $this);
      return TRUE;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('chatroom_save', $e);
      return FALSE;
    }
  }

  /**
   * Get messages for a given chat.
   *
   * @param $last_cmid
   *   Only load messages with cmids greater than this value.
   * @return array $messages
  */
  public function loadMessages($last_cmid = 0) {
    $sql = "SELECT cm.*, u.name, 0 AS guest_id
            FROM {chatroom_msg} cm
            INNER JOIN {users} u ON u.uid = cm.uid
            WHERE cm.cid = :cid
            AND cm.cmid > :cmid
            ORDER BY cm.cmid ASC";
    $args = array(
      ':cid' => $this->cid,
      ':cmid' => $last_cmid,
    );
    return $this->loadMessagesHelper($sql, $args);
  }

  /**
   * Load the latest messages.
   *
   * @param $limit
   *   Number of messages to load.
   * @return array
   */
  public function loadLatestMessages($limit = 20) {
    if (!preg_match('/^\d+$/', $limit)) {
      return array();
    }
    $sql = "SELECT cm.*, u.name, 0 AS guest_id
            FROM {chatroom_msg} cm
            INNER JOIN {users} u ON u.uid = cm.uid
            WHERE cm.cid = :cid
            ORDER BY cm.cmid DESC
            LIMIT $limit";
    $messages = $this->loadMessagesHelper($sql, array(':cid' => $this->cid));
    return array_reverse($messages, TRUE);
  }

  /**
   * Load older messages.
   *
   * @param $cmid
   *   Only load messages with cmids less than this value.
   * @param limit
   *   The number of messages to return.
   * @return array $messages
   *
   */
  public function loadPreviousMessages($cmid, $limit = 20) {
    $sql = "SELECT cm.*, u.name, 0 AS guest_id
            FROM {chatroom_msg} cm
            INNER JOIN {users} u ON u.uid = cm.uid
            WHERE cm.cid = :cid
            AND cm.cmid < :cmid
            ORDER BY cm.cmid DESC
            LIMIT $limit";
    $args = array(
      ':cid' => $this->cid,
      ':cmid' => $cmid,
    );
    return $this->loadMessagesHelper($sql, $args);
  }

  /**
   * Load the cmid of the oldest displayed message.
   *
   * @return $cmid
   */
  public function loadPrevMsgId() {
    $message = array_shift($this->loadLatestMessages());
    if (isset($message->cmid)) {
      return $message->cmid;
    }
    return 0;
  }

  protected function loadMessagesHelper($sql, $args) {
    $messages = array();
    $result = db_query($sql, $args)->fetchAll(PDO::FETCH_OBJ);
    foreach ($result as $message) {
      $messages[$message->cmid] = $message;
    }
    return $messages;
  }

  /**
   * Why? Because fields caching is not what we want.
   */
  public function __wakeup() {
    $this->load($this->cid);
  }
}

