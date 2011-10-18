
(function ($) {

/**
 * Control the visibility of additional configuration options
 */
Drupal.behaviors.webformAltUI = {
  attach: function (context) {
    $('.check-expand', context).children().not(':first-child').hide();
    $('.check-expander', context)
      .bind('click', Drupal.behaviors.webformAltUI.toggleInputs)
      .each(Drupal.behaviors.webformAltUI.toggleInputs);
    if (Drupal.formBuilder) {
      Drupal.formBuilder.fieldConfigureForm = $('#form-builder-field-configure');
    }

    // Add checkboxes before the labels for fields in the field configure form
    Drupal.behaviors.webformAltUI.fieldSettingsCheckboxes(context);

    // Because runs all behaviors on every AJAX callback unset and set clicks.
    $('#form-builder .form-builder-links a')
      .unbind('click')
      .bind('click', Drupal.formBuilder.editField)
      .bind('click', Drupal.behaviors.webformAltUI.showFieldSettings);
    $('.form-builder-element')
      .unbind('click')
      .bind('click', Drupal.behaviors.webformAltUI.showFieldSettings)
      .bind('click', Drupal.formBuilder.clickField);
    $('.form-builder-element label').unbind('click');
    $('#form-builder .form-builder-links a.delete')
      .unbind('click')
      .bind('click', Drupal.behaviors.webformAltUI.removeField);

    // Enable the scroll for the active tabs.
    $('.horizontal-tabs-pane').bind('horizontalTabActivate', Drupal.behaviors.webformAltUI.tabActivated);
    $(window).bind('scroll', Drupal.behaviors.webformAltUI.windowScroll);
    if (!Drupal.behaviors.webformAltUI.activeTab) {
      var tab = {};
      tab.fieldset = $('.horizontal-tabs-pane:visible');
      if (tab.fieldset.length) {
        Drupal.behaviors.webformAltUI.activeTab = tab;
        Drupal.behaviors.webformAltUI.tabActivated({}, tab);
      }
    }

    // Bind event handlers to update the submit button text.
    $('input[name=submit_button]').bind('blur keyup', Drupal.behaviors.webformAltUI.submitTextChange);

    // Check to see if the form is empty and need explanatory text
    Drupal.behaviors.webformAltUI.checkForm(true);

    // Remove useless links from the options_element configuration
    $('#edit-options-options-field-widget a.add').remove();
    $('.form-options-manual').remove();

    // Disable dragging on the submit button
    $('a.configure.button').closest('.form-builder-wrapper').draggable('destroy');

    // Remove the field settings form from inside the node form before
    // submitting the node form.
    $('#webform-node-form').bind('submit', Drupal.behaviors.webformAltUI.removeNestedForms);
  },
  toggleInputs: function (event) {
    if ($(this).attr('checked')) {
      $(this).closest('.check-expand').children().not(':first-child').slideDown();
    } else {
      $(this).closest('.check-expand').children().not(':first-child').slideUp();
    }
  },
  fieldSettingsCheckboxes: function (context) {
    $('.check-toggle:visible', context).once('check-toggle', function() {
      // Keep track of, and hide the input itself.
      var input = $(this).is('textarea') ? $(this).parent() : $(this);

      var wrapper = input.parent();

      // Add the checkbox and bind its behavior.
      var checkbox = $('<input type="checkbox" />');
      checkbox
        .attr('id', $(this).attr('name') + '-checkbox')
        .bind('click', input, Drupal.behaviors.webformAltUI.fieldSettingsCheckToggle)
        .prependTo(wrapper);

      // If there is already a value here, check the box, otherwise hide the input
      if ($(this).val()) {
        checkbox.attr('checked', true);
      } else {
        wrapper.children().not(checkbox).not('label').hide();
      }
      // Link the label to the new checkbox so that clicking on it selects the
      // checkbox.
      wrapper.find('label').attr('for', checkbox.attr('id'));
    });
  },
  fieldSettingsCheckToggle: function (event) {
    var input = event.data;
    if ($(event.target).attr('checked') == true) {
      // Show the field.
      input.parent().children().not(event.target).not('label').slideDown();
    } else {
      // Clear the field and hide it.
      input.parent().find('input, textarea').val('').text('').trigger('change');
      input.parent().children().not(event.target).not('label').slideUp();
    }
  },
  showFieldSettings: function (event) {
    // Trigger a click on the field settings tab
    $('#edit-ui-wrapper .horizontal-tabs-list > .last:not(.selected) a').click();
  },
  tabActivated: function (event, tab) {
    tab.offset = tab.fieldset.offset().top;
    if (typeof Drupal.toolbar != 'undefined') {
      tab.offset = tab.offset - Drupal.toolbar.height();
    }
    Drupal.behaviors.webformAltUI.activeTab = tab;
    Drupal.behaviors.webformAltUI.scrollFieldset();

    $('#form-builder').css('min-height', tab.fieldset.height());
  },
  windowScroll: function (event) {
    if (Drupal.behaviors.webformAltUI.timeout) {
      clearTimeout(Drupal.behaviors.webformAltUI.timeout);
    }
    Drupal.behaviors.webformAltUI.timeout = setTimeout(Drupal.behaviors.webformAltUI.scrollFieldset, 100);
  },
  scrollFieldset: function () {
    var fieldset = Drupal.behaviors.webformAltUI.activeTab.fieldset;
    var offset = Drupal.behaviors.webformAltUI.activeTab.offset;
    // Do not move the palette while dragging a field.
    if (Drupal.formBuilder.activeDragUi) {
      return;
    }

    var windowOffset = $(window).scrollTop();
    var blockHeight = fieldset.height();
    var formBuilderHeight = $('#form-builder').height();
    if (windowOffset - offset > 0) {
      // Do not scroll beyond the bottom of the editing area.
      var toolbarOffset = parseInt($('body').css('padding-top')) + parseInt($('body').css('margin-top')) + 10;
      var newTop = Math.min(windowOffset - offset + toolbarOffset, formBuilderHeight - blockHeight);
      fieldset.animate({'margin-top': (newTop + 'px')}, 'fast');
    }
    else {
      fieldset.animate({'margin-top': '0px'}, 'fast');
    }
  },
  removeField: function (event) {
    event.preventDefault();
    event.stopPropagation();
    Drupal.formBuilder.setActive($(event.target).parents('.form-builder-wrapper').get(0), event.target);
    Drupal.formBuilder.ajaxOptions = {
      url: $(this).attr('href'),
      type: 'POST',
      dataType: 'json',
      data: 'js=1',
      success: Drupal.behaviors.webformAltUI.removeFieldCallback,
      error: Drupal.formBuilder.ajaxError,
      errorMessage: Drupal.t('Field could not be deleted at this time.  Please try again later.'),
      maxTry: 3,
      tryCount: 0
    };
    // Submit the form via ajax
    $.ajax(Drupal.formBuilder.ajaxOptions);
  },
  removeFieldCallback: function (response) {
    Drupal.formBuilder.deleteField(Drupal.behaviors.webformAltUI.checkForm);
  },
  submitTextChange: function (e) {
    // Put the value of this field into the submit button.
    var text = $(this).val() ? $(this).val() : Drupal.t('Submit');
    $('#form-builder .button').text(text);
    // Put the value into the hidden field in the form settings advanced tab
    $('input[name=submit_text]').val($(this).val());
  },
  checkForm: function () {
    var fb = $('#form-builder');
    if (fb.children().length <= 1) {
      $('<div class="form-builder-placeholder empty-form">' + Drupal.t('Drag a field from the Add field tab to add it to your webform') + '</div>').prependTo(fb);
    } else {
      // Ensure that the submit button is always at the bottom of the form
      fb.find('.button').closest('.form-builder-wrapper').appendTo(fb);
    }
  },
  removeNestedForms: function (submitEvent) {
    $(submitEvent.target).find('form').remove();
    return true;
  }
};

})(jQuery);
