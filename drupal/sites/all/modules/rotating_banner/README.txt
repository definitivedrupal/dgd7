Install the module via the installation routine of Drupal 7 in the admin section | module | Install new module.

The following modules are required to be installed:

## dependencies
Media
http://drupal.org/project/media

Styles
http://drupal.org/project/styles

Important!
In order to run rotating banners you have to copy two scripts to the server directory

sites/all/modules/rotating_banner/includes/

1. jquery.cycle.js
2. jquery.easing.js

You will find the scripts here, they have to be renamed.

Try to use the "jquery.cycle.all.min.js",
rename it to "jquery.cycle.js"
http://jquery.malsup.com/cycle/download.html

Try to use "jquery.easing.1.3.js" rename it to "jquery.easing.js"
http://gsgd.co.uk/sandbox/jquery/easing/

## Usage
To begin building a rotating banner you first have to define a new block in the admin section | structure | blocks.

Next to "add new block" there is a new link called "add new rotating banner".
Now you have to declare a new title for the banner-block. The following is self explanatory.
After uploading the pictures and choosing the transition and so on, it is necessary to enable the block "Rotating banners: TITLE YOU ADDED" and position the block in the wanted region.
