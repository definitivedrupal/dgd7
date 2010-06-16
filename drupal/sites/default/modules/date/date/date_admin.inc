<?php
// $Id: date_admin.inc,v 1.59 2010/03/28 11:48:48 karens Exp $
/**
 * @file
 * Date administration code.
 * Moved to separate file since there is a lot of code here that is not needed often.
 */
function _date_field_instance_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $instance['settings'];
  
  $form['default_value'] = array(
    '#type' => 'select', '#title' => t('Default value'),
    '#default_value' => $settings['default_value'],
    '#options' => array('blank' => t('Blank'), 'now' => t('Now'), 'strtotime' => t('Relative')),
    '#description' => t("A default value to use for this field. If you select 'Relative', add details below."),
    '#weight' => 1,
    );
  if (!empty($field['settings']['todate'])) {
    $form['default_value2'] = array(
      '#type' => 'select', '#title' => t('Default value for To date'),
      '#default_value' => $settings['default_value2'],
      '#options' => array('same' => t('Same as From date'), 'blank' => t('Blank'), 'now' => t('Now'), 'strtotime' => t('Relative')),
      '#description' => t("A default value to use for this field. If you select 'Relative', add details below."),
      '#weight' => 2,
      );
  }
  $form['default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customize Default Value'),
    '#description' => '<p>' . t("The custom value for a Relative default should be something that describes a time by reference to the current day using strtotime, like '+90 days' (90 days from the day the field is created) or '+1 Saturday' (the next Saturday). See !strtotime for more details.", array('!strtotime' => l(t('strtotime'), 'http://www.php.net/manual/en/function.strtotime.php'))) . '</p>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 3,
    );
  $form['default']['default_value_code'] = array(
    '#type' => 'textfield', '#title' => t('Custom default value'),
    '#default_value' => $settings['default_value_code'],
    );
  if (!empty($field['settings']['todate'])) {
    $form['default']['default_value_code2'] = array(
      '#type' => 'textfield', '#title' => t('Custom default value for To date'),
      '#default_value' => $settings['default_value_code2'],
      );
  }
  
  $format_types = array();  
  foreach (_system_date_format_types_build() as $name => $info) {
    $format_types[$name] = $info['title'];
  }
  
  $form['default_format'] = array(
    '#type' => 'select', 
    '#title' => t('Default Display'),
    '#default_value' => $settings['default_format'],
    '#options' => $format_types,
    '#description' => t('Select a default format type to be used for the date display. Visit the <a href="@date-time-page">Date and time date format page</a> to add and edit format types.', array('@date-time-page' => base_path() . 'admin/config/regional/date-time/formats')),
    );

  if (module_exists('date_repeat') && date_is_repeat_field($field, $instance)) {
    $form['repeat_collapsed'] = array(
      '#type' => 'radios',
      '#default_value' => $settings['repeat_collapsed'],
      '#options' => array(0 => t('Expanded'), 1 => t('Collapsed')),
      '#title' => t('Repeat display'),
      '#description' => t("Should the repeat options form start out expanded or collapsed? Set to 'Collapsed' to make those options less obtrusive."),
      );
  }
    
  return $form;
}

function date_field_instance_settings_form_validate(&$form, &$form_state) {
  $settings = $form_state['values']['instance']['settings'];
  if ($settings['default_value'] == 'strtotime') {
    $is_strtotime = @strtotime($settings['default_value_code']);
    if (!$is_strtotime) {
      form_set_error('instance][settings][default_value_code', t('The Strtotime default value is invalid.'));
    }
  }
  if ($settings['default_value2'] == 'strtotime') {
    $is_strtotime = @strtotime($settings['default_value_code2']);
    if (!$is_strtotime) {
      form_set_error('instance][settings][default_value_code2', t('The Strtotime default value for the To Date is invalid.'));
    }
  }
}

function _date_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $widget['settings'];
  
  $form = array(
    '#element_validate' => array('date_field_widget_settings_form_validate'),
    );

  $options = array();
  if ($widget['type'] == 'date_popup' && module_exists('date_popup')) {
    $formats = date_popup_formats();
  }
  else {
    // example input formats must show all possible date parts, so add seconds.
    $formats = str_replace('i', 'i:s', array_keys(system_get_date_formats('short')));
    $formats = drupal_map_assoc($formats);
  }
  $now = date_example_date();
  foreach ($formats as $f) {
    $options[$f] = date_format_date($now, 'custom', $f);
  }
  $form['input_format'] = array(
    '#type' => 'select', '#title' => t('Input format'),
    '#default_value' => $settings['input_format'],
    '#options' => $options,
    '#description' => t('Set the order and format for the date parts in the input form. The format will be adapted to remove values not in the granularity for this field.'),
    '#weight' => 4,
    
  );
  
  // Only a limited set of formats is available for the Date Popup module
  if ($widget['type'] != 'date_popup') {
    $form['format'] = array(
      '#type' => 'fieldset',
      '#title' => t('Customize Input Format'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 5,
    );
    $form['format']['input_format_custom'] = array(
      '#type' => 'textfield',  
      '#title' => t('Custom input format'),
      '#default_value' => $settings['input_format_custom'],
      '#description' => t("The custom format, if provided, will override the input format selected above. The custom format, if provided, will override the selected display or input options. Define a php date format string like 'm-d-Y H:i' (see <a href=\"@link\">http://php.net/date</a> for more details).", array('@link' => 'http://php.net/date')),
    );
  }
  else {
    $form['format']['input_format_custom'] = array(
      '#type' => 'hidden',
      '#value' => '',
      );
  }
  if (in_array($widget['type'], array('date_select', 'date_popup', 'date_select_repeat', 'date_popup_repeat'))) {
    $form['year_range'] = array(
      '#type' => 'textfield',
      '#title' => t('Years back and forward'),
      '#default_value' => $settings['year_range'],
      '#size' => 10,
      '#maxsize' => 10,
      '#description' => t('Number of years to go back and forward in the year selection list, default is -3:+3.'),
      '#weight' => 6,
      );
    $form['increment'] = array(
      '#type' => 'select', '#title' => t('Time increment'),
      '#default_value' => $settings['increment'],
      '#options' => array(1 => 1, 5 => 5, 10 => 10, 15 => 15, 30 => 30),
      '#description' => t('Increment the minute and second fields by this amount.'),
      '#weight' => 7,
    );
  }
  else {
    $form['increment'] = array(
      '#type' => 'hidden',
      '#value' => $settings['increment'],
      );
    $form['year_range'] = array(
      '#type' => 'hidden',
      '#value' => $settings['year_range'],
      );
  }

  $form['label_position'] = array(
    '#type' => 'value',
    '#value' => $settings['label_position'],
    );
  $form['text_parts'] = array(
    '#type' => 'value',
    '#value' => $settings['text_parts'],
    );
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customize Date Parts'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 8,
    );
  $form['advanced']['label_position'] = array(
    '#type' => 'radios',
    '#options' => array('above' => t('Above'), 'within' => t('Within'), 'none' => t('None')),
    '#default_value' => $settings['label_position'],
    '#title' => t('Position of date part labels'),
    '#description' => t("The location of date part labels, like 'Year', 'Month', or 'Day' . 'Above' will display them as titles above each date part. 'Within' will insert the label as the first option in the select list and in blank textfields. 'None' will not label any of the date parts. The exact text in the label is controlled by themes like 'date_part_label_year' and 'date_part_label_month' ."),
    );
  $form['advanced']['text_parts'] = array(
    '#theme' => $widget['type'] == 'date_select' ? 'date_text_parts' : '',
    );
  $text_parts = (array) $settings['text_parts'];
  foreach (date_granularity_names() as $key => $value) {
    if ($widget['type'] == 'date_select') {
      $form['advanced']['text_parts'][$key] = array(
        '#type' => 'radios',
        '#default_value' => in_array($key, $text_parts) ? 1 : 0,
        '#options' => array(0 => '', 1 => ''),
        );
    }
    else {
      $form['advanced']['text_parts'][$key] = array(
        '#type' => 'value',
        '#value' => in_array($key, (array) $settings['text_parts']) ? 1 : 0,
        );
    }
  }
  return $form;
}

function date_field_widget_settings_form_validate(&$form, &$form_state) {
  $widget = $form_state['values']['instance']['widget'];
  if (isset($widget['settings']['advanced']['label_position'])) {
    form_set_value($form['label_position'], $widget['settings']['advanced']['label_position'], $form_state);
  }
  // Munge the table display for text parts back into an array of text parts.
  if (is_array($widget['settings']['advanced']['text_parts'])) {
    form_set_value($form['text_parts'], array_keys(array_filter($widget['settings']['advanced']['text_parts'])), $form_state);
  }
  if ($widget['type'] == 'date_select' && !preg_match('@\-[0-9]*:[\+|\-][0-9]*@', $widget['settings']['year_range'])) {
    form_set_error('instance][widget][settings][year_range', t('Years back and forward must be in the format -9:+9.'));
  }
}

