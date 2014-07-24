<?php
/**
 * @file
 * A virtual MailChimp API implementation for use in testing.
 */

class MailChimpTest extends MailChimp {

  /**
   * Get the details of an email address's settings in a particular list.
   * 
   * @return array
   *   An array that counts the successes and failures, and contains
   *   a Data index for the info. Only supports a single operation despite
   *   the structure of the success and error indexes.
   */
  public function listMemberInfo($id, $email_address) {
    $lists = $this->loadLists();
    $ret = array(
      'success' => 0,
      'errors' => 0,
      'data' => array(),
    );
    if (isset($lists[$id]['data'][$email_address[0]])) {
      $member = $lists[$id]['data'][$email_address[0]];
      $ret['success'] = 1;
      $ret['data'][] = array(
        'email' => $email_address[0],
        'email_type' => $member['email_type'],
        'merges' => $member['merge_vars'],
        'status' => $member['subscribed'] ? 'subscribed' : 'unsubscribed',
      );
    }
    else {
      $ret['errors'] = 1;
    }
    return $ret;
  }

  /**
   * Get the mergevars for a list.
   * 
   * @return array|bool
   *   Returns an array of mergevars if they exist, otherwise false.
   */
  public function listMergeVars($id) {
    $lists = $this->loadLists();
    if (isset($lists[$id]) && isset($lists[$id]['mergevars'])) {
      return $lists[$id]['mergevars'];
    }
    return FALSE;
  }

  /**
   * Subscribe a user to a list.
   * 
   * @return bool
   *   TRUE if user was added or subscribed, otherwise FALSE.
   */
  public function listSubscribe($id, $email_address, $merge_vars = NULL, $email_type = 'html', $double_optin = TRUE, $update_existing = FALSE, $replace_interests = TRUE, $send_welcome = FALSE) {
    $lists = $this->loadLists();
    if (isset($lists[$id])) {
      if (isset($lists[$id]['data'][$email_address])) {
        $lists[$id]['data'][$email_address]['subscribed'] = TRUE;
      }
      else {
        $lists[$id]['data'][$email_address] = array(
          'subscribed' => TRUE,
          'email_type' => $email_type,
          'merge_vars' => $merge_vars,
        );
      }
      $this->writeLists($lists);

      return TRUE;
    }
    else {
      $this->errorMessage = "Could not add " . $email_address . " to non-existant list: " . $id;
      return FALSE;
    }
  }

  /**
   * Batch subscribe method.
   */
  public function listBatchSubscribe($id, $batch, $double_optin = TRUE, $update_existing = FALSE, $replace_interests = TRUE) {
    foreach ($batch as $sub) {
      $email = isset($sub['EMAIL']) ? $sub['EMAIL'] : NULL;
      $this->listSubscribe($id, $email, $merge_vars = $sub, 'html', TRUE, TRUE);
    }
  }

  /**
   * Unsubscribe a user from a list.
   * 
   * @return bool
   *   True if the user was removed, false if unnecessary or no such list.
   */
  public function listUnsubscribe($id, $email_address, $delete_member = FALSE, $send_goodbye = TRUE, $send_notify = TRUE) {
    $lists = $this->loadLists();
    if (isset($lists[$id])) {
      if (isset($lists[$id]['data'][$email_address])) {
        if ($lists[$id]['data'][$email_address]['subscribed']) {
          if ($delete_member) {
            unset($lists[$id]['data'][$email_address]);
          }
          else {
            $lists[$id]['data'][$email_address]['subscribed'] = FALSE;
          }
          $this->writeLists($lists);

          return TRUE;
        }
        else {
          $this->errorMessage = "Could not unsubscribe " . $email_address . " from: " . $id . ": not currently subscribed.";
        }
      }
      else {
        $this->errorMessage = "Could not unsubscribe " . $email_address . " from: " . $id . ": address not on list";
      }
    }
    else {
      $this->errorMessage = "Could not unsubscribe " . $email_address . " from non-existant list: " . $id;
    }
    return FALSE;
  }

