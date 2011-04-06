<?php

/**
 * Implements hook_html_head_alter().
 */
function apress_html_head_alter(&$head_elements) {
  // Force the latest IE rendering engine and Google Chrome Frame.
  $head_elements['apress_edge_chrome'] = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array('http-equiv' => 'X-UA-Compatible', 'content' => 'IE=edge,chrome=1'),
  );
  // Add an Apple touch icon.
  $head_elements['apress_apple_touch_icon'] = array(
    '#type' => 'html_tag',
    '#tag' => 'link',
    '#attributes' => array(
      'rel' => 'apple-touch-icon-precomposed', // No gradient.
      // 'rel' => 'apple-touch-icon', // Automatic gradient.
      'href' => path_to_theme() . '/images/apple-touch-icon.png',
    ),
  );
  $head_elements['apress_viewport'] = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
      'name' => 'viewport',
      'content' => 'width=device-width, initial-scale=1.0',
    ),
  );
}

/**
 * Implements hook_css_alter().
 */
function apress_css_alter(&$css) {
  // Remove system.theme.css & system.menus.css.
  unset($css['modules/system/system.theme.css']);
  unset($css['modules/system/system.menus.css']);
  unset($css['modules/system/system.messages.css']);
}

/**
 * Implements hook_page_alter().
 */
function apress_page_alter(&$page) {
  // Remove the block template wrapper from the main content block.
  if (!empty($page['content']['system_main'])) {
    $page['content']['system_main']['#theme_wrappers'] = array_diff($page['content']['system_main']['#theme_wrappers'], array('block'));
  }
  // Add the breadcrumbs to the bottom of the footer region.
  $page['footer']['breadcrumbs'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('breadcrumb-wrapper', 'clearfix')),
    '#weight' => 10,
  );
  $page['footer']['breadcrumbs']['breadcrumb'] = array(
    '#theme' => 'breadcrumb',
    '#prefix' => '<div class="breadcrumb-wrapper clearfix">',
    '#breadcrumb' => drupal_get_breadcrumb(),
    '#suffix' => '</div>',
    '#weight' => 10,
  );
  // Trigger the contents of the region to be sorted.
  $page['footer']['#sorted'] = FALSE;

  // Move the messages.
  if ($page['#show_messages']) {
    $page['help']['messages'] = array(
      '#markup' => theme('status_messages'),
      '#weight' => -10,
    );
    // Trigger the contents of the region to be sorted.
    $page['help']['#sorted'] = FALSE;
  }

  // Kill the sidebar regions in the admin section.
  if (arg(0) == 'admin') {
    unset($page['sidebar_first']);
    unset($page['sidebar_second']);
  }

  // Do special stuff on the front page.
  if (drupal_is_front_page()) {
    // Remove the sidebars and content regions.
    unset($page['sidebar_first']);
    unset($page['sidebar_second']);
    unset($page['content']);
  }
}

/**
 * Implements hook_menu_local_tasks_alter().
 */
function apress_menu_local_tasks_alter(&$data, $router_item, $root_path) {
  // Add <span class="icon"/> inside action links.
  foreach ($data['actions']['output'] as $key => $link) {
    // Set the link to allow a title with HTML in it.
    $data['actions']['output'][$key]['#link']['localized_options']['html'] = TRUE;
    // When HTML is set to true, the title must be sanitized.
    $title = check_plain($data['actions']['output'][$key]['#link']['title']);
    // Change the title to include the icon markup and sanitized title.
    $data['actions']['output'][$key]['#link']['title'] = '<span class="icon"/></span>' . $title;
  }

  if ($root_path == 'user/%') {
    // Change the first tab title from 'View' to 'My profile'.
    if ($data['tabs'][0]['output'][0]['#link']['title'] == t('View')) {
      $data['tabs'][0]['output'][0]['#link']['title'] = t('Profile');
    }
    // Change the second tab title from 'Edit' to 'Edit profile'.
    if ($data['tabs'][0]['output'][1]['#link']['title'] == t('Edit')) {
      $data['tabs'][0]['output'][1]['#link']['title'] = t('Edit profile');
    }
  }
}

/**
 * Implements template_preprocess_html().
 */
function apress_preprocess_html(&$vars) {
  // Make the theme path available.
  global $theme_path;

  // Set the default options.
  $options = array('group' => CSS_THEME);
  // Set 'preprocess' to 'FALSE' for conditionally loaded stylesheets.
  $conditional = array_merge($options, array('preprocess' => FALSE));
  // Set 'external' to TRUE for external stylesheets.  This is ridiculous, see:
  // http://drupal.org/node/953340
  $external = array_merge($options, array('external' => TRUE));

  // Add external font files.
  drupal_add_css('http://fonts.googleapis.com/css?family=Droid+Serif:regular,italic,bold,bolditalic&subset=latin', $external);
  drupal_add_css('http://fonts.googleapis.com/css?family=Droid+Sans:regular,bold&subset=latin', $external);

  // Add the regular theme stylesheets.
  drupal_add_css($theme_path . '/css/layout.css', $options);
  drupal_add_css($theme_path . '/css/forms.css', $options);
  drupal_add_css($theme_path . '/css/style.css', $options);

  // Add css/overlay.css when the overlay window is in child mode.
  if (module_exists('overlay')) {
    if (overlay_get_mode() == 'child') {
      drupal_add_css($theme_path . '/css/overlay.css', $conditional);
    }
  }
  // Add css/tabs.css if the page as tabs.
  if (menu_primary_local_tasks() || menu_secondary_local_tasks()) {
    drupal_add_css($theme_path . '/css/tabs.css', $conditional);
  }
  // Add css/admin.css in the admin section.
  if (arg(0) == 'admin') {
    drupal_add_css($theme_path . '/css/admin.css', $conditional);
  }

  // Add an IE conditional stylesheet.
  // <link> and <style> combinations should not happen, as it hurts performance
  // in IE. It was fixed in this issue: [#228818] and was somehow reverted in
  // [#769226], I think? The line of code below gives us a <style> tag when
  // aggregation is disabled and a <link> when it's enabled.
  $preprocess = variable_get('preprocess_css', 0) == 0 ? TRUE : FALSE;
  $ie = array_merge($options, array('browsers' => array('IE' => 'lte IE 7', '!IE' => FALSE), 'preprocess' => $preprocess));
  drupal_add_css($theme_path . '/css/ie.css', $ie);

  // Add a class to indicate there's no title for styling the content region.
  if (!drupal_get_title()) {
    $vars['classes_array'][] = 'no-title';
  }
}

/**
 * Implements template_preprocess_field().
 */
function apress_preprocess_field(&$vars, $hook) {
  $element = $vars['element'];

  // Make some changes to taxonomy term reference fields.
  if ($element['#field_type'] == 'taxonomy_term_reference') {
    // Use a different template file for these: field--tags.tpl.php.
    $vars['theme_hook_suggestions'][] = 'field__tags';
    // Start from scratch with the wrapper classes because some of the classes
    // such as the label placement classes will no longer work with the new
    // markup.
    $vars['classes_array'] = array('field', 'field-tags', drupal_html_class($element['#field_name']), 'clearfix');
  }
}

/**
 * Implements template_preprocess_region().
 */
function apress_preprocess_region(&$vars) {
  $region = $vars['region'];
  // Sidebars and content area, need a good class to style against.  We should
  // not be using id's like #main or #main-wrapper to style contents.
  if (in_array($region, array('sidebar_first', 'sidebar_second', 'content'))) {
    $vars['classes_array'][] = 'main';
  }
  // Add a "clearfix" class to certain regions.
  if (in_array($region, array('footer', 'help', 'highlight'))) {
    $vars['classes_array'][] = 'clearfix';
  }
  // Add an "outer" class to the darker regions.
  if (in_array($region, array('header', 'footer', 'sidebar_first', 'sidebar_second'))) {
    $vars['classes_array'][] = 'outer';
  }
}

/**
 * Implements template_preprocess_block().
 */
function apress_preprocess_block(&$vars) {
  // Give the title a decent class.
  $vars['title_attributes_array']['class'][] = 'block-title';
}

/**
 * Implements template_preprocess_node().
 */
function apress_preprocess_node(&$vars) {
  // Give node titles decent classes.
  $vars['title_attributes_array']['class'][] = 'node-title';

  // Remove the "Add new comment" link when the form is below it.
  if (!empty($vars['content']['comments']['comment_form'])) {
    unset($vars['content']['links']['#links']['comment-add']);
  }

  // Make some changes when in teaser mode.
  if ($vars['teaser']) {
    // Don't display author or date information.
    $vars['display_submitted'] = FALSE;
    // Trim the node title and append an ellipsis.
    $vars['title'] = truncate_utf8($vars['title'], 70, TRUE, TRUE);
  }
}

/**
 * Implements template_preprocess_user_picture().
 * - Add "change picture" link to be placed underneath the user image.
 */
function apress_preprocess_user_picture(&$vars) {
  // Create our variable with an empty string to prevent PHP notices when
  // attempting to print the variable
  $vars['edit_picture'] = '';
  // The account object contains the information of the user whose photo is
  // being processed. We compare that to the user id of the user object which
  // represents the currently logged in user.
  if ($vars['account']->uid == $vars['user']->uid) {
    // Create a variable containing a link to the user profile, with a class
    // "change-user-picture" that we'll style against with CSS.
    $vars['edit_picture'] = l('Change picture', 'user/' . $vars['account']->uid . '/edit', array(
      'fragment' => 'edit-picture',
      'attributes' => array('class' => array('change-user-picture')),
      )
    );
  }
}

/**
 * Implements template_process_page().
 */
function apress_process_page(&$vars) {
  // Prevent these from printing twice. Set to an empty string to prevent PHP
  // notices.
  $vars['breadcrumb'] = '';
  $vars['messages'] = '';
}

/**
 * Implements hook_form_ID_alter().
 */
function apress_form_comment_form_alter(&$form, &$form_state) {
  // krumo($form);
  $form['notify_settings']['notify_type']['#prefix'] = '<div class="container-inline clearfix">';
  $form['notify_settings']['notify_type']['#suffix'] = '</div>';
}

/**
 * Implements hook_form_ID_alter().
 */
function apress_form_user_login_block_alter(&$form, &$form_state) {
  $form['links']['#prefix'] = '<div class="login-links container-inline clearfix">';
  $form['links']['#suffix'] = '</div>';
}

/**
 * Overrides theme_links().
 * Added: <span class="icon"> before links.
 */
function apress_links__node($variables) {
  global $language_url;

  $links = $variables['links'];
  $attributes = $variables['attributes'];
  $heading = $variables['heading'];
  $output = '';

  if ($links) {
    // Prepend the heading to the list, if any.
    if ($heading) {
      // Convert a string heading into an array, using a H2 tag by default.
      if (is_string($heading)) {
        $heading = array('text' => $heading);
      }
      // Merge in default array properties into $heading.
      $heading += array(
        'level' => 'h2',
        'attributes' => array(),
      );
      // @todo D8: Remove backwards compatibility for $heading['class'].
      if (isset($heading['class'])) {
        $heading['attributes']['class'] = $heading['class'];
      }

      $output .= '<' . $heading['level'] . drupal_attributes($heading['attributes']) . '>';
      $output .= check_plain($heading['text']);
      $output .= '</' . $heading['level'] . '>';
    }

    $output .= '<ul' . drupal_attributes($attributes) . '>';

    $num_links = count($links);
    $i = 0;
    foreach ($links as $key => $link) {
      $i++;

      $class = array();
      // Use the array key as class name.
      $class[] = drupal_html_class($key);
      // Add odd/even, first, and last classes.
      $class[] = ($i % 2 ? 'odd' : 'even');
      if ($i == 1) {
        $class[] = 'first';
      }
      if ($i == $num_links) {
        $class[] = 'last';
      }
      $item = '';

      // Handle links.
      if (isset($link['href'])) {
        $is_current_path = ($link['href'] == $_GET['q'] || ($link['href'] == '<front>' && drupal_is_front_page()));
        $is_current_language = (empty($link['language']) || $link['language']->language == $language_url->language);
        if ($is_current_path && $is_current_language) {
          $class[] = 'active';
        }
        // Pass in $link as $options, they share the same keys.
        $item = '<span class="icon"></span>' . l($link['title'], $link['href'], $link);
      }
      // Handle title-only text items.
      else {
        // Merge in default array properties into $link.
        $link += array(
          'html' => FALSE,
          'attributes' => array(),
        );
        $item .= '<span' . drupal_attributes($link['attributes']) . '>';
        $item .= ($link['html'] ? $link['title'] : check_plain($link['title']));
        $item .= '</span>';
      }

      $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';
      $output .= $item;
      $output .= '</li>';
    }

    $output .= '</ul>';
  }

  return $output;
}

/**
 * Overrides theme_breadcrumb().
 */
function apress_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  // It's totally asenine to print just a "home" link directly under a home
  // navigation menu item when that's all there is. Jesus.
  if (count($breadcrumb) > 1) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    $output .= '<div class="breadcrumb">' . implode(' » ', $breadcrumb) . '</div>';
    return $output;
  }
}

/**
 * Overrides theme_status_messages().
 */
function apress_status_messages($variables) {
  $display = $variables['display'];
  $output = '';

  $status_heading = array(
    'status' => t('Status'),
    'error' => t('Error'), 
    'warning' => t('Warning'),
  );
  foreach (drupal_get_messages($display) as $type => $messages) {
    $output .= "<div class=\"messages $type\">\n";
    if (!empty($status_heading[$type])) {
      $output .= '<h2 class="message-title"><span class="icon"></span>' . $status_heading[$type] . "</h2>\n";
    }
    $output .= '<div class="content">';
    if (count($messages) > 1) {
      $output .= "<ul>\n";
      foreach ($messages as $message) {
        $output .= '  <li>' . $message . "</li>\n";
      }
      $output .= " </ul>\n";
    }
    else {
      $output .= $messages[0];
    }
    $output .= "</div>\n";
    $output .= "</div>\n";
  }
  return $output;
}