function _date_field_schema($field) {
  $db_columns = array();
  if ($field['type'] == 'date') {
    $db_columns['value'] = array('type' => 'varchar', 'length' => 20, 'not null' => FALSE, 'sortable' => TRUE, 'views' => TRUE);
  }
  elseif ($field['type'] == 'datestamp') {
    $db_columns['value'] = array('type' => 'int', 'not null' => FALSE, 'sortable' => TRUE, 'views' => TRUE);
  }
  elseif ($field['type'] == 'datetime') {
    $db_columns['value'] = array('type' => 'datetime', 'not null' => FALSE, 'sortable' => TRUE, 'views' => TRUE);
  }
  // If a second date is needed for 'To date', just make a copy of the first one.
  if (!empty($field['settings']['todate'])) {
    $db_columns['value2'] = $db_columns['value'];
    
    // We don't want CCK to create additional columns, just the first.
    // We modify them our own way in views data.
    $db_columns['value2']['views'] = FALSE;
  }
  // timezone and offset columns are used only if date-specific dates are chosen.
  if (isset($field['settings']['tz_handling']) && $field['settings']['tz_handling'] == 'date') {
    $db_columns['timezone'] = array('type' => 'varchar', 'length' => 50, 'not null' => FALSE, 'sortable' => TRUE, 'views' => FALSE);
    $db_columns['offset'] = array('type' => 'int', 'not null' => FALSE, 'sortable' => TRUE, 'views' => FALSE);
    if (!empty($field['settings']['todate'])) $db_columns['offset2'] = array('type' => 'int', 'not null' => FALSE, 'sortable' => TRUE, 'views' => FALSE);
  }
  if (isset($field['settings']['repeat']) && $field['settings']['repeat'] == 1) {
    $db_columns['rrule'] = array('type' => 'text', 'not null' => FALSE, 'sortable' => FALSE, 'views' => FALSE);
  }
  return array('columns' => $db_columns);  
}

function _date_field_settings_form($field, $instance, $has_data) {
  $settings = $field['settings'];
  
  $form = array(
    '#element_validate' => array('date_field_settings_validate'),
  );
  // Make sure granularity is in the right format and has no empty values.
  if (!empty($settings['granularity']) && is_array($settings['granularity'])) {
    $granularity = array_filter($settings['granularity']);  
  }
  $tz_handling = $settings['tz_handling'];
  
  // If adding a repeat, override the Content module's handling of the multiple values option.
  if (module_exists('date_repeat') && date_is_repeat_field($field, $instance)) {
    $form['repeat'] = array('#type' => 'hidden', '#value' => 1);
  }
  else {
    $form['repeat'] = array('#type' => 'hidden', '#value' => 0);
  }

  $description = t("Display a matching second date field as a 'To date' . If marked 'Optional' field will be presented but not required. If marked 'Required' the 'To date' will be required if the 'From date' is required or filled in.");
  $form['todate'] = array(
    '#type' => 'select', 
    '#title' => t('To Date'),
    '#options' => array('' => t('Never'), 'optional' => t('Optional'), 'required' => t('Required')),
    '#description' => $description,
    '#default_value' => $settings['todate'],
    '#disabled' => $has_data,
    );
  $description = t('Set the date elements to be stored (at least a year is required).');
  $form['granularity'] = array(
    '#type' => 'select', 
    '#title' => t('Granularity'),
    '#default_value' => $granularity,
    '#options' => date_granularity_names(),
    '#multiple' => TRUE,
    '#description' => $description,
    '#disabled' => $has_data,
    );
  $description = t('Select the timezone handling method to be used for this date field.');
  $form['tz_handling'] = array(
    '#type' => 'select',
    '#title' => t('Time zone handling'),
    '#default_value' => $tz_handling,
    '#options' => date_timezone_handling_options(),
    '#description' => $description,
    '#disabled' => $has_data,
  );
  // Force this value to hidden because we don't want to allow it to be changed right now,
  // but allow it to be a variable if needed.
  $form['timezone_db'] = array(
    '#type' => 'hidden',
    '#value' => date_get_timezone_db($tz_handling),
    );
  

  return $form;
}

/**
 * Custom field settings manipulation.
 */
function date_field_settings_validate(&$form, &$form_state) {
  $field = $form_state['values']['field'];
  if ($field['settings']['tz_handling'] == 'none') {
    form_set_value($form['timezone_db'], '', $form_state);
  }
  else {
    form_set_value($form['timezone_db'], date_get_timezone_db($field['settings']['tz_handling']), $form_state);
  }
  if (!in_array('year', $field['settings']['granularity'])) {
    form_set_error('field][settings][granularity', t('Granularity must include a year.'));
  }
  if ($field['settings']['tz_handling'] != 'none' && !in_array('hour', array_filter($field['settings']['granularity']))) {
    form_set_error('field][settings][tz_handling', t('Dates without hours granularity must not use any timezone handling.'));
  }  
}

/**
 *  Timezone handling options
 *
 *  the 'none' option will do no timezone conversions and will store and display dates exactly as entered
 *  useful in locales or situations where timezone conversions are not working reliably,
 *  for dates with no times, for historical dates where timezones are irrelevant,
 *  or anytime conversion is unnecessary or undesirable
 */
