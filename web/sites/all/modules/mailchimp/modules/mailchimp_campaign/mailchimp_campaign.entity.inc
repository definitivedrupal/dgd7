<?php

/**
 * @file
 * Entity API hooks for mailchimp_campaign module.
 */

/**
 * Implements hook_entity_info().
 */
function mailchimp_campaign_entity_info() {
  $return = array(
    'mailchimp_campaign' => array(
      'label' => t('MailChimp Campaign'),
      'controller class' => 'MailChimpCampaignController',
      'entity class' => 'MailChimpCampaign',
      'base table' => 'mailchimp_campaigns',
      'uri callback' => 'mailchimp_campaign_uri',
      'fieldable' => FALSE,
      'entity keys' => array(
        'id' => 'mc_campaign_id',
      ),
      'token type' => 'mailchimp_campaign',
      'module' => 'mailchimp_campaign',
      'label callback' => 'entity_class_label',
      'view modes' => array(
        'full' => array(
          'label' => t('Full Campaign'),
          'custom settings' => FALSE,
        ),
      ),
    ),
  );
  return $return;
}

/**
 * Saves a campaign.
 */
function mailchimp_campaign_save(MailChimpCampaign $campaign) {
  $campaign->updated = REQUEST_TIME;
  // Set the creation timestamp if not set.
  if (!isset($campaign->created) || empty($campaign->created)) {
    $campaign->created = REQUEST_TIME;
  }

  return $campaign->save();
}

/**
 * Load a campaign by ID.
 */
function mailchimp_campaign_load($cid, $reset = FALSE) {
  $campaign = mailchimp_campaign_load_multiple(array($cid), array(), $reset);
  return $campaign ? $campaign[$cid] : FALSE;
}

/**
 * Loads multiple campaigns.
 */
function mailchimp_campaign_load_multiple($campaign_ids = array(), $conditions = array(), $reset = FALSE) {
  if (empty($campaign_ids)) {
    $campaign_ids = FALSE;
  }
  $campaigns = entity_load('mailchimp_campaign', $campaign_ids, $conditions, $reset);

  return $campaigns;
}

/**
 * Delete a campaign.
 */
function mailchimp_campaign_delete($cid) {
  return mailchimp_campaign_delete_multiple(array($cid));
}

/**
 * Delete multiple campaigns.
 */
function mailchimp_campaign_delete_multiple($campaign_ids) {
  return entity_get_controller('mailchimp_campaign')->delete($campaign_ids);
}

/**
 * Display a campaign.
 *
 * @param object $campaign
 *   A fully loaded campaign object.
 *
 * @return array
 *   Renderable elements.
 */
function mailchimp_campaign_view($campaign) {
  return $campaign->view();
}

/**
 * Defines MailChimpCampaign entities
 */
class MailChimpCampaign extends Entity {
  public $mc_campaign_id, $template, $created, $updated, $mc_data, $list, $mc_template;

  public function __construct(array $values = array(), $entityType = 'mailchimp_campaign') {
    parent::__construct($values, $entityType);
  }

  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $build = parent::buildContent($view_mode, $langcode);

    // prepare rendered content
    $content = mailchimp_campaign_render_template($this->template);
    $rendered = '';
    foreach ($content as $key => $section) {
      $rendered .= "<h3>$key</h3>" . $section;
    }

    // get the template name
    $mc_template = mailchimp_campaign_get_templates($this->mc_data['template_id']);
    $mc_template_name = isset($mc_template) ? $mc_template['name'] : '';

    $fields = array(
      'subject' => array(
        'label' => t('Subject'),
        'value' => $this->mc_data['subject']
      ),
      'list' => array(
        'label' => t('MailChimp List'),
        'value' => l($this->list['name'], 'https://admin.mailchimp.com/lists/dashboard/overview?id=' . $this->list['web_id'], array(
          'attributes' => array('target' => '_blank'),
        ))
      ),
      'from_email' => array(
        'label' => t('From Email'),
        'value' => $this->mc_data['from_email']
      ),
      'from_name' => array(
        'label' => t('From Name'),
        'value' => $this->mc_data['from_name']
      ),
      'template' => array(
        'label' => t('Template'),
        'value' => $mc_template_name
      ),
      'type' => array(
        'label' => t('List type'),
        'value' => $this->mc_data['type']
      ),
      'status' => array(
        'label' => t('Status'),
        'value' => $this->mc_data['status']
      ),
      'emails_sent' => array(
        'label' => t('Emails sent'),
        'value' => $this->mc_data['emails_sent']
      ),
      'send_time' => array(
        'label' => t('Send time'),
        'value' => $this->mc_data['send_time']
      ),
      'content' => array(
        'label' => t('Rendered template HTML (!archive)',
          array(
            '!archive' => l('View MailChimp archive', $this->mc_data['archive_url'], array(
              'attributes' => array('target' => '_blank'),
            ))
          )),
        'value' => $rendered
      )
    );

    foreach ($fields as $key => $field) {
      $build[$key] = array(
        '#prefix' => "<div class=\"field campaign-{$key}\"><div class=\"field-label\">{$field['label']}</div>",
        '#markup' => $field['value'],
        '#suffix' => '</div>',
      );
    }

    return $build;
  }

  protected function defaultLabel() {
    return $this->mc_data['title'];
  }

}

/**
 * MailChimpCampaign controller class.
 */
class MailChimpCampaignController extends EntityAPIController {

  /**
   * Implement in order to attach MC data to campaign entities.
   *
   * @param $campaigns
   * @param bool $revision_id
   */
  protected function attachLoad(&$campaigns, $revision_id = FALSE) {
    if (!empty($campaigns)) {
      foreach ($campaigns as $campaign) {
        $ids[] = $campaign->mc_campaign_id;
      }
    }
    $mc_campaigns = mailchimp_campaign_get_campaigns($ids);

    foreach ($campaigns as $mc_campaign_id => $campaign) {
      $campaign->mc_data = $mc_campaigns[$mc_campaign_id];

      // lists are cachced separately, but we want to load them here.
      if (isset($campaign->mc_data['list_id']) && $campaign->mc_data['list_id']) {
        $campaign->list = mailchimp_get_list($campaign->mc_data['list_id']);
      }

      if (isset($campaign->mc_data['template_id']) && $campaign->mc_data['template_id']) {
        $campaign->mc_template = mailchimp_campaign_get_templates($campaign->mc_data['template_id']);
      }
    }

    parent::attachLoad($campaigns, $revision_id);
  }

  /**
   * Implement to clear MC campaign data caches.
   *
   * @param array|null $ids
   */
  public function resetCache(array $ids = NULL) {
    parent::resetCache($ids);
    if ($cached_campaigns = cache_get('mailchimp_campaigns')) {
      foreach ($cached_campaigns->data as $id => $cached_campaign) {
        if (in_array($id, $ids)) {
          unset($cached_campaigns->data[$id]);
        }
      }
      cache_set('mailchimp_campaign', $cached_campaigns);
    }

    drupal_static_reset('mailchimp_campaign_get_template');
  }

}
