<?php
// $Id: date_tools.wizard.inc,v 1.7 2010/04/13 19:35:33 karens Exp $

function date_tools_wizard_form() {
  
  $form = array();
  $form['type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content type'),
    );
  $form['type']['bundle'] = array(
    '#type' => 'textfield',
    '#default_value' => 'date',
    '#title' => t('Content type name'),
    '#description' => t('Machine-readable name. Allowed values: (a-z, 0-9, _). If this is not an existing content type, the content type will be created.'),
    ); 
  $form['type']['name'] = array(
    '#type' => 'textfield',
    '#default_value' => t('Date'),
    '#title' => t('Content type label'),
    '#description' => t('The human-readable name for this content type. Only needed when creating a new content type.'),
    ); 
  $form['type']['type_description'] = array(
    '#type' => 'textarea',
    '#default_value' => t('A date content type that is linked to a Views calendar.'),
    '#title' => t('Content type description'),
    '#description' => t('A description for the content type. Only needed when creating a new content type.'),
    );      
  $form['field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date field'),
    );
  $form['field']['field_name'] = array(
    '#type' => 'textfield',
    '#default_value' => 'date',
    '#field_prefix' => 'field_',
    '#title' => t('Date field name'),
    '#description' => t('Machine-readable name. Allowed values: (a-z, 0-9, _) Must not be an existing field name.'),
    );
  $form['field']['label'] = array(
    '#tree' => TRUE,
    '#type' => 'textfield',
    '#default_value' => t('Date'),
    '#title' => t('Date field label'),
    '#description' => t('The human-readable label for this field.'),
    );
  $form['field']['widget_type'] = array(
    '#type' => 'select',
    '#options' => date_tools_wizard_widget_types(),
    '#default_value' => 'date_select',
    '#title' => t('Date widget type'),
    );
  $form['field']['repeat'] = array(
    '#type' => 'select',
    '#default_value' => 0,
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#title' => t('Show repeating date options'),
    );  
  $form['field']['advanced'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Advanced options'),
    );
  $form['field']['advanced']['field_type'] = array(
    '#type' => 'select',
    '#options' => date_tools_wizard_field_types(),
    '#default_value' => 'datetime',
    '#title' => t('Date field type'),
    '#description' => t("The recommend type is Datetime, except for historical dates or dates with only year or month granularity. Older or incomplete dates should use the Date type (an ISO date)."),
    );
  $form['field']['advanced']['granularity'] = array(
    '#type' => 'select',
    '#options' => date_granularity_names(),
    '#default_value' => array('month', 'day', 'year', 'hour', 'minute'),
    '#title' => t('Granularity'),
    '#multiple' => TRUE,
    );  
  $form['field']['advanced']['tz_handling'] = array(
    '#type' => 'select',
    '#options' => date_tools_wizard_tz_handling(),
    '#default_value' => 'site',
    '#title' => t('Date timezone handling'),
    '#description' => t("Timezone handling should be set to 'none' for granularity without time elements.")
    );
  $form['calendar'] = array(
    '#type' => 'select',
    '#default_value' => 1,
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#title' => t('Create a calendar for this date field'),
    ); 
  $form['blocks'] = array(
    '#type' => 'select',
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => 0,
    '#title' => t('Add calendar blocks to the current theme'),
    );  
     
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    );      
  
  return $form;
}

