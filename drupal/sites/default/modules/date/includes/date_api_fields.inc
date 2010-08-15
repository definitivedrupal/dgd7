<?php
// $Id: date_api_fields.inc,v 1.9 2010/08/11 23:42:05 karens Exp $

/**
 * @file
 * Helper for identifying Date API fields for views.
 */

/**
 *  Identify all potential date/timestamp fields.
 *
 *  @return
 *    array with fieldname, type, and table.
 *  @see 
 *    date_api_date_api_fields() which implements 
 *    the hook_date_api_fields() for the core date fields.
 */
function _date_api_fields($base = 'node') {
  
  // Make sure $base is never empty.
  if (empty($base)) {
    $base = 'node';
  }
  
  $cid = 'date_api_fields_' . $base;
  cache_clear_all($cid, 'cache_views');
  $all_fields = date_api_views_fetch_fields($base, 'field');
  $content_fields = field_views_data();
  
  // Iterate over all the fields that Views knows about.
  $fields = array();
  foreach ((array) $all_fields as $name => $val) {
    $fromto = array();
    $tmp = explode('.', $name);
    $field_name = $tmp[1];
    $table_name = $tmp[0];
        
    // If we don't have a field handler, we don't care about it.
    if (!$handler = views_get_handler($table_name, $field_name, 'field')) {
      continue; 
    }
    
    $handler_name = $handler->definition['handler'];
    $type = '';
    
    // For Field module fields, get the date type.
    $custom = array();
    if ($field_name == 'entity_id') {
      
      $field_name = $content_fields[$table_name]['entity_id']['field']['field_name'];
      $field = field_info_field($field_name);
      
      // @TODO This is cheating, we should use the API to figure this out.
      // Plus we still need to figure out how to add in value2.
      // Just trying to get things working for now.
      $name = $table_name . "." . $field_name . "_value";
    
      if ($field['type'] == 'date') {
        $type = 'cck_string';
      }
      elseif ($field['type'] == 'datestamp') {
        $type = 'cck_timestamp';
      }
      elseif ($field['type'] == 'datetime') {
        $type = 'cck_datetime';
      }
    }
    
    // Allow custom modules to provide date fields.
    // The is_a() function makes this work for any handler
    // that was derived from 'views_handler_field_date' .
    elseif (is_a($handler, 'views_handler_field_date')) {
      foreach (module_implements('date_api_fields') as $module) {
        $function = $module . '_date_api_fields';
        if ($custom = $function("$table_name.$field_name")) {
          $type = 'custom';
          break;
        }
      }
    }

    // Don't do anything if this is not a date field we can handle.
    if (!empty($type)) {

      // Handling for simple timestamp fields
      $alias = str_replace('.', '_', $name);
      $fromto = array($alias, $alias);
      $tz_handling = 'site';
      $related_fields = array();
      $timezone_field = '';
      $offset_field = '';
      $rrule_field = '';
      $delta_field = '';
      $granularity = array('year', 'month', 'day', 'hour', 'minute');
    
      if (isset($field['tz_handling'])) {
        $tz_handling = $field['tz_handling'];
        $db_info = date_api_database_info($field);
        if ($tz_handling == 'date') {
          $offset_field = $table_name . ' . ' . $db_info['columns']['offset']['column'];
        }
        $related_fields = array(
          $table_name . ' . ' . $field_name
          );
        if (isset($db_info['columns']['value2']['column'])) {
          $related_fields = array_merge($related_fields, array($table_name . ' . ' . $db_info['columns']['value2']['column']));
        }
        if (isset($db_info['columns']['timezone']['column'])) {
          $related_fields = array_merge($related_fields, array($table_name . ' . ' . $db_info['columns']['timezone']['column']));
          $timezone_field = $table_name . ' . ' . $db_info['columns']['timezone']['column'];
        }
        if (isset($db_info['columns']['rrule']['column'])) {
          $related_fields = array_merge($related_fields, array($table_name . ' . ' . $db_info['columns']['rrule']['column']));
          $rrule_field = $table_name . ' . ' . $db_info['columns']['rrule']['column'];
        }
      }
      // Get the delta value into the query.
      if (!empty($handler->content_field['multiple'])) {
        array_push($related_fields, "$table_name.delta");
        $delta_field = $table_name . '_delta';
      }

      // Handling for cck fromto dates
      if (isset($handler->content_field_name)) {
        $real_field_name = $this->content_field_name;
        $field = field_info_field($real_field_name);
    
        switch ($field['type']) {
          case 'date':
          case 'datetime':
          case 'datestamp':
            $db_info = date_api_database_info($field);
            $fromto = array(
              $table_name . '_' . $db_info['columns']['value']['column'],
              $table_name . '_' . (!empty($field['todate']) ? $db_info['columns']['value2']['column'] : $db_info['columns']['value']['column']),
              );
            break;
        }
        $granularity = !empty($field['granularity']) ? $field['granularity'] : array('year', 'month', 'day', 'hour', 'minute');
      }
      
      switch ($type) {
        case 'cck_string':
          $sql_type = DATE_ISO;
          break;
        case 'cck_datetime':
          $sql_type = DATE_DATETIME;
          break;
        default:
          $sql_type = DATE_UNIX;
          break;
      }
      $fields['name'][$name] = array(
        'type' => $type,
        'sql_type' => $sql_type,
        'label' => $val['group'] . ': ' . $val['title'],
        'granularity' => $granularity,
        'fullname' => $name,
        'table_name' => $table_name,
        'field_name' => $field_name,
        'query_name' => $alias,
        'fromto' => $fromto,
        'tz_handling' => $tz_handling,
        'offset_field' => $offset_field,
        'timezone_field' => $timezone_field,
        'rrule_field' => $rrule_field,
        'related_fields' => $related_fields,
        'delta_field' => $delta_field,
      );
      
      // Allow the custom fields to over-write values.
      if (!empty($custom)) {
        foreach ($custom as $key => $val) {
          $fields['name'][$name][$key] = $val;
        }
      }
      $fields['name'][$name]['real_field_name'] = $field_name == 'entity_id' ? $real_field_name : $field_name;
      $fields['alias'][$alias] = $fields['name'][$name];
    }
  }

  cache_set($cid, $fields, 'cache_views');
  return $fields;
}
