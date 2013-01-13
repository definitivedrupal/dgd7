<?php

/**
 * Override EntityDefaultUIController to customize our menu items.
 */
class MailChimpActivityUIController extends EntityDefaultUIController {

  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage MailChimp Activity entity settings.';
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    return $items;
  }

}