function date_tools_wizard_form_validate(&$form, &$form_state) {
  $bundle = $form_state['values']['bundle'];
  $field_name = 'field_' . $form_state['values']['field_name'];
  $existing_type = db_query("SELECT type FROM {node_type} WHERE type=:bundle", array(':bundle' => $bundle))->fetchField();
  $existing_instance = db_query("SELECT field_name FROM {field_config_instance} WHERE field_name=:field_name AND bundle=:bundle AND entity_type=:entity_type", array(':field_name' => $field_name, ':bundle' => $bundle, ':entity_type' => 'node'))->fetchField();
  if ($existing_type) {
    drupal_set_message(t('This content type name already exists, adding new field to existing content type.'));
  }
  if (!preg_match('!^[a-z0-9_]+$!', $bundle)) {
    form_set_error('bundle', t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'));
  }
  if ($existing_instance) {
    form_set_error('field_name', t('This field name already exists.'));
  }
  if (!date_has_time($form_state['values']['granularity']) && $form_state['values']['tz_handling'] != 'none') {
    form_set_error('tz_handling', t('Timezone handling must be none for granularity without time.'));
  }
}

function date_tools_wizard_form_submit(&$form, &$form_state) {
  $view_name = date_tools_wizard_build($form_state['values']);
  menu_rebuild();
  if (!empty($view_name)) {
    drupal_set_message(t('Change the calendar as needed and save the view.'), 'error');
    $form['#redirect'] = 'admin/structure/views/edit/' . $view_name;
  }
  else {
    $form['#redirect'] = 'admin/structure/types/manage/' . str_replace('_', '-', $form_state['values']['bundle']) . '/fields';
  }
}

function date_tools_wizard_build($form_values) {
  extract($form_values);
  
  $field_name = 'field_' . $field_name;
  
  if (!empty($repeat)) {
    $widget_type .= '_repeat';
  }
  
  // Create a node type if it doesn't already exist.
  // This makes it possible to add additional date fields to 
  // an existing type.
  $types = node_type_get_names();
  $type_settings = array();
  if (!array_key_exists($bundle, $types)) {
    date_tools_wizard_create_content_type($name, $bundle, $type_description, $type_settings);
  }
  else {
    $types = node_type_get_types();
    $type = $types[$bundle];
    if (!empty($type_settings)) {
      foreach ($type_settings as $key => $setting) {
        $type->$key = $setting;
      }
      node_type_save($type);
    }
    $name = $type->name;
  }
  
  $field_settings = array(
    'field' => array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => $repeat ? FIELD_CARDINALITY_UNLIMITED : 0,
      'settings' => array(
        'granularity' => $granularity,
        'tz_handling' => $tz_handling,
        'repeat' => $repeat,
        'todate' => 'optional',
       ),
    ),
    'instance' => array(
      'field_name' => $field_name,
      'label' => $label,
      'bundle' => $bundle,
      // Move the date right below the title.
      'weight' => -4,
      'widget' => array(
        'type' => $widget_type,
        // Increment for minutes and seconds, can be 1, 5, 10, 15, or 30.
        'settings' => array(
          'increment' => 15,
          // The number of years to go back and forward in drop-down year selectors.
          'year_range' => '-0:+1',
        ),
      ),
      'settings' => array(
        // Set a display format that will show complete date information, 
        // to help test that values are correct.
        'default_format' => 'long',
      ),
    )
  );
 
  $date_field = date_tools_wizard_create_date_field($field_type, $widget_type, $tz_handling, $field_settings);
  
  $view_name = '';
  if (!empty($calendar)) {
    $view_name = date_tools_wizard_create_calendar($name, $bundle, $date_field);
    if (!empty($blocks)) {
      date_tools_wizard_create_blocks($bundle);  
    }
  }
  return $view_name;
  
}

function date_tools_wizard_include() {
  module_load_include('inc', 'node', 'content_types');
  module_load_include('inc', 'node', 'node.pages');
  module_load_include('inc', 'field', 'field.crud');
  module_load_include('inc', 'date', 'date_admin');
}

function date_tools_wizard_options($options = array()) {
  $default_options = array(
    'popups', // Use popup features like Date Popup and Calendar Popup?
    'repeat', // Use repeating dates?
    'linked',  // Create links between calendars and dates?
    );
  // Allow override of default options.
  if (!empty($options)) {
    return $options;
  }
  else {
    return $default_options;
  }
}

/**
 * Return an array of the modules needed by this wizard.
 */
function date_tools_wizard_required_modules($options = array()) {
  $options = date_tools_wizard_options($options);
  $modules = array(
    'date_timezone', 'date_api', 'content', 'date', 
    'calendar', 'views', 'views_ui',
  );
  if (in_array('popups', $options)) {
    $modules = array_merge($modules, array('date_popup', 'jcalendar'));
  }
  if (in_array('repeat', $options)) {
    $modules = array_merge($modules, array('date_repeat'));
  }
  return $modules;
}

function date_tools_wizard_modules() {
  return array(
    'date' => t('Date'),
    'date_api' => t('Date API'),
    'date_timezone' => t('Date Timezone'),
    'field' => t('Field'),
    'calendar' => t('Calendar'),
    'calendar_ical' => t('Calendar iCal'),
    'date_repeat' => t('Date Repeat'),
    'date_popup' => t('Date Popup'),
    'jcalendar' => t('Calendar Popup'),
    'views' => t('Views'),
    'views_ui' => t('Views UI'),
    );
}

function date_tools_wizard_enabled_modules($options = array()) {
  $modules = date_tools_wizard_required_modules($options);
  $enabled = array();
  $names = date_tools_wizard_modules();
  foreach ($modules as $option) {
    if (module_exists($option)) {
      $enabled[$option] = $names[$option] . ' (<span class="admin-enabled">' . t('enabled') . '</span>)';
    }
  }
  return $enabled;
}

function date_tools_wizard_disabled_modules($options = array()) {
  $modules = date_tools_wizard_required_modules($options);
  $enabled = array();
  $names = date_tools_wizard_modules();
  foreach ($modules as $option) {
    if (!module_exists($option)) {
      $enabled[$option] = $names[$option] . ' (<span class="admin-disabled">' . t('disabled') . '</span>)';
    }
  }
  return $enabled;
}

