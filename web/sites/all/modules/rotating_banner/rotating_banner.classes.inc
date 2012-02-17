<?php

class RotatingBannerSlide extends SimpleActiveRecord{
  public $sid;
  public $rbid;
  public $weight;
  public $fid;
  public $link;
  public $layout;
  public $textboxes;

  function initialize() {
    if (is_string($this->textboxes)) {
      $this->textboxes = drupal_json_decode($this->textboxes);
    };
  }

  protected function dbFields() {
    return array('rbid', 'weight', 'fid', 'link', 'layout', 'textboxes');
  }

  public static function getDefaultSettings($key = NULL) {
    //@todo: add variables here.
    $defaults =  array(
      'textboxes' => array(
        array(
          'position' => array('top' => "20", 'left' => '20'),
          'content' => t('Banner Headline'),
          'type' => 'rb-textbox-type-header',
        ),
        array(
          'position' => array('top' => "80", 'left' => '20'),
          'content' => t('Banner text goes here.<br/> <strong> Double click</strong> to edit.'),
          'type' => 'rb-textbox-type-text',
        ),
      ),
      'layout' => variable_get('rotating_banner_default_slide_layout', 'top-left'),
      'link' => '<front>',
      'weight' => 0,
      'fid' => NULL,
    );

    $fid = variable_get('rotating_banner_default_slide_fid', NULL);
    if ($fid) {
      $defaults['fid'] = $fid;
    }

    if ($key) {
      return $defaults[$key];
    }

    return $defaults;
  }

  public static function create($rbid, $weight = NULL, $fid = NULL, $link = NULL, $layout = NULL, $textboxes = NULL) {
    $defaults = RotatingBannerSlide::getDefaultSettings();

    $rbs = new RotatingBannerSlide();
    $rbs->rbid = $rbid;
    $rbs->weight = $weight ? $weight : $defaults['weight'];
    $rbs->fid = $fid ? $fid : $defaults['fid'];
    $rbs->link = $link ? $link : $defaults['link'];
    $rbs->layout = $layout ? $layout : $defaults['layout'];
    $rbs->textboxes = $textboxes ? $textboxes : $defaults['textboxes'];
    $rbs->save();
    return $rbs;
  }

  public function delete() {
    return db_delete('rotating_banner_slides')
      ->condition('sid', $this->sid)
      ->execute();
  }

  function save() {
    if (!$this->rbid) {
      throw new Exception('Unable to save slide, rotating banner ID is required.');
    }

    foreach($this->dbFields() as $field_name) {
      $fields[$field_name] = $this->{$field_name};
    }

    if (!is_string($fields['textboxes'])) {
      $fields['textboxes'] = drupal_json_encode($fields['textboxes']);
    };

    if (!$this->sid) {
      $sid = db_insert('rotating_banner_slides')
        ->fields($fields)
        ->execute();
      $this->sid = $sid;
      if ($this->sid) {
        return TRUE;
      }
    }
    else {
      return (bool)db_update('rotating_banner_slides')
        ->condition('sid', $this->sid)
        ->fields($fields)
        ->execute();
    }

  }

  public static function get($sid) {
    $result = db_select('rotating_banner_slides', 's')
      ->fields('s')
      ->condition('sid', $sid)
      ->range(0,1)
      ->execute()
      ->fetchAll(PDO::FETCH_CLASS, 'RotatingBannerSlide');

    if (!count($result)) {
      return FALSE;
    }
    SimpleActiveRecord::runInitialize($result);
    return array_shift($result);
  }

  public static function getByRotatingBanner($rbid) {
    $result = db_select('rotating_banner_slides', 's')
      ->fields('s')
      ->condition('rbid', $rbid)
      ->orderBy('weight')
      ->execute()
      ->fetchAll(PDO::FETCH_CLASS, 'RotatingBannerSlide');
    SimpleActiveRecord::runInitialize($result);
    return $result;
  }

}

class RotatingBanner extends SimpleActiveRecord {
  public $rbid;
  public $title;
  public $machine_name;
  public $settings;

  protected function dbFields() {
    return array('title', 'machine_name', 'settings');
  }

  public function initialize() {
    if (is_string($this->settings)) {
      $this->settings = unserialize($this->settings);
    };
  }

  public static function create($title, $machine_name = NULL, $settings = NULL) {
    $defaults = RotatingBanner::getDefaultSettings();
    if ($settings) {
      $settings = array_merge($defaults, $settings);
    }
    else {
      $settings = $defaults;
    }
    $rb = new RotatingBanner();
    $rb->title = $title;
    $rb->machine_name = $machine_name ? $machine_name : uniqid('rb_');
    $rb->settings = $settings;
    $rb->save();
    return $rb;
  }

  public static function getDefaultSettings($key = NULL) {
    //@todo: add variables here.
    static $defaults = array(
      'controls' => 'buttons',
      'prev_next' => 0,
      'width' => '',
      'height' => '',
      'fluid' => TRUE,
      'cycle' => array(
        'fx' => 'scrollDown',
        'timeout' => 8000,
        'fit' => 0,
      ),
    );

    if ($key) {
      return $defaults[$key];
    }

    return $defaults;
  }

  /**
   * Get by Id
   * @param int $rbid
   * @return RotatingBanner
   */
  public static function get($rbid) {
    $result = db_select('rotating_banners', 'rb')
      ->fields('rb')
      ->condition('rbid', $rbid)
      ->range(0,1)
      ->execute()
      ->fetchAll(PDO::FETCH_CLASS, 'RotatingBanner');
    
    if (!count($result)) {
      return FALSE;
    }
    SimpleActiveRecord::runInitialize($result);
    return array_shift($result);
  }

  public static function getAll() {
    $result = db_select('rotating_banners', 'rb')
      ->fields('rb')
      ->execute()
      ->fetchAll(PDO::FETCH_CLASS, 'RotatingBanner');

    if (!count($result)) {
      return FALSE;
    }
    SimpleActiveRecord::runInitialize($result);
    return $result;
  }

  public function getSlides() {
    return RotatingBannerSlide::getByRotatingBanner($this->rbid);
  }

  function save() {

    if (!$this->title) {
      throw new Exception('Unable to save rotating banner, title is required');
    }

    foreach($this->dbFields() as $field_name) {
      $fields[$field_name] = $this->{$field_name};
    }

    if (!is_string($fields['settings'])) {
      $fields['settings'] = serialize($fields['settings']);
    };

    if (!$this->rbid) {
      $rbid = db_insert('rotating_banners')
        ->fields($fields)
        ->execute();
      $this->rbid = $rbid;
      if ($this->rbid) {
        return TRUE;
      }
    }
    else {
      db_update('rotating_banners')
        ->condition('rbid', $this->rbid)
        ->fields($fields)
        ->execute();
      return TRUE;
    }
  }
}

class SimpleActiveRecord {

  public function setValuesFromArray($array) {
    foreach($this->dbFields() as $field_name) {
      if (isset($array[$field_name])) {
        $this->{$field_name} = $array[$field_name];
      }
    }
  }

  /**
   * Disclaimer: This is a hack.  The PDO::FETCH_CLASS does not behave as
   * documented.  It is supposed to run the constructor *after* the properties
   * have been assigned and in some versions of PHP, it does that.
   *
   * However, as of http://bugs.php.net/bug.php?id=46292, it does not.  This method
   * receives a list of Objects and runs a ->initialize() method on them.  This
   * is basically another constructor which can be run afterwards.
   */
  public static function runInitialize(&$result) {
    foreach ($result as &$row) {
      $row->initialize();
    }
  }
}