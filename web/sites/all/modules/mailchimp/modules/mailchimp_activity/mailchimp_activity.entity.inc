<?php

class MailchimpActivityEntity extends Entity {
  public
    $mailchimp_activity_entity_id,
    $name,
    $label,
    $entity_type,
    $bundle,
    $email_property,
    $entity_path,
    $enabled;

  /**
   * Override constructor to set entity type.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'mailchimp_activity_entity');
  }

  public function label() {
    return $this->label;
  }

}
