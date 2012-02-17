  <div class="<?php echo $variables['plus1']['widget_class'];?>">
  <?php if (!$variables['logged_in'] || user_access('vote on content')) { ?>
      <div class="<?php echo $variables['plus1']['message_class'];?>">
  <?php if (!$variables['logged_in'] && !user_access('vote on content')) { ?>
      <small> <?php echo $variables['plus1']['loginvote'];?> </small>
   <?php }
    elseif ($variables['voted']) { ?>
      <?php echo $variables['plus1']['youvoted'];
    }
    elseif (user_access('vote on content')) {
      /* The class name provided by Drupal.settings.plus1.link_class what we will search for in our jQuery later. */
     ?>
     <div class="plus1-vote"> <?php echo $variables['plus1']['linkvote'];?> </div>
   <?php } ?>
    </div>
  <?php } ?>

  <div class="<?php echo $variables['plus1']['score_class'];?>">
  <?php echo $score; ?>
  </div>
  </div>