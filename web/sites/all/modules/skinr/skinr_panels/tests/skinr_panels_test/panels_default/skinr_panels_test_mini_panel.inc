<?php

$mini = new stdClass();
$mini->disabled = FALSE; /* Edit this to true to make a default mini disabled initially */
$mini->api_version = 1;
$mini->name = 'skinr_panels_test_mini_panel';
$mini->category = '';
$mini->admin_title = 'Skinr Panels Test mini panel';
$mini->admin_description = 'Skinr Panels Test mini panel.';
$mini->requiredcontexts = array();
$mini->contexts = array();
$mini->relationships = array();
$display = new panels_display();
$display->layout = 'onecol';
$display->layout_settings = array();
$display->panel_settings = array(
  'style_settings' => array(
    'default' => NULL,
    'middle' => NULL,
  ),
);
$display->cache = array();
$display->title = '';
$display->uuid = '218f5b97-58f2-44a8-84d8-14155d2c1d5f';
$display->content = array();
$display->panels = array();
  $pane = new stdClass();
  $pane->pid = 'new-c9bf0f8f-e48a-42d7-9a38-7a6760791765';
  $pane->panel = 'middle';
  $pane->type = 'block';
  $pane->subtype = 'system-user-menu';
  $pane->shown = TRUE;
  $pane->access = array();
  $pane->configuration = array(
    'override_title' => 0,
    'override_title_text' => '',
  );
  $pane->cache = array();
  $pane->style = array(
    'settings' => NULL,
  );
  $pane->css = array();
  $pane->extras = array();
  $pane->position = 0;
  $pane->locks = '';
  $pane->uuid = 'c9bf0f8f-e48a-42d7-9a38-7a6760791765';
  $display->content['new-c9bf0f8f-e48a-42d7-9a38-7a6760791765'] = $pane;
  $display->panels['middle'][0] = 'new-c9bf0f8f-e48a-42d7-9a38-7a6760791765';
$display->hide_title = PANELS_TITLE_FIXED;
$display->title_pane = '0';
$mini->display = $display;