  /**
   * Emulates batch unsubscribe.
   */
  public function listBatchUnsubscribe($id, $emails, $delete_member = FALSE, $send_goodbye = TRUE, $send_notify = TRUE) {
    foreach ($emails as $email) {
      $this->listUnsubscribe($id, $email, $delete_member, $send_goodbye, $send_notify);
    }
  }

  /**
   * Update a list member.
   * 
   * @return bool
   *   True if an update took place, otherwise False.
   */
  public function listUpdateMember($id, $email_address, $merge_vars, $email_type = '', $replace_interests = TRUE) {
    $lists = $this->loadLists();
    if (isset($lists[$id])) {
      if (isset($lists[$id]['data'][$email_address])) {
        foreach ($merge_vars as $var => $value) {
          $lists[$id]['data'][$email_address]['merge_vars'][$var] = $value;
        }
        if (isset($merge_vars['EMAIL']) && strcmp($email_address, $merge_vars['EMAIL'])) {
          $lists[$id][$merge_vars['EMAIL']] = $lists[$id]['data'][$email_address];
          unset($lists[$id]['data'][$email_address]);
        }
        $this->writeLists($lists);

        return TRUE;
      }
      else {
        $this->errorMessage = "Could not update " . $email_address . " on: " . $id . ": not currently a member.";
      }
    }
    else {
      $this->errorMessage = "Could not update " . $email_address . " on non-existant list: " . $id;
    }
    return FALSE;
  }

  /**
   * Load lists.
   *
   * @return array
   *   Stored lists formatted as the actual MC API returns them.
   */
  public function lists($filters = array(), $start = 0, $limit = 25, $sort_field = 'created', $sort_dir = 'DESC') {
    $lists = $this->loadLists();
    $ret = array(
      'data' => array(),
      'total' => 0,
    );
    foreach ($lists as $list_id => $list_array) {
      $ret['data'][] = array(
        'id' => $list_id,
        'name' => $list_array['name'],
      );
      $ret['total']++;
    }
    return $ret;
  }

  /**
   * Loads list values, initializing if necessary.
   *
   * @return array
   *   Stored lists.
   */
  protected function loadLists() {
    $list_data = variable_get('mailchimp_test_list_data', $this->defaultLists());

    return $list_data;
  }

  /**
   * Creates initial list values.
   *
   * @return array
   *   Basic lists.
   */
  protected function defaultLists() {
    $default_mergevars = array();
    $default_mergevars[] = array(
      'name' => 'Email',
      'order' => 0,
      'tag' => 'EMAIL',
      'req' => TRUE,
      'webid' => 'test',
      'field_type' => 'text',
      'size' => 40,
      'default' => '',
      'public' => TRUE,
    );
    $default_mergevars[] = array(
      'name' => 'First Name',
      'order' => 1,
      'tag' => 'FIRSTNAME',
      'req' => FALSE,
      'webid' => 'test',
      'field_type' => 'text',
      'size' => 40,
      'default' => '',
      'public' => TRUE,
    );
    $default_mergevars[] = array(
      'name' => 'Last Name',
      'order' => 2,
      'tag' => 'LASTNAME',
      'req' => FALSE,
      'web_id' => 'test',
      'field_type' => 'text',
      'size' => 40,
      'default' => '',
      'public' => TRUE,
    );
    $lists = array(
      MAILCHIMP_TESTLIST_ANONYMOUS => array(
        'name' => 'Test List A',
        'data' => array(),
        'mergevars' => $default_mergevars,
      ),
      MAILCHIMP_TESTLIST_OPTIONAL => array(
        'name' => 'Test List B',
        'data' => array(),
        'mergevars' => $default_mergevars,
      ),
      MAILCHIMP_TESTLIST_REQUIRED => array(
        'name' => 'Test List C',
        'data' => array(),
        'mergevars' => $default_mergevars,
      ),
    );
    return $lists;
  }

  /**
   * Saves list changes.
   */
  protected function writeLists($lists) {
    variable_set('mailchimp_test_list_data', $lists);
  }

}
