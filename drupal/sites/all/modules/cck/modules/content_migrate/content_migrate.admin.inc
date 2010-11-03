<?php
// $Id: content_migrate.admin.inc,v 1.3 2010/04/24 18:30:42 karens Exp $
/*

/**
 * @file content_migrate.admin.inc
 * Contains page callbacks for the module.
 */

/**
 * Form generator for the migration selection form.
 * 
 * @todo Make this into a nice table where you have 
 * an option to check all available fields to migrate
 * them all at once.
 */
function content_migrate_select($form, &$form_state) {
  $form = array();
  $options = array('available' => array(), 'converted' => array(), 'missing' => array());
  
  $field_values = content_migrate_get_field_values();
  
  if (empty($field_values)) {
    drupal_set_message(t('There is no D6 field information in this database.'));
    return $form;
  }

  $type_names = node_type_get_names();
  $new_fields = array_keys(field_info_fields());
  
  // Figure out which field and widget modules are available.
  $available_modules = array_unique(array_merge(module_implements('field_info'), module_implements('field_widget_info')));
  
  foreach ($field_values as $field_name => $field_value) {
    $bundles = array();
    $missing_module = !in_array($field_value['module'], $available_modules);
    $missing_modules = $missing_module ? array($field_value['module']) : array();
    $instance_values = content_migrate_get_instance_values(NULL, $field_name);
    
    // Debug
    //dsm($field_value);
    //dsm($instance_values);
    
    foreach ($instance_values as $bundle => $instance_value) {
      $bundles[] = $type_names[$bundle];
      $label = $instance_value['label'];
      if (!in_array($instance_value['widget']['module'], $available_modules)) {
        $missing_module = TRUE;
        $missing_modules[] = $instance_value['widget']['module'];
      }
    }
    $data = array(
      0 => $field_name,
      1 => $field_value['type'],
      2 => implode(', ', $bundles),
      3 => empty($missing_modules) ? '' : t('Missing modules: @list', array('@list' => implode(', ', $missing_modules))),
    );
    if (in_array($field_name, $new_fields)) {
      $options['converted'][$field_name] = $data;
    }
    // TODO, do we need to check for more than the mere presence of a module?
    elseif ($missing_module) {
      $options['missing'][$field_name] = $data;
    }
    else {
      $options['available'][$field_name] = $data;
    }
  }

  $header = array(t('Field'), t('Field type'), t('Content type(s)'), t('Other information'));
  $form['#tree'] = TRUE;
  $form['available'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => count($options['available']) < 1,
    '#title' => t('Available fields'),
    '#description' => t('Fields that have not yet been migrated but are available for migration.'),
  );
  $form['available']['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options['available'],
    '#empty' => t('No fields are available to be migrated.'),
  );  
  $form['available']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Migrate selected fields'),
    '#submit' => array('content_migrate_select_submit'),
  );  

  $form['converted'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => count($options['converted']) < 1,
    '#title' => t('Converted fields'),
    '#description' => '<p>'. t('Fields that have already been converted. You can choose to roll them back if the conversion did not work correctly. Note that rolling fields back will completely destroy the new field tables.') . ' <span class="error"><strong>' . t('This operation cannot be undone!') . '</strong></span>',
  );    
  $form['converted']['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options['converted'],
    '#empty' => t('No fields are already converted.'),
  );
  $form['converted']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Roll back selected fields'),
    '#submit' => array('content_migrate_rollback_submit'),
  );     
  
  $form['missing'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => count($options['missing']) < 1,
    '#title' => t('Unavailable fields'),
    '#description' => t('Fields that cannot be migrated because some modules are missing.'),
  );  
  $form['missing']['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options['missing'],
    '#empty' => t('No fields have missing modules.'),
  ); 
      
  return $form;
}

/**
 * Submit handler.
 * 
 * @TODO add a confirmation on the rollback submission.
 */
function content_migrate_rollback_submit($form, &$form_state) {
  $field_names = array_filter($form_state['values']['converted']['data']);
  foreach ($field_names as $field_name) {
    $field = field_info_field($field_name);
    
    // Deleting the field only marks it for deletion.
    field_delete_field($field_name);
    
    // We are bypassing the field batch processing
    // and simply deleting all the data.
    // The assumption is that the migration was
    // unsuccessful and will be re-attempted
    // and we need to remove all traces of the 
    // new field for later migrations to work.
    $new_table = content_migrate_new_table($field);
    db_drop_table($new_table);
    
    $instances = field_read_instances(array('field_id' => $field['id']), array('include_deleted' => 1));
    foreach ($instances as $instance) {
      field_purge_instance($instance);
    }
    field_purge_field($field);
    drupal_set_message(t('Rolling back @field_name.', array('@field_name' => $field_name)));
  }
}

/**
 * Submit handler.
 */
function content_migrate_select_submit($form, &$form_state) {
  $field_names = array_filter($form_state['values']['available']['data']);
  _content_migrate_batch($field_names);
}

