<?php

class context_bool_field_condition extends context_condition {

  function get_bool_fields() {
    $bool_fields = array();
    foreach(field_info_fields() as $id=>$field) {
      if ($field['type'] == 'list_boolean') {
        $bool_fields[$id] = $field;
      }
    }
    return $bool_fields;
  }
  //we are setting a value for each field
  function condition_values() {
    $values = array();
    $ids = array_keys($this->get_bool_fields());
    foreach ($ids as $id) {
      $values[$id] = $id;
    }
    return $values;
  }

  //we are adding a value option so we can check if a check box is set to a particlar value.
  function options_form($context) {
    $values = $this->fetch_from_context($context, 'options');
    $form = array();
    $form["bool_field_value"] = array(
      '#title' => t('Value'),
      '#description' => t('Field Value must match this'),
      '#type' => 'radios',
      '#options'=> array(0=>'No', 1=>'Yes'),
      '#default_value' => isset($values["bool_field_value"]) ? $values["bool_field_value"] : FALSE,
    );
    return $form;
  }
  function options_form_sumbit($values) {
  }
  
  function execute($node, $view_mode, $langcode) {
    //find all of the contextes 
    foreach ($this->get_contexts() as $k => $v) { 
      // check to see if any of the context use our plugin and if so if 
      // they have set any fields
      if ((isset($v->conditions[$this->plugin])) &&
          ($fields = $v->conditions[$this->plugin]['values'])) {
        // lets go though each field
        foreach($fields as $field) {
          //see if the current node has that field
          if (isset($node->{$field}) && !empty($node->{$field}) ) {
            //TODO: need to work out lang stuff
            if (!isset($node->{$field}[$langcode])) {
              $langcode = 'und';
            }
            // see if the value of the courrent nodes field match the bool option
            // sett in context
            $value = isset($v->conditions[$this->plugin]['options']['bool_field_value']) ? $v->conditions[$this->plugin]['options']['bool_field_value'] : FALSE;
            if ($node->{$field}[$langcode][0]['value'] == $value) {
              $this->condition_met($v, $field);
            }
          }
        }
      }
    }
  }
}
