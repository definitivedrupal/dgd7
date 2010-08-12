<div id="page-wrapper"><div id="page">

  <div id="header"><div class="section clearfix">

    <?php if ($logo): ?>
      <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo">
        <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
      </a>
    <?php endif; ?>

    <?php if ($site_name || $site_slogan): ?>
      <div id="name-and-slogan"<?php if ($hide_site_name && $hide_site_slogan) { print ' class="element-invisible"'; } ?>>

        <?php if ($site_name): ?>
          <?php if ($title): ?>
            <div id="site-name"<?php if ($hide_site_name) { print ' class="element-invisible"'; } ?>>
              <strong>
                <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
              </strong>
            </div>
          <?php else: /* Use h1 when the content title is empty */ ?>
            <h1 id="site-name"<?php if ($hide_site_name) { print ' class="element-invisible"'; } ?>>
              <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><span><?php print $site_name; ?></span></a>
            </h1>
          <?php endif; ?>
        <?php endif; ?>

        <?php if ($site_slogan): ?>
          <div id="site-slogan"<?php if ($hide_site_slogan) { print ' class="element-invisible"'; } ?>>
            <?php print $site_slogan; ?>
          </div>
        <?php endif; ?>

      </div> <!-- /#name-and-slogan -->
    <?php endif; ?>

    <?php print render($page['header']); ?>

    <?php if ($main_menu): ?>
      <div id="navigation"><div class="section clearfix">
        <?php print theme('links__system_main_menu', array(
          'links' => $main_menu,
          'attributes' => array(
            'id' => 'main-menu',
            'class' => array('links', 'clearfix'),
          ),
          'heading' => array(
            'text' => t('Main menu'),
            'level' => 'h2',
            'class' => array('element-invisible'),
          ),
        )); ?>
      </div></div> <!-- /.section, /#navigation -->
    <?php endif; ?>

  </div></div> <!-- /.section, /#header -->

  <?php if ($messages): ?>
    <div id="messages"><div class="section clearfix">
      <?php print $messages; ?>
    </div></div> <!-- /.section, /#messages -->
  <?php endif; ?>

  <?php if ($page['featured']): ?>
    <div id="featured" class="region"><div class="section clearfix">
      <?php print render($page['featured']); ?>
    </div></div> <!-- /.section, /#featured -->
  <?php endif; ?>

  <div id="main-wrapper" class="clearfix"><div id="main" class="clearfix">

    <div id="content" class="column"><div class="section">
      <a id="main-content"></a>
      <?php print render($title_prefix); ?>
      <?php if ($title): ?>
        <h1 class="title" id="page-title">
          <?php print $title; ?>
        </h1>
      <?php endif; ?>
      <?php print render($title_suffix); ?>
      <?php if ($tabs): ?>
        <div class="tabs">
          <?php print render($tabs); ?>
        </div>
      <?php endif; ?>
      <?php print render($page['help']); ?>
      <?php if ($action_links): ?>
        <ul class="action-links">
          <?php print render($action_links); ?>
        </ul>
      <?php endif; ?>
      <div id="highlight"><?php print render($page['highlight']); ?></div>

    </div></div> <!-- /.section, /#content -->

  </div></div> <!-- /#main, /#main-wrapper -->

  <?php if ($page['triptych_first'] || $page['triptych_middle'] || $page['triptych_last']): ?>
    <div id="triptych-wrapper"><div id="triptych" class="clearfix">

      <?php if ($page['triptych_first']): ?>
        <div id="triptych-first" class="region triptych"><div class="section">
          <?php print render($page['triptych_first']); ?>
        </div></div> <!-- /.section, /#triptych-first -->
      <?php endif; ?>

      <?php if ($page['triptych_middle']): ?>
        <div id="triptych-middle" class="region triptych"><div class="section">
          <?php print render($page['triptych_middle']); ?>
        </div></div> <!-- /.section, /#triptych-middle -->
      <?php endif; ?>

      <?php if ($page['triptych_last']): ?>
        <div id="triptych-last" class="region triptych"><div class="section">
          <?php print render($page['triptych_last']); ?>
        </div></div> <!-- /.section, /#triptych-last -->
      <?php endif; ?>

    </div></div> <!-- /#triptych, /#triptych-wrapper -->
  <?php endif; ?>

  <div id="footer-wrapper"><div class="section">

    <?php if ($page['footer_firstcolumn'] || $page['footer_secondcolumn'] || $page['footer_thirdcolumn'] || $page['footer_fourthcolumn']): ?>
      <div id="footer-columns" class="clearfix">

        <?php if ($page['footer_firstcolumn']): ?>
          <div id="footer-firstcolumn" class="region sitemap"><div class="section">
            <?php print render($page['footer_firstcolumn']); ?>
          </div></div> <!-- /.section, /#footer-firstcolumn -->
        <?php endif; ?>

        <?php if ($page['footer_secondcolumn']): ?>
          <div id="footer-secondcolumn" class="region sitemap"><div class="section">
            <?php print render($page['footer_secondcolumn']); ?>
          </div></div> <!-- /.section, /#footer-secondcolumn -->
        <?php endif; ?>

        <?php if ($page['footer_thirdcolumn']): ?>
          <div id="footer-thirdcolumn" class="region sitemap"><div class="section">
            <?php print render($page['footer_thirdcolumn']); ?>
          </div></div> <!-- /.section, /#footer-thirdcolumn -->
        <?php endif; ?>

        <?php if ($page['footer_fourthcolumn']): ?>
          <div id="footer-fourthcolumn" class="region sitemap"><div class="section">
            <?php print render($page['footer_fourthcolumn']); ?>
          </div></div> <!-- /.section, /#footer-fourthcolumn -->
        <?php endif; ?>

      </div><!-- /#footer-columns -->
    <?php endif; ?>

    <?php if ($page['footer'] || $secondary_menu): ?>
      <div id="footer" class="clearfix">
        <?php print theme('links__system_secondary_menu', array(
          'links' => $secondary_menu,
          'attributes' => array(
            'id' => 'secondary-menu',
            'class' => array('links', 'clearfix'),
          ),
          'heading' => array(
            'text' => t('Secondary menu'),
            'level' => 'h2',
            'class' => array('element-invisible'),
          ),
        )); ?>
        <?php print render($page['footer']); ?>
    </div><!-- /#footer -->
    <?php endif; ?>

  </div></div> <!-- /.section, /#footer-wrapper -->

</div></div> <!-- /#page, /#page-wrapper -->
