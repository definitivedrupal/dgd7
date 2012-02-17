// $Id: README.txt,v 1.1 2010/04/29 20:23:09 johnalbin Exp $

HEAD to HEAD
------------

This module provides upgrade paths for schema changes in Drupal 7 until the
HEAD-HEAD upgrade path is officially supported (after Drupal 7.0-beta1 is
released.)

In order to use this module, you will need to create a custom module that calls
the appropriate head2head_[issue number]() functions needed for your specific
installation.


Alpha to Alpha
--------------

Alpha2Alpha is a helper module that depends on head2head. It can only be used if
your website only uses official alpha releases. It cannot be used if your
website is using Drupal HEAD from CVS.

The alpha2alpha module uses the head2head upgrade functions in its .install file
to provide upgrades through Drupal's normal update.php page.

If you install and use alpha2alpha, you should not uninstall it or alpha2alpha
will forget which updates have been completed the next time you install it.
However, you can safely disable the module; the next time you enable it, it will
remember which updates have already been run.

However, just because this module is easier to use than head2head, in no way
should it be seen as an endorsement of running a real website on Drupal alpha.
There are no guarantees that you won't lose all your data.
