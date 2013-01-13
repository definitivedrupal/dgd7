<?php

class context_no_title_reaction extends context_reaction {

  function editor_form($context) {
  }
  
  // Add option to reactions menu.
  function options_form($context) {
    $values = $this->fetch_from_context($context);
    $form = array(
      'no_title' => array(
        '#title' =>' No Title on Page',
        '#description' => 'Remove the title from the page template (often need if the node template wants to render the title)',
        '#type' => 'checkbox',
        '#default_value' => isset($values['no_title']) ? $values['no_title'] : '',

      ),
    );
    return $form;
  }

  // Hide node title if option is set.
  function execute(&$vars) {
    foreach ($this->get_contexts() as $k => $v) { 
      if ((isset($v->reactions[$this->plugin])) &&
          ($v->reactions[$this->plugin]['no_title'])) {
        $vars['title'] = '';
      }
    }
  }
}