function date_tools_wizard_field_types() {
  $field_types = array();  
  foreach (date_field_info() as $name => $info) {
    $field_types[$name] = $info['label']; //. ' - ' . $info['description'];
  }
  return $field_types;  
}

function date_tools_wizard_widget_types() {
  $widget_types = array();  
  foreach (date_field_widget_info() as $name => $info) {
    if (!strstr($name, '_repeat')) {
      $widget_types[$name] = $info['label'];
    }
  }
  return $widget_types;  
}

function date_tools_wizard_tz_handling() {
  include_once(drupal_get_path('module', 'date') . '/date_admin.inc');
  return date_timezone_handling_options();
}

function date_tools_wizard_create_content_type($name, $bundle, $description, $type_settings = array()) {
  
  date_tools_wizard_include();
  
  // Create the content type.
  $values  = array (
    'name' => $name,
    'type' => $bundle,
    'description' => $description,
    'title_label' => 'Title',
    'body_label' => 'Body',
    'min_word_count' => '0',
    'help' => '',
    'node_options' => 
    array (
      'status' => 1,
      'promote' => 1,
      'sticky' => 0,
      'revision' => 0,
    ),
    'language_content_type' => '0',
    'old_type' => $bundle,
    'orig_type' => '',
    'base' => 'node_content',
    'custom' => '1',
    'modified' => '1',
    'locked' => '0',
    'url_str' => str_replace('_', '-', $bundle),
  );
  
  // Allow overrides of these values.
  foreach ($type_settings as $key => $value) {
    $values[$key] = $value;
  }

  node_type_save((object) $values);
  
  // Update the menu router information.
  menu_rebuild();
  field_cache_clear();
  
}


function date_tools_wizard_create_date_field($field_type, $widget_type, $tz_handling, $overrides = array()) {
  date_tools_wizard_include();
    
  // Get field default values for this combination of field and widget type.
  $result = date_tools_default_values($field_type, $widget_type, $tz_handling, $overrides); 
  
  $field = field_create_field($result['field']);
  $instance = field_create_instance($result['instance']);
  
  return $field;
}

function date_tools_wizard_create_calendar($name, $bundle, $date_field) {
  
  $date_name = !empty($date_field) ? $date_field['field_name'] : '';
  $path = 'calendar-' . str_replace('field_', '', $date_name);
  
  // Add a default calendar for this content type.
  $calendar_options = variable_get('calendar_default_view_options', array());
  if (array_key_exists('calendar_' . $bundle, $calendar_options)) {
    unset($calendar_options['calendar_' . $bundle]);
  }
  $option = array(
    'name' => 'calendar_' . $bundle,
    'description' => 'An event calendar for ' . $date_field['widget']['label'],
    'path' => $path,
    'types' => array($bundle => $name),
    'date_fields' => array($date_name),
    'display_fields' => array('title' => array(), $date_name => array()),
  );
  $calendar_options['calendar_' . $bundle] = $option;
  variable_set('calendar_default_view_options', $calendar_options);
  
  // Make sure menu items get rebuilt as necessary.
  menu_rebuild();

  // Clear the views cache.
  cache_clear_all('*', 'cache_views');

  // Clear the page cache.
  cache_clear_all();  
  
  // Remove this view from cache so we can edit it properly.
  views_object_cache_clear('view', 'calendar_' . $bundle);  

  // Force Views to empty its cache and find the new view.
  views_discover_default_views();

  return 'calendar_' . $bundle;
}

function date_tools_wizard_create_blocks($bundle) {
  // Add calendar blocks to the default theme.
  $current_theme = variable_get('theme_default', 'garland');
  
  // Legend block.
  $block = new stdClass();
  $block->theme = $current_theme;
  $block->status = 1;
  $block->weight = -1;
  $block->region = 'left';
  $block->title = '';
  $block->module = 'calendar';
  $block->delta = 0;
  date_tools_wizard_add_block($block);
  
  // Mini calendar block. 
  $block->module = 'views';
  $block->delta = 'calendar_' . $bundle . '-calendar_block_1';
  date_tools_wizard_add_block($block); 
  
  // Upcoming events block.
  $block->module = 'views';
  $block->delta = 'calendar_' . $bundle . '-block_1';
  date_tools_wizard_add_block($block);
  return;      
}

/**
 * Add a block.
 */
function date_tools_wizard_add_block($block) {
  $bid = db_result(db_query("SELECT bid FROM {blocks} WHERE module='%s' AND delta='%s' AND theme='%s'", $block->module, $block->delta, $block->theme));
  if (empty($bid)) {
    $block->bid = NULL;
    drupal_write_record('blocks', $block);
  }
  else {
    $block->bid = $bid;
    drupal_write_record('blocks', $block, array('bid'));
  }
}