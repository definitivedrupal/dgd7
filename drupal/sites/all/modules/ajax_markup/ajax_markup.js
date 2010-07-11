// $Id: ajax_markup.js,v 1.4 2010/06/13 10:51:26 ufku Exp $
(function($) {

//get markup after processing the input by the format.
//The "callback" is called with 3 parameters: output, status(true|false), xmlHttpRequest
$.ajaxMarkup = function(input, format, callback) {
  //check if callback exists.
  if (!$.isFunction(callback)) {
    return;
  }
  //check input
  var input = $.trim(input);
  if (!input) {//return empty output for empty input.
    return callback('', true);
  }
  //check cache
  var hash = $.ajaxMarkup.hash, cache = $.ajaxMarkup.cache;
  var cid = format + ':' + hash(input);
  if (cache[cid] !== undefined) {
    return callback(cache[cid], true);
  }
  //check required parameters for ajax request.
  if (!$.ajaxMarkup.url || !$.ajaxMarkup.token) {
    return callback(Drupal.t('Some required parameters are missing for an ajax request.'), false);
  }
  //request filtered output.
  var output = '', status = false;
  $.ajax({
    url: $.ajaxMarkup.url,
    data: {input: input, format: format, token: $.ajaxMarkup.token},
    type: 'POST',
    dataType: 'json',
    success: function(response) {
      output = cache[cid] = response.output || '';
      status = true;
    },
    error: function(request) {
      output = Drupal.ahahError(request, this.url);
    },
    complete: function(request) {
      callback.call(this, output, status, request);
    }
  });
};

//store recent output
$.ajaxMarkup.cache = {};

//string hashing used for creating cache ids. Prof. Daniel J. Bernstein's algorithm.
$.ajaxMarkup.hash = function(str) {
  for(var c, i = 0, h = 5381; c = str.charCodeAt(i); i++) {
    h = (h << 5) + h + c;
  }
  return h & 0x7FFFFFFF;
};

//Helper function to find the input format of a given textarea. defaults to 0 when there is no format.
$.ajaxMarkup.getFormat = function(selector) {
  var T = $(selector)[0];
  if (!T || T.tagName != 'TEXTAREA') {
    return 0;
  }
  var i, format = 0, name = T.name || '';
  if ((i = name.indexOf('[value]')) > 0) {//fields
    format = $(T.form.elements[name.substring(0, i) + '[format]']).val() || 0;
  }
  return format;
};

//set dynamic parameters(url & token).
$(document).ready(function() {
  $.extend($.ajaxMarkup, Drupal.settings.ajaxMarkup);
});

})(jQuery);