<?php
// $Id: simplenews_handler_field_category_name.inc,v 1.1 2010/03/29 21:13:44 sutharsan Exp $

/**
 * @file
 * Views handler for simplenews newsletter category name.
 */

/**
 * Display newsletter sent status.
 */
class simplenews_handler_field_category_name extends views_handler_field {
  /**
   * Constructor to provide additional field to add.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['scid'] = 'scid';
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_category'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Provide link to taxonomy option
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_category'] = array(
      '#title' => t('Link this field to its category page'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_category']),
    );
  }

  /**
   * Render whatever the data is as a link to the category.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_to_category']) && !empty($values->{$this->aliases['scid']}) && $data !== NULL && $data !== '') {
      $category = new stdClass();
      $category->scid = $values->{$this->aliases['scid']};
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = simplenews_category_path($category);
    }
    return $data;
  }

  function render($values) {
    return $this->render_link(check_plain($values->{$this->field_alias}), $values);
  }
}
