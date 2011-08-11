This module provides integration between your Drupal site and the 
Varnish HTTP Accelerator, an advanced and very fast reverse-proxy system. 
Basically, Varnish handles serving static files and anonymous page-views
for your site much faster and at higher volumes than Apache, 
in the neighborhood of 3000 requests per second.

## Installation
In order to install the module you need to do the following:

* Enable the module.
* Add something like this to your settings.php file:
  // Add Varnish as the page cache handler.
  $conf['cache_backends'] = array('sites/all/modules/varnish/varnish.cache.inc');
  $conf['cache_class_cache_page'] = 'VarnishCache';
  // Drupal 7 does not cache pages when we invoke hooks during bootstrap. This needs
  // to be disabled.
  $conf['page_cache_invoke_hooks'] = FALSE;
* Go to admin/config/development/varnish and configure your connection Varnish
  appropriately. It should be pretty straight forward from here on.

## Running the simpletests for Varnish
In order to test the Varnish module, you need a "working" Varnish configuration that caches
pages for anonymous users. You also need to specify the variables that configures
your Varnish connection in your $conf array in your settings.php file.