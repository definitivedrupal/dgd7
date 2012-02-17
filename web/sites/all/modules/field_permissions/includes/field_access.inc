<?php

/**
 * @file
 * Implementation of helper functions related to hook_field_access().
 */

/**
 * Implementation of hook_field_access('view').
 */
function _field_permissions_field_view_access($field_name, $field_permissions, $obj_type, $object, $account) {
  // Check if user has access to view this field in any object.
  if (!empty($field_permissions['view']) && user_access('view '. $field_name, $account)) {
    return TRUE;
  }

  // If 'view own' permission has been enabled for this field, then we can
  // check if the user has the right permission, and ownership of the object.
  if (!empty($field_permissions['view own']) && user_access('view own '. $field_name, $account)) {

    // When field_access('view') is invoked, it may or may not provide
    // an object. It will, almost always, except when this function is
    // invoked as a field access callback from Views, where it is used to
    // evaluate if the field can be included in the query itself. In this
    // case we should grant access. Views will invoke field_access('view')
    // again, indirectly, when rendering the fields using field_format(),
    // and this time it will provide a pseudo object that includes the
    // uid of the object author, so here is where we have the chance to
    // evaluate ownership to check for 'view own <field>' permission.
    if (!isset($object)) {
      return TRUE;
    }

    // Try to get the uid of the object author from the object itself.
    // When invoked by Views to render a field, we may not have the uid of the
    // object, so we need to retrieve it from the object table.
    if (isset($object->uid)) {
      $object_uid = $object->uid;
    }
    // @todo: what to do with non-node objects?
    elseif (!empty($object->vid)) {
      $object_uid = db_query('SELECT uid FROM {node_revision} WHERE vid = :vid', array(':vid' => $object->vid))->fetchField();
    }
    elseif (!empty($object->nid)) {
      $object_uid = db_query('SELECT uid FROM {node} WHERE nid = :nid', array(':nid' => $object->nid))->fetchField();
    }
    else {
      // Deny access to view the field if we have not been able to get the uid
      // of the object author.
      return FALSE;
    }

    // Finally, we can now check if ownership of the object matches.
    return (is_numeric($object_uid) && $object_uid == $account->uid);
  }

  return FALSE;
}

/**
 * Implementation of hook_field_access('edit').
 */
function _field_permissions_field_edit_access($field_name, $field_permissions, $obj_type, $object, $account) {
  // Check if user has access to edit this field on object creation.
  if (empty($object->nid) && !empty($field_permissions['create'])) {
    return user_access('create '. $field_name, $account);
  }

  // Check if user has access to edit this field in any object.
  if (!empty($field_permissions['edit']) && user_access('edit '. $field_name, $account)) {
    return TRUE;
  }

  // If 'edit own' permission has been enabled for this field, then we can
  // check if the user has the right permission, and ownership of the object.
  if (!empty($field_permissions['edit own']) && user_access('edit own '. $field_name, $account) && $object->uid == $account->uid) {
    return TRUE;
  }

  return FALSE;
}
