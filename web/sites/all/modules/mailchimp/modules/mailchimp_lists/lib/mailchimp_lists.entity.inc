<?php

class MailchimpList extends Entity {
  public
    $id,
    $name,
    $mc_list_id,
    $label,
    $description,
    $settings;

  /**
   * Override parent constructor to set the entity type.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'mailchimp_list');
  }

  /**
   * Return a label for a list.
   */
  public function label() {
    return $this->label;
  }

}
