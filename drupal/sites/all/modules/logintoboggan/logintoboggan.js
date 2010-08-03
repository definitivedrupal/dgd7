// $Id: logintoboggan.js,v 1.3 2009/10/19 17:48:25 thehunmonkgroup Exp $

(function ($) {
  Drupal.behaviors.LoginToboggan = {
    attach: function (context, settings) {
      $('#toboggan-login', context).once('toggleboggan_setup', function () {
        $(this).hide();
        Drupal.logintoboggan_toggleboggan();
      });
    }
  };

  Drupal.logintoboggan_toggleboggan = function() {
    $("#toboggan-login-link").click(
      function () {
        $("#toboggan-login").slideToggle("fast");
        this.blur();
        return false;
      }
    );
  };
})(jQuery);