
(function ($) {

Drupal.chatroom = Drupal.chatroom || {'initialised' : false};

/**
 * Add behaviours to chatroom elements.
 */
Drupal.behaviors.chatroom = {
  attach: function (context, settings) {
    if (!Drupal.chatroom.initialised) {
      if (!Drupal.settings.chatroom.customPollingBackend) {
        setInterval("Drupal.chatroom.poll()", Drupal.settings.chatroom.pollInterval * 1000);
      }
      Drupal.settings.chatroom.pageTitle = document.title;
      Drupal.settings.chatroom.hasFocus = true;
      if (Drupal.settings.chatroom.latestMsgId > 0) {
        var targetOffset = $('div.new-message:last').offset().top;
        var boardOffset = $('#chatroom-board').offset().top;
        var scrollAmount = targetOffset - boardOffset;
        $('#chatroom-board').animate({scrollTop: '+='+ scrollAmount +'px'}, 500);
        $('.new-message').removeClass('new-message');
      }
      $(self).focus(
        function() {
          clearInterval(Drupal.settings.chatroom.warnInterval);
          Drupal.settings.chatroom.hasFocus = true;
          document.title = Drupal.settings.chatroom.pageTitle;
        }
      );
      $(self).blur(
        function() {
          Drupal.settings.chatroom.hasFocus = false;
        }
      );
      Drupal.chatroom.initialised = true;
    }

    $('#edit-chatroom-message-entry-box').keyup(function(e) {
      var messageText = $('#edit-chatroom-message-entry-box').val().replace(/^\s+|\s+$/g, '');
      var anonNameText = '';
      if ($('#edit-chatroom-anon-name').length) {
        anonNameText = $('#edit-chatroom-anon-name').val().replace(/^\s+|\s+$/g, '');
      }
      if (messageText && e.keyCode == 13 && !e.shiftKey && !e.ctrlKey) {
        Drupal.chatroom.postMessage(messageText, anonNameText);
        $('#edit-chatroom-message-entry-box').val('').focus();
      }
      else {
        return true;
      }
    });
    $('#edit-chatroom-message-entry-submit').click(function (e) {
      e.preventDefault();
      e.stopPropagation();
      var messageText = $('#edit-chatroom-message-entry-box').val().replace(/^\s+|\s+$/g, '');
      var anonNameText = '';
      if ($('#edit-chatroom-anon-name').length) {
        anonNameText = $('#edit-chatroom-anon-name').val().replace(/^\s+|\s+$/g, '');
      }
      if (messageText) {
        Drupal.chatroom.postMessage(messageText, anonNameText);
        $('#edit-chatroom-message-entry-box').val('').focus();
      }
    });

    $('.chatroom-kick-user-link').click(function (e) {
      e.preventDefault();
      e.stopPropagation();
      Drupal.chatroom.kickUser(e.target.parentNode.id);
    });

    $('.chatroom-ban-user-link').click(function (e) {
      e.preventDefault();
      e.stopPropagation();
      Drupal.chatroom.banUser(e.target.parentNode.id);
    });

    $('.chatroom-remove-user-link').click(function (e) {
      e.preventDefault();
      e.stopPropagation();
      Drupal.chatroom.removeUser(e.target.parentNode.id);
    });
  }
};

Drupal.chatroom.banUser = function(uid) {
  $.ajax({
    type: 'POST',
    url: Drupal.settings.basePath + Drupal.settings.chatroom.banUserPath + '/' + Drupal.settings.chatroom.chatId,
    dataType: 'json',
    success: Drupal.chatroom.pollHandler,
    data: {
      uid: uid,
      formToken: $('#edit-chatroom-chat-management-form-form-token').val(),
      formId: 'chatroom_chat_management_form'
    }
  });
}

Drupal.chatroom.kickUser = function(uid) {
  $.ajax({
    type: 'POST',
    url: Drupal.settings.basePath + Drupal.settings.chatroom.kickUserPath + '/' + Drupal.settings.chatroom.chatId,
    dataType: 'json',
    success: Drupal.chatroom.pollHandler,
    data: {
      uid: uid,
      formToken: $('#edit-chatroom-chat-management-form-form-token').val(),
      formId: 'chatroom_chat_management_form'
    }
  });
}

Drupal.chatroom.removeUser = function(uid) {
  $.ajax({
    type: 'POST',
    url: Drupal.settings.basePath + Drupal.settings.chatroom.removeUserPath + '/' + Drupal.settings.chatroom.chatId,
    dataType: 'json',
    success: Drupal.chatroom.pollHandler,
    data: {
      uid: uid,
      formToken: $('#edit-chatroom-chat-management-form-form-token').val(),
      formId: 'chatroom_chat_management_form'
    }
  });
}

Drupal.chatroom.poll = function() {
  var skipCacheCheck = 0;
  if (Drupal.settings.chatroom.successiveCacheHits > Drupal.settings.chatroom.skipCacheCheckCount) {
    skipCacheCheck = 1;
  }

  $.ajax({
    type: 'POST',
    url: Drupal.settings.basePath + 'chatroomread.php',
    dataType: 'json',
    success: Drupal.chatroom.pollHandler,
    data: {
      latest_msg_id: Drupal.settings.chatroom.latestMsgId,
      chat_cache_directory: Drupal.settings.chatroom.cacheDirectory,
      chat_id: Drupal.settings.chatroom.chatId,
      skip_cache: skipCacheCheck,
      successive_cache_hits: Drupal.settings.chatroom.successiveCacheHits
    }
  });
}

Drupal.chatroom.pollHandler = function(response, responseStatus) {
  if (!response) {
    return;
  }

  // If the user was kicked or banned, get them out of here.
  if (response.data.accessDenied) {
    window.location = Drupal.settings.basePath + Drupal.settings.chatroom.accessDeniedPath + '/' + Drupal.settings.chatroom.chatId + '/' + response.data.accessDenied;
  }

  // If the chat was archived, reload the page.
  if (response.data.archived) {
    window.location = Drupal.settings.basePath + Drupal.settings.chatroom.chatPath;
  }

  // If we hit the cache, then keep track of that. If the number of
  // successive cache hits gets high enough, we may want to signal to the
  // server that we should skip the cache check so that our online time
  // gets updated.
  if (response.data.cacheHit) {
    Drupal.settings.chatroom.successiveCacheHits++;
  }
  else {
    Drupal.settings.chatroom.successiveCacheHits = 0;
  }

  if (response.data.messages) {
    Drupal.chatroom.addMessagesToBoard(response.data.messages);
  }

  if (response.data.usersHtml) {
    Drupal.chatroom.updateUserList(response.data.usersHtml);
  }

  if (response.data.commandResponse) {
    Drupal.chatroom.addCommandMessage(response.data.commandResponse);
  }
}

Drupal.chatroom.updateUserList  = function(usersHtml) {
  $('#chatroom-user-list-wrapper').replaceWith(usersHtml);
  Drupal.attachBehaviors('#chatroom-user-list-wrapper');
}

Drupal.chatroom.addMessagesToBoard = function(messages) {
  var newMessage = false;
  for (var i = 0; i < messages.length; i++) {
    // Poll requests can pass each other over the wire, so we can't rely on
    // getting a given message once only, so only add if we haven't already
    // done so.
    if (messages[i].cmid > Drupal.settings.chatroom.latestMsgId) {
      Drupal.settings.chatroom.latestMsgId = messages[i].cmid;
      $('#chatroom-board').append(messages[i].html);
      newMessage = messages[i];
      if (messages[i].newDayHtml) {
        $('#chatroom-board').append(messages[i].newDayHtml);
      }
    }
  }
  if (newMessage) {
    Drupal.chatroom.scrollToLatestMessage();
    if (Drupal.settings.chatroom.hasFocus == false) {
      Drupal.settings.chatroom.newMsg = newMessage;
      clearInterval(Drupal.settings.chatroom.warnInterval);
      Drupal.settings.chatroom.warnInterval = setInterval("Drupal.chatroom.warnNewMsgLoop()", 1500);
    }
  }
}

Drupal.chatroom.addCommandMessage = function(response) {
  $('#chatroom-board').append('<div class="new-message command-message">** ' + response.msg + '</div>');
  Drupal.chatroom.scrollToLatestMessage();
}

Drupal.chatroom.addCommandMessage = function(response) {
  $('#chatroom-board').append('<div class="new-message command-message">** ' + response.msg + '</div>');
  Drupal.chatroom.scrollToLatestMessage();
}

Drupal.chatroom.scrollToLatestMessage = function() {
  var boardOffset = $('#chatroom-board').offset().top;
  var targetOffset = $('div.new-message:last').offset().top;
  var scrollAmount = targetOffset - boardOffset;
  $('#chatroom-board').animate({scrollTop: '+='+ scrollAmount +'px'}, 500);
  $('.new-message').removeClass('new-message');
}

Drupal.chatroom.postMessage = function(message, anonName) {
  // Fix the resolution of the form tokens
  $.ajax({
    type: 'POST',
    url: Drupal.settings.basePath + Drupal.settings.chatroom.postMessagePath + '/' + Drupal.settings.chatroom.chatId + '/' + Drupal.settings.chatroom.latestMsgId,
    dataType: 'json',
    success: Drupal.chatroom.pollHandler,
    data: {
      message: message,
      anonName: anonName,
      formToken: $('#edit-chatroom-chat-buttons-form-token').val(),
      formId: 'chatroom_chat_buttons'
    }
  })
}

Drupal.chatroom.warnNewMsgLoop = function() {
  if (document.title == Drupal.settings.chatroom.pageTitle) {
    document.title = Drupal.settings.chatroom.newMsg.name_stripped + ' says: ' + Drupal.settings.chatroom.newMsg.text;
  }
  else {
    document.title = Drupal.settings.chatroom.pageTitle;
  }
}

})(jQuery);

// vi:ai:expandtab:sw=2 ts=2

