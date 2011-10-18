
/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true */

(function ($) {
  
Drupal.behaviors.RotatingBannerSettings = {
  context: null,

  attach: function(context) {
    $('#block-admin-configure', context).once('rotatingBannerAdminProcessed', function() {
      // Set up the effect preview div.
      $('#edit-rotating-banner-banner-settings-cycle-fx').bind('change', function() {
        if ($(this).val() === '') {
          $('.rb-fx-setting').parent().hide();
          $('#rb-settings-effect-preview').hide();
        } else {
          $('#rb-settings-effect-preview img').css('height', '150px').css('width', '150px');
          Drupal.behaviors.RotatingBannerSettings.updatePreview();
        }
      });
      $('#edit-rotating-banner-banner-settings-cycle-fx').trigger('change');


      // Only show the timeout field if the user checks the checkbox.
      if ($('#edit-rotating-banner-banner-settings-cycle-timeout').val() == '') {
        $('#auto-transition-selector').attr('checked', false);
      } else {
        $('#auto-transition-selector').attr('checked', true);
      }

      var timeoutDelay = $('#edit-rotating-banner-banner-settings-cycle-timeout').val();
      $('#edit-rotating-banner-banner-settings-cycle-timeout').bind('change', function () {
        timeoutDelay = $('#edit-rotating-banner-banner-settings-cycle-timeout').val();
      });

      $('#auto-transition-selector').bind('change', function() {
        if ($(this).attr('checked')) {
          $('#edit-rotating-banner-banner-settings-cycle-timeout').parent().show();
          if (!timeoutDelay) {
            timeoutDelay = 8000;
          }
          $('#edit-rotating-banner-banner-settings-cycle-timeout').val(timeoutDelay);
        } else {
          $('#edit-rotating-banner-banner-settings-cycle-timeout').parent().hide();
          $('#edit-rotating-banner-banner-settings-cycle-timeout').val('');
        }
      });
      $('#auto-transition-selector').trigger('change');


      // Only show the height and width if the user doesn't want fluid mode
      $('#edit-rotating-banner-banner-settings-fluid').bind('change', function () {
        var fluid_setting = $('input:checked', this).val();
        if (fluid_setting === '1') {
          // Cheap, but for now, we just set it to null.
          $('.rb-dimension-settings').parent().hide();
        } else {
          $('.rb-dimension-settings').parent().show();
        }
      });
      $('#edit-rotating-banner-banner-settings-fluid').trigger('change');
    });
  },

  // Updates the preiew of the effect.
  updatePreview: function () {
    var effect = $('#edit-rotating-banner-banner-settings-cycle-fx').val();
    // If we want to add this later we can.
    //var timeout = $('#edit-rotating-banner-banner-settings-cycle-timeout').val();
    $('#rb-settings-effect-preview').cycle({
      'fx' : effect,
      'timeout': '1000ms'
    });
  }
}

/**
 * For editing a banner slide
 */
Drupal.behaviors.RotatingBannerSlideEdit = {
  attach: function (context) {
    if (Drupal.settings.rotatingBanner == undefined) {
      // This is not the slide edit page.
      return;
    }

    $('#rotating-banner-slide-form', context).once('rotatingBannerSlideEditorProcessed', function() {

      // Setup a jQuery method to get the layout class on the wrapper
      $.fn.getTextBoxType = function() {
        var classes = this.attr('class').split(' ')
        for (var i = 0; i < classes.length; i++){
          if (classes[i].match(/rb-textbox-type-/)) {
            return classes[i];
          }
        }
        return false;
      }

      // Hide the slide preview if there is no image.
      if ($('img', '.rb-slide').length === 0) {
        $('.rb-slide-preview').hide();
      }

      // This custom handler gets called when the user selects a background image
      // used to add show the preview label.
      $('.rb-slide').bind('imgAdded', function() {
        $('.rb-slide-preview').show();
      });

      // Create a submit handler to save the textbox positions and content
      $('#rotating-banner-slide-form').bind('submit', Drupal.behaviors.RotatingBannerSlideEdit.savetextboxes);

      // Configures the slide height and width.
      var bannerSettings = Drupal.settings.rotatingBanner;
      //$('.rb-slide').css('width', bannerSettings.width);
      //$('.rb-slide').css('height', bannerSettings.height);

      // Setup the image chooser link
      $('a.choose-image-link').click(function () {
        Drupal.media.popups.mediaBrowser(function (mediaFiles) {
          var background = mediaFiles[0];
          $('#edit-fid').val(background.fid);
          $('.rb-slide img').remove();
          var imgElem = $('<img/>').attr('src', background.url);
          $('.rb-slide').append(imgElem);
          $('.rb-slide').trigger('imgAdded');
        });
       });


      // Record the original classes applied to the slide element
      // This is so we can swap out the layout classes.
      var originalClasses = $('.layout-wrapper').attr('class');

      // Removes the anchor wrappers around rb-textbox elements
      // The inline element throws off the textEdit state
      Drupal.behaviors.RotatingBannerSlideEdit.stripLinks();
      
      // Makes the textboxes editable by double-clicking them.
      Drupal.behaviors.RotatingBannerSlideEdit.makeEditable();

      // Binds the editing input elements to the textboxes.
      Drupal.behaviors.RotatingBannerSlideEdit.setupLayoutTextBoxes();

      $('#edit-layout').bind('change', function() {
        // If the user selects Custom mode from this select box, then they can
        // drag the header and text box around the slide as required.  The can also
        // add as many as they want.
        // If they select anything else, we show text boxes for title and header
        // They are not able to position the boxes in this case.

        if ($(this).val() === 'custom') {
          // Custom mode.

          $('.layout-wrapper').removeClass().addClass(originalClasses);
          $('.layout-wrapper').addClass($(this).val());
          $('.rb-custom-layout-tools').show();
          $('.layout-text-editor').hide();
          // Setup dragging
          $('.rb-textbox-wrapper').draggable({
            containment: 'parent'
          });
          // Iterate through the textboxes and set their right and bottom to auto so they can be dragged properly
          $('.rb-textbox-wrapper').each( function() {
            $(this)
              .css('left', $(this).position().left)
              .css('top', $(this).position().top)
              .css('bottom', 'auto')
              .css('right', 'auto');
          });
        } else {
          $('.rb-custom-layout-tools').hide();
          $('.layout-text-editor').show();
          $('.layout-wrapper').removeClass().addClass(originalClasses);
          $('.layout-wrapper').addClass($(this).val());

          $('.rb-textbox-wrapper.ui-draggable').draggable('destroy');
          //Drupal.behaviors.RotatingBannerSlideEdit.makeUnEditable();
        }
      });

      // Fire a trigger to tell the form to go into custom or layout mode.
      $('#edit-layout').trigger('change');


      // Configure the add text-box functions
      $('#rb-add-text').bind('click', function() {return Drupal.behaviors.RotatingBannerSlideEdit.newText({type: 'rb-text'});});
      $('#rb-add-headline').bind('click', function() {
        return Drupal.behaviors.RotatingBannerSlideEdit.newText({type: 'rb-textbox-type-header', content: 'Lorem ipsum puttum baneradi four ure bannerum'});
      });
    });
  },

  newText: function(options) {
    
    options = $.extend({
      type: 'rb-textbox-type-text', // The CSS class to add
      position: {top:20, left:20},
      content: 'Lorem ipsum puttum textim four ure bannerum. <br/> Isthay aragrphay siya ay ittleay ongerlae.'
    },options);

    // Create an editable text region
    var rbTextArea = $('<div></div>')
      .addClass('rb-textbox')
      .html(options.content);
  
    // Put the whole thing in a wrapper for dragging
    $('<div></div>')
      .css('top', options.position.top)
      .css('left', options.position.left)
      .addClass('rb-textbox-wrapper')
      .addClass(options.type)
      .append(rbTextArea)
      // Append to the slide
      .prependTo($('.layout-wrapper'));

    $('.rb-textbox-wrapper').draggable({
      containment: 'parent'
    });
    Drupal.behaviors.RotatingBannerSlideEdit.makeEditable();
    return false;
  },
  
  stripLinks: function() {
    $('.rb-textbox').find('a').children().unwrap();
  },

  makeUnEditable: function() {
    $('.rb-textbox').unbind('dblclick');
    $('.rb-textbox').unbind('blur');
  },

  makeEditable: function () {
    $('.rb-textbox').once('editableAttached', function() {
      $(this).bind('dblclick', function() {
        // Odd, when I use disable here, my addClass() no longer works :(
        $(this).attr('contentEditable', true);
        $(this).parent().draggable('destroy');
        $(this).parent().addClass('editMode');
        $('#edit-submit').attr('disabled', true);
        $('#edit-submit').css('opacity', 0.5);
      });
     
     $(this).bind('blur', function() {
       $(this).trigger('editEdit');
       $(this).attr('contentEditable', false);
       // Only make draggable if the layout is set to custom
       if ($('.layout-wrapper').hasClass('custom')) {
         $(this).parent().draggable({containment: 'parent'});
       }
       $(this).parent().removeClass('editMode');
       $('#edit-submit').attr('disabled', false);
       $('#edit-submit').css('opacity', 1);
     });
     //$(this).editableText({
     //  newlinesEnabled: true
     //});
    });
  },

  savetextboxes: function () {
    var textboxes = [];
    $('.rb-textbox-wrapper').each(function() {
      // Convert position from px to %
      $position = $(this).position();
      $parentHeight = $(this).parent().height();
      $parentWidth = $(this).parent().width();
      $position.right = ($parentWidth - ($position.left + $(this).width())) / $parentWidth * 100;
      $position.bottom = ($parentHeight - ($position.top + $(this).height())) / $parentHeight * 100;
      $position.left = $position.left / $parentWidth * 100;
      $position.top = $position.top / $parentHeight * 100;
      
      if ($position.right < $position.left) {
        $position.left = 'auto';
      }
      if ($position.bottom < $position.top) {
        $position.top = 'auto'
      }
      
      textboxes.push({
        position: $position,
        content: $('.rb-textbox', this).html(),
        type: $(this).getTextBoxType()
      });
    });
    $('#edit-textboxes').val(JSON.stringify(textboxes));
  },

  setupLayoutTextBoxes: function() {
    // For each textbox, create a text field.
    //$('.layout-text-editor').append();
    $('.rb-textbox-wrapper').each(function() {
      var content = $('.rb-textbox', this).html();
      if ($(this).getTextBoxType() === 'rb-textbox-type-header') {
        Drupal.behaviors.RotatingBannerSlideEdit.bindEditorTextBox($('#edit-layout-text-editor-header'), $(this))
        $(this).trigger('keyup');
      } else {
        Drupal.behaviors.RotatingBannerSlideEdit.bindEditorTextBox($('#edit-layout-text-editor-text'), $(this));
        $(this).trigger('keyup');
      }
//      $('.layout-text-editor').append(input);
      // Create a textbox.
    });
  },

  bindEditorTextBox: function(input, textbox) {
    function nl2br(txt) {
      return txt.replace(/[\n(\r\n)]/g, "<br/>");
    }
    function br2nl(txt) {
      return txt.replace(/(<br\/>|<br>)/g, "\n");
    }
    textbox.bind('keyup', function() {
      var content = br2nl($('.rb-textbox', this).html());
      input.val(content);
    });
    input.bind('keyup', function() {
      $('.rb-textbox', textbox).html(nl2br(input.val()));
    });
  }
};


})(jQuery);