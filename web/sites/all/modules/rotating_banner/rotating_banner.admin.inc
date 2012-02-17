<?php

/**
 * @see rotating_banner_slide_form().
 */
function rotating_banner_slide_add($form, &$form_state, $banner) {
  return rotating_banner_slide_form($form, $form_state, $banner);
}

/**
 * @see rotating_banner_slide_form().
 */
function rotating_banner_slide_edit($form, &$form_state, $slide) {
  $banner = rotating_banner_load($slide->rbid);
  drupal_set_title('Editing slide');
  // Get the banner this slide belongs to
  return rotating_banner_slide_form($form, $form_state, $banner, $slide);
}

/**
 * Form callback to edit a rotating banner slide.
 * 
 * @global <type> $base_url
 * @param <type> $form
 * @param <type> $form_state
 * @param <type> $banner
 * @param <type> $slide
 * @return <type>
 */
function rotating_banner_slide_form($form, &$form_state, $banner, $slide = NULL) {
  global $base_url;
  $slide_defaults = RotatingBannerSlide::getDefaultSettings();
  include_once drupal_get_path('module', 'media') . '/wysiwyg_plugins/media.inc';
  media_include_browser_js();

  drupal_add_js(array('rotatingBanner' => $banner->settings), 'setting');

  $form['#id'] = 'rotating-banner-slide-form';
  $form['#submit'][] = 'rotating_banner_slide_form_submit';

  // Add assets
  $path = drupal_get_path('module', 'rotating_banner');
  $form['#attached']['js'][] = $path . '/rotating_banner.admin.js';
  $form['#attached']['js'][] = $path . '/json2.js';
  $form['#attached']['css'][] = $path . '/rotating_banner.css';
  $form['#attached']['css'][] = $path . '/rotating_banner.admin.css';

  $form['rbid'] = array(
    '#type' => 'value',
    '#value' => $banner->rbid,
  );

  if (isset($slide)) {
    $form['sid'] = array(
      '#type' => 'value',
      '#value' => $slide->sid
    );
  }

  $form['fid'] = array(
    '#type' => 'hidden',
    '#default_value' => 0,
    // Used by rotating_banner.admin.js.
    '#attributes' => array('id' => 'edit-fid'),
  );

$link_text = <<<EOF
<ul class="action-links">
  <li><a href="#" class="choose-image-link">Choose banner image</a></li>
  <li class="rb-custom-layout-tools"><a href="#" id="rb-add-headline">Add new banner headline</a></li>
  <li><a href="#" class="rb-custom-layout-tools" id="rb-add-text">Add new text area</a></li>

</ul>
EOF;
  
  $form['textboxes'] = array(
    '#type' => 'hidden',
    '#default_value' => '',
    // Used by rotating_banner.admin.js.
    '#attributes' => array('id' => 'edit-textboxes'),
  );
  
  $form['slide'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="form-item rb-slide-preview"><label> Preview </label>',
    '#suffix' => '</div>',
  );

  // Actions the user can take
  $form['links'] = array(
    '#type' => 'markup',
    '#markup' => $link_text,
  );

  $layout_instructions_custom = <<<EOS
  <strong>Instructions:</strong>
    <ol>
    <li> Add as many text areas and headlines as you want using the links at the top of the page.</li>
    <li> Drag them around your slide. </li>
    <li> Double click them to edit (click outside the textbox to save your edit). </li>
    </ol>
EOS;
  $form['layout-instructions-custom'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="rb-custom-layout-tools">',
    '#markup' => t($layout_instructions_custom),
    '#suffix' => '</div>',
  );
  

  $form['layout'] = array(
    '#type' => 'select',
    '#title' => t('Layout'),
    '#description' => t('Where should the headline and text be placed on this slide.'),
    '#options' => module_invoke_all('rotating_banner_slide_layouts'),
    '#default_value' => $slide_defaults['layout'],
  );

  $form['layout-text-editor'] = array(
    '#prefix' => '<div class="layout-text-editor">',
    '#suffix' => '</div>',
    '#tree' => true,
  );

  $form['layout-text-editor']['header'] = array(
    '#type' => 'textfield',
    '#title' => t('Banner headline'),
  );

  $form['layout-text-editor']['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Banner text'),
  );

  $form['link'] = array(
    '#type' => 'textfield',
    '#title' => t('Link'),
    '#description' => t('Where to send the user when this slide is clicked on.  Can be an absolute URL (http://google.com), or a relative URL (node/1).  Use "&lt;front&gt;" for the front page of your site.'),
    '#default_value' => $slide_defaults['link'],
    //'#prefix' => $base_url . '/',
    // change if there is time
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if ($slide) {
    $form['link']['#default_value'] = $slide->link;
    $form['fid']['#default_value'] = $slide->fid;
    $form['layout']['#default_value'] = $slide->layout;
    //$form['textboxes']['#default_value'] = drupal_json_encode($slide->textboxes);
    $image = file_create_url(file_load($slide->fid)->uri);
    // This is totally yuk.  @todo

    $file = file_load($slide->fid);
    $link = $slide->link;
    $textboxes = $slide->textboxes;
    $form['slide'] += array(
      '#theme' => 'rotating_banner_slide',
      '#banner' => $banner,
      '#background_image' => $file,
      '#textboxes' => $textboxes,
      //'#prefix' => '<div class="rotating-banner-slide">',
      //'#suffix'  => '</div>',
    );
    
    //$form['slide']['#prefix'] = '<div class="rotating-banner-slide" style="' . sprintf('background-image: url(%s)', $image) . '">';
  } else {
    $default_textboxes = $slide_defaults['textboxes'];
    $background_image = NULL;
    if (isset($slide_defaults['fid'])) {
      $background_image = file_load($slide_defaults['fid']);
    }

    $form['fid']['#default_value'] = $slide_defaults['fid'];
    
    $form['slide'] += array(
      '#theme' => 'rotating_banner_slide',
      '#banner' => $banner,
      '#textboxes' => $default_textboxes,
      '#background_image' => $background_image,
    );
  }

//  Annoying, this applies a lot of markup I don't want.
//  $form['links'] = array(
//    '#theme' => 'item_list',
//    '#items' => array('<a href="#" id=".choose-image-link">Choose background image</a>', '<a href="#" id=".rb-textbox-text">Add text area</a>'),
//    '#attributes' => array('class' => array('action-links')),
//  );

  //Why isn't this getting called on its own?
  $form['#validate'][] = 'rotating_banner_slide_form_validate';

  
  return $form;
}

