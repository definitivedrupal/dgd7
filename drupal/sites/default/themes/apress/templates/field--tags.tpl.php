<?php
// $Id: field.tpl.php,v 1.13 2010/03/26 17:14:45 dries Exp $

/**
 * @file field--tags.tpl.php
 * Custom template file used to output tag-like data.
 *
 * @see template_preprocess_field()
 * @see theme_field()
 */
?>
<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php if (!$label_hidden) : ?>
    <h4 class="title" <?php print $title_attributes; ?>><?php print $label ?></h4>
  <?php endif; ?>
  <?php if ($content_attributes): ?>
  <div<?php print $content_attributes; ?>>
  <?php endif; ?>
    <?php $num_fields = count($items); ?>
    <?php $i = 1; ?>
    <?php foreach ($items as $delta => $item) : ?>
      <?php print render($item); ?><?php $i != $num_fields ? print ', ' : ''; ?>
    <?php $i++; ?>
    <?php endforeach; ?>
  <?php if ($content_attributes): ?>
  </div>
  <?php endif; ?>
</div>
