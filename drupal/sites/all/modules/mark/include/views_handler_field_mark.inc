<?php
// $Id: views_handler_field_mark.inc,v 1.2.4.2 2010/09/17 18:17:40 tristanoneil Exp $

/**
 * Field handler for marks
 */
class views_handler_field_mark extends views_handler_field {

  /**
   * Render the field as a mark link.
   */
  function render($values) {
    $markname = substr($this->field, 5);
    $value = $values->{$this->field_alias};
    $links = array(); 
    $links[$markname] = array(
      'mark' => mark_load($markname),
      'votingapi' => array(
        'entity_type' => $this->table,
        'entity_id' => $value,
        'value_type' => 'mark',
        'value' => 1,
        'tag' => $markname,
      ),
    );
    return theme('mark_link', array('marks' => $links));
  }
}