/**
 * Helper function to create a batch.
 */
function _content_migrate_batch($field_names) {
  $batch = array(
    'title' => t('Migrating data'),
    'file'  => drupal_get_path('module', 'content_migrate') . '/content_migrate.admin.inc',
    'operations' => array(
      array('_content_migrate_batch_process_create_fields', array($field_names)),
    ),
    'finished' => "Field migration is finished",
    'init_message' => t("Fields migration is starting."),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Field migration has encountered an error.'),
  );
  // Migrate field data one field at a time.
  foreach ($field_names as $field_name) {
    $batch['operations'][] = array('_content_migrate_batch_process_migrate_data', array($field_name));
  }
  batch_set($batch);
}

/**
 * Batch operation callback to create fields.
 */
function _content_migrate_batch_process_create_fields($field_names) {
  $type_names = node_type_get_names();
  foreach ($field_names as $field_name) {
    $context['message'] = "creating field $field_name";
    $field_value = content_migrate_get_field_values($field_name);
    
    // Create the field and store the new field 
    // definition in $context so we can retrieve it later. 
    try {
      unset($data['field']['columns']);
      unset($data['field']['db_storage']); 
      $field = field_create_field($field_value);
      $context['fields'][$field_name] = $field;
      drupal_set_message(t("Created field @field_name", array('@field_name' => $field_name))); 
      
      // Create each of the new instances and store 
      // the new instance definitions in $context.
      $instance_values = content_migrate_get_instance_values(NULL, $field_name);
      foreach ($instance_values as $bundle => $instance_value) {
        try {
          $instance = field_create_instance($instance_value); 
          $context['instances'][$fieldname][$instance['bundle']] = $instance;
          drupal_set_message(t("Created instance of @field_name in bundle @bundle.", array(
            '@field_name' => $field_name, '@bundle' => $type_names[$instance['bundle']])));
        }
        catch (Exception $e) {
          drupal_set_message(t('Error creating instance of @field_name in bundle @bundle.', array(
            '@field_name' => $field_name, '@bundle' => $type_names[$instance_value['bundle']])), 'error');
          drupal_set_message($e, 'error');
        }
      }
    }
    catch (Exception $e) {
      drupal_set_message(t("Error creating field @field_name", array('@field_name' => $field_name)), 'error'); 
      drupal_set_message($e, 'error');
    }
    field_info_cache_clear();

  }
  $context['finished'] = TRUE;
} 

/**
 * Batch operation callback to migrate data.
 * Copy old table data to new field table.
 */
function _content_migrate_batch_process_migrate_data($field_name) {
  $field_value = content_migrate_get_field_values($field_name);
  $instance_values = content_migrate_get_instance_values(NULL, $field_name);
  
  $field = field_info_field($field_name);
  //$instances = field_read_instances(array('field_id' => $field['id']));
  foreach ($instance_values as $instance_value) {
    
    $old_table          = content_migrate_old_table($field_value, $instance_value);
    $old_cols           = content_migrate_old_columns($field_value, $instance_value);
    $new_table          = content_migrate_new_table($field);
    $new_revision_table = content_migrate_new_revision($field);
    $new_columns        = content_migrate_new_columns($field);
    
    $query = db_select($old_table, 'old');
    
    // We need a new columns for bundle name, entity type, and language.
    $query->addExpression("'". $instance_value['bundle'] ."'", 'bundle');
    $query->addExpression(1, 'etid');
    $query->addExpression("'". LANGUAGE_NONE ."'", 'language');
    
    // There are new names for what were the nid and vid columns.
    $query->addField('old', 'nid', 'entity_id');
    $query->addField('old', 'vid', 'revision_id');
    
    // Add the field columns to the select query.
    // Use the new column names as aliases in case the 
    // name changed, hopefully none did.
    foreach ($old_cols as $key => $col) {
      $query->addField('old', $col, $new_columns[$key]);
    }
    
    // Add delta, or construct it if missing.
    if (content_migrate_storage_type($field_value, $instance_value) == CONTENT_DB_STORAGE_PER_FIELD) {
      $query->addField('old', 'delta', 'delta');
    }
    else {
      $query->addExpression(0, 'delta');
    }
    
    // Now copy the data to the new field tables.
    try {
      db_insert($new_table)
        ->from($query)
        ->execute();
      db_insert($new_revision_table)
        ->from($query)
        ->execute();
      drupal_set_message(t('Data migrated for field @field_name in bundle @bundle.', array(
        '@field_name' => $field_name, '@bundle' => $instance_value['bundle'])));
    }
    catch (Exception $e) {
      drupal_set_message(t('Data could not be migrated for field @field_name in bundle @bundle.', array(
        '@field_name' => $field_name, '@bundle' => $instance_value['bundle'])), 'error');
      drupal_set_message($e, 'error');
    }
  }
}