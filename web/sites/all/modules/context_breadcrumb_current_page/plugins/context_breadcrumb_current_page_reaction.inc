<?php
/**
 * @file
 * Define class and override.
 */

class context_breadcrumb_current_page_reaction extends context_reaction {

  /**
   * Prevent editor form.
   */
  function editor_form($context) {
  }

  /**
   * Add options.
   */
  function options_form($context) {
    $values = $this->fetch_from_context($context);
    $form = array(
      'breadcrumb_show_current_page' => array(
        '#title' => t('Show the Current Page in the Breadrumbs'),
        '#description' => t('Have the last element in the breadcrumbs be the current page title.'),
        '#type' => 'checkbox',
        '#default_value' => isset($values['breadcrumb_show_current_page']) ? $values['breadcrumb_show_current_page'] : '',
      ),
      'breadcrumb_show_current_page_title' => array(
        '#title' => t('Title Override'),
        '#description' => t('Show this as the breadcrumb title'),
        '#type' => 'textfield',
        '#default_value' => isset($values['breadcrumb_show_current_page_title']) ? $values['breadcrumb_show_current_page_title'] : '',
      ),
      'breadcrumb_show_current_page_only' => array(
        '#title' => t('Remove parent Breadcurmbs'),
        '#description' => t('Have the only item in the breadcrumbs be the title.'),
        '#type' => 'checkbox',
        '#default_value' => isset($values['breadcrumb_show_current_page_only']) ? $values['breadcrumb_show_current_page_only'] : '',
      ),
    );
    return $form;
  }

  /**
   * Set the breadcrumb.
   */
  function execute(&$vars) {
    foreach ($this->get_contexts() as $k => $v) {
      if ((isset($v->reactions[$this->plugin])) &&
          ($v->reactions[$this->plugin]['breadcrumb_show_current_page'])) {
        $breadcrumb = drupal_get_breadcrumb();
        if ($v->reactions[$this->plugin]['breadcrumb_show_current_page_only']) {
          $breadcrumb = array($breadcrumb[0]);
        }
        $title =
          $v->reactions[$this->plugin]['breadcrumb_show_current_page_title']?
          $v->reactions[$this->plugin]['breadcrumb_show_current_page_title']:
          drupal_get_title();

        $breadcrumb[] = $title;
        drupal_set_breadcrumb($breadcrumb);
      }
    }
  }
}
