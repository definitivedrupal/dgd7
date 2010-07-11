// $Id: README.txt,v 1.2 2009/06/28 21:28:45 ufku Exp $

- Ajax markup:
Provides an API for generating filtered markup using ajax.

- INSTALL:
Just enable it at module administration page.

- USE:
1) Load the API by calling ajax_markup_on() at server side.

if (module_invoke('ajax_markup', 'on')) {
  drupal_add_js(YOUR_SCRIPT.js);
}

2) Inside YOUR_SCRIPT.js call $.ajaxMarkup

$.ajaxMarkup(INPUT, INPUT_FORMAT, CALLBACK);

function CALLBACK(OUTPUT, SUCCESS, REQUEST) {
  if (SUCCESS) $('div#preview').html(OUTPUT);
  else alert(OUTPUT);
}

INPUT: String of which you want to get a filtered version.
INPUT_FORMAT: Integer representing one of the drupal input formats. Provide 0 for the default.
CALLBACK: Function to send the output after the request.
OUTPUT: String that is the filtered INPUT. Content depends on the INPUT_FORMAT and SUCCESS state.
SUCCESS: Boolean representing the status of the request.
REQUEST: XmlHttpRequest. Not available when the OUTPUT is retrieved from the cache.

- ALSO:
API provides a function that tries to get the INPUT_FORMAT of a textarea.

$.ajaxMarkup.getFormat('#edit-body'); //returns 0 (the default format) when there is no input format for the element.

- ACCESS:
Users must have "access ajax markup" permission.
Users must have access to the supplied INPUT_FORMAT, otherwise the default format is used.