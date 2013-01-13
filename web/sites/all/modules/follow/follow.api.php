<?php

/**
 * Alter the available networks to the Follow module.
 *
 * @param $networks
 *   Associative array of networks that are available.
 * @param $uid
 *   The User ID of the networks to be displayed. If 0 is provided, will be the
 *   networks for the website rather then an individual user.
 */
function hook_follow_networks_alter(&$networks, $uid = 0) {
  // Add a network.
  $networks[$uid]['mailinglist'] = array(
    'title' => t('Mailing List'),
    'domain' => 'mailinglist.domain.com',
  );

  // Replace Twitter with Identi.ca
  unset($networks[$uid]['twitter']);
  $networks[$uid]['identica'] = array(
    'title' => t('Identi.ca'),
    'domain' => 'identi.ca',
  );
}
