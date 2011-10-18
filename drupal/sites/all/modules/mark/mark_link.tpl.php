<?php
// $Id: mark_link.tpl.php,v 1.2.4.1 2010/10/01 18:47:04 yhahn Exp $
foreach ($links as $id => $link):
?>
<div id="<?php print $id ?>" class="mark-link <?php print ($link->marked ? 'marked' : 'unmarked') ?>">
  <?php print l($link->title, $link->path, $link->options) ?>
</div>
<?php endforeach; ?>