function rotating_banner_slide_form_validate($form, &$form_state) {
  if (!$form_state['values']['fid']) {
    form_set_error('', 'You must select a background image');
  }
}

function rotating_banner_slide_form_submit($form, &$form_state) {
  $is_edit = FALSE;
  $values = $form_state['values'];
  $sid = !empty($values['sid']) ? $values['sid'] : NULL;
  
  if (isset($sid)) {
    $rbs = RotatingBannerSlide::get($sid);
  }
  else {
    //@todo: let it work w/o an fid in the DB in case a default is not set.
    $rbs = RotatingBannerSlide::create($values['rbid'], 0, $values['fid']);
  }

  $rbs->setValuesFromArray($values);
  $rbs->save();

  $form_state['redirect'] = 'admin/structure/block/manage/rotating_banner/' . $rbs->rbid . '/configure';
}

function rotating_banner_slide_delete_confirm($form, &$form_state, $slide) {
  return confirm_form(
    array(
      '#slide' => $slide,
    ),
    t('Are you sure you want to delete this slide?'),
    'admin/structure/block/manage/rotating_banner/' . $slide->rbid . '/configure',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function rotating_banner_slide_delete_confirm_submit($form, &$form_state) {
  $slide = $form['#slide'];
  $sid = $slide->sid;
  $rbs = RotatingBannerSlide::get($sid);
  if ($rbs->delete()) {
    drupal_set_message('The selected slide has been deleted.');
  }
  else {
    drupal_set_message('Error deleting slide.', 'error');
  }

  $form_state['redirect'] = 'admin/structure/block/manage/rotating_banner/' . $slide->rbid . '/configure';
}

function rotating_banner_add_form($form, &$form_state) {
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Banner title'),
    '#description' => t('The title is not shown on the public site, just for the administrator to identify this banner.'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create'),
  );

  return $form;
}

function rotating_banner_add_form_submit($form, &$form_state) {
  $rb = RotatingBanner::create($form_state['values']['title']);
  $delta = $rb->rbid;
  $form_state['redirect'] = 'admin/structure/block/manage/rotating_banner/' . $delta . '/configure';
}

function rotating_banner_delete_form($form, &$form_state, $delta) {
  $form['delta'] = array('#type' => 'hidden', '#value' => $delta);
  $rb = RotatingBanner::get($delta);
  $form['info'] = array('#type' => 'hidden', '#value' => $rb->title);

  return confirm_form($form, t('Are you sure you want to delete the block %name?', array('%name' => $rb->title)), 'admin/structure/block', '', t('Delete'), t('Cancel'));
}

function rotating_banner_delete_form_submit($form, &$form_state) {
  db_delete('block')
    ->condition('module', 'rotating_banner')
    ->condition('delta', $form_state['values']['delta'])
    ->execute();
  db_delete('rotating_banners')
    ->condition('rbid', $form_state['values']['delta'])
    ->execute();
  db_delete('rotating_banner_slides')
    ->condition('rbid', $form_state['values']['delta'])
    ->execute();
  db_delete('block_role')
    ->condition('module', 'rotating_banner')
    ->condition('delta', $form_state['values']['delta'])
    ->execute();
  drupal_set_message(t('The block %name has been removed.', array('%name' => $form_state['values']['info'])));
  cache_clear_all();
  $form_state['redirect'] = 'admin/structure/block';
  return;
}