function date_timezone_handling_options() {
  return array(
    'site' => t("Site's time zone"),
    'date' => t("Date's time zone"),
    'user' => t("User's time zone"),
    'utc' => 'UTC',
    'none' => t('No time zone conversion'),
    );
}

/**
 * Get an example date and make sure the difference between
 * month and day and 12 and 24 hours will be clear.
 */
function date_example_date() {
  $now = date_now();
  if (date_format($now, 'm') == date_format($now, 'd')) {
    date_modify($now, '+1 day');
  }
  if (date_format($now, 'H') == date_format($now, 'h')) {
    date_modify($now, '+12 hours');
  }
  return $now;
}

/**
 * Formatter settings.
 * 
 * Form element used both in the date_handler_field_multiple Views handler
 * and on the CCK Display fields page.
 */
function _date_formatter_settings($form_state = NULL, $field, $options = array(), $views_form = FALSE) {
  $field_name = $field['field_name'];
  $field = field_info_field($field_name);
  $bundle = isset($options['bundle']) ? $options['bundle'] : $field['bundle'];
  $context = isset($options['context']) ? $options['context'] : 'full';
  if (empty($options['fromto'])) {
    $options = date_formatter_get_settings($field_name, $bundle, $context);
  }
  
  $form = array();
  $form['fromto'] = array(
    '#access' => $field['settings']['todate'],
    '#weight' => 5,
    );
  if (isset($options['fromto']) && isset($options['fromto']['fromto'])) {
    $default = $options['fromto']['fromto'];
  }
  else {
    $default = 'both';
  }
  $form['fromto']['fromto'] = array(
    '#type' => 'select',
    '#options' => array(
      'both' => t('Display From and To dates'), 
      'value' => t('Display From date only'), 
      'value2' => t('Display To date only'),
      ),
    '#default_value' => $default,
    '#weight' => 1,
  );  

  $form['multiple'] = array(
    '#access' => $field['cardinality'] == FIELD_CARDINALITY_UNLIMITED ? 1 : $field['cardinality'],
    '#weight' => 6,
  );

  // Make the string translatable by keeping it as a whole rather than
  // translating prefix and suffix separately.
  if (isset($options['multiple']) && isset($options['multiple']['multiple_number'])) {
    $default = $options['multiple']['multiple_number'];    
  }
  else {
    $default = '';
  }
  list($prefix, $suffix) = explode('@count', t('Show @count value(s)'));
  $form['multiple']['multiple_number'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#field_prefix' => theme('advanced_help_topic', 'date_api', 'date-display') . $prefix,
    '#field_suffix' => $suffix,
    '#default_value' => $default,
    '#weight' => 1,
  );
  if ($views_form) {
    $form['multiple']['multiple_number'] += array(
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array(TRUE)),
    );
  }
  
  if (isset($options['multiple']) && isset($options['multiple']['multiple_from'])) {
    $default = $options['multiple']['multiple_from'];    
  }
  else {
    $default = '';
  }
  list($prefix, $suffix) = explode('@count', t('starting from @count'));
  $form['multiple']['multiple_from'] = array(
    '#type' => 'textfield',
    '#size' => 15,
    '#field_prefix' => $prefix,
    '#field_suffix' => $suffix,
    '#default_value' => $default,
    '#weight' => 2,
  );
  if ($views_form) {
    $form['multiple']['multiple_from'] += array(
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array(TRUE)),
    );
  }

  if (isset($options['multiple']) && isset($options['multiple']['multiple_to'])) {
    $default = $options['multiple']['multiple_to'];    
  }
  else {
    $default = '';
  }
  list($prefix, $suffix) = explode('@count', t('ending on @count'));
  $form['multiple']['multiple_to'] = array(
    '#type' => 'textfield',
    '#size' => 15,
    '#field_prefix' => $prefix,
    '#field_suffix' => $suffix,
    '#default_value' => $default,
    '#weight' => 3,
  );
  if ($views_form) {
    $form['multiple']['multiple_to'] += array(
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-multiple-group' => array(TRUE)),
    );
  }
  
  $form['repeat'] = array(
    '#access' => $field['settings']['repeat'],
    '#weight' => 7,
  );
  if (isset($options['repeat']) && isset($options['repeat']['show_repeat_rule'])) {
    $default = $options['repeat']['show_repeat_rule'];
  }
  else {
    $default = 'show';
  }
  $form['repeat']['show_repeat_rule'] = array(
    '#type' => 'select',
    '#options' => array(
      'show' => t('Display repeat rule'), 
      'hide' => t('Hide repeat rule')),
    '#default_value' => $default,
  );

  if (!$views_form) {
    $form['field'] = array(
      '#type' => 'value',
      '#value' => $field,
      );
    $form['bundle'] = array(
      '#type' => 'value',
      '#value' => $bundle,
      );  
    $form['context'] = array(
      '#type' => 'value',
      '#value' => $context,
      );  
  }
  return $form;
}