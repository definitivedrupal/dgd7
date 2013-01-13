
(function ($) {

Drupal.chatroom = Drupal.chatroom || {'initialised' : false, chats: {}};

Drupal.chatroom.initialiseChat = function(chat) {
  if (chat.latestMsgId > 0) {
    var targetOffset = $('#chatroom-board-' + chat.cid + ' div.new-message:last').offset().top;
    var boardOffset = $('#chatroom-board-' + chat.cid).offset().top;
    var scrollAmount = targetOffset - boardOffset;
    $('#chatroom-board-' + chat.cid).animate({scrollTop: '+='+ scrollAmount +'px'}, 250);
    $('#chatroom-board-' + chat.cid + '.new-message').removeClass('new-message');
  }

  $('#edit-chatroom-message-entry-box-' + chat.cid).keyup(function(e) {
    var chat = Drupal.settings.chatroom.chats[this.id.replace(/^edit-chatroom-message-entry-box-/, '')];
    var messageText = $('#edit-chatroom-message-entry-box-' + chat.cid).val().replace(/^\s+|\s+$/g, '');
    var anonNameText = '';
    if (messageText && e.keyCode == 13 && !e.shiftKey && !e.ctrlKey) {
      Drupal.chatroom.postMessage(messageText, anonNameText, chat);
      $('#edit-chatroom-message-entry-box-' + chat.cid).val('').focus();
    }
    else {
      return true;
    }
  });

  $('#edit-chatroom-message-entry-submit-' + chat.cid).click(function (e) {
    var chat = Drupal.settings.chatroom.chats[this.id.replace(/^edit-chatroom-message-entry-submit-/, '')];
    e.preventDefault();
    e.stopPropagation();
    var messageText = $('#edit-chatroom-message-entry-box-' + chat.cid).val().replace(/^\s+|\s+$/g, '');
    var anonNameText = '';
    if (messageText) {
      Drupal.chatroom.postMessage(messageText, anonNameText, chat);
      $('#edit-chatroom-message-entry-box-' + chat.cid).val('').focus();
    }
  });

  $('#chatroom-board-' + chat.cid).scroll(function() {
    var chat = Drupal.settings.chatroom.chats[this.id.replace(/^chatroom-board-/, '')];
    var yPos = $('#chatroom-board-' + chat.cid).scrollTop();
    if (yPos === 0) {
      Drupal.chatroom.getPreviousMessages(chat.cid, chat.prevMsgId);
    }
  });
}

/**
 * Provide js API functions for modules that want to create chatrooms
 * from js events.
 */
Drupal.chatroom.createChat = function(options, callback) {
  if (typeof options.uid === 'undefined') {
    options.uid = Drupal.settings.chatroom.uid;
  }
  if (typeof options.public === 'undefined') {
    options.public = 0;
  }
  $.ajax({
    type: 'POST',
    url: Drupal.settings.chatroom.createChatPath,
    dataType: 'json',
    success: function (data, textStatus, XHR) {
      callback(data.data);
    },
    data: {
      chatroom: options,
      formToken: Drupal.settings.chatroom.createChatToken,
      formId: Drupal.settings.chatroom.createChatFormId
    }
  });
}

/**
 * We depend on the Nodejs module successfully create a socket for us.
 */
Drupal.Nodejs.connectionSetupHandlers.chatroom = {
  connect: function () {
    for (var cid in Drupal.settings.chatroom.chats) {
      Drupal.chatroom.initialiseChat(Drupal.settings.chatroom.chats[cid]);
    }
    Drupal.chatroom.initialised = true;
  }
};

Drupal.chatroom.postMessage = function(message, anonName, chat) {
  var formId = 'chatroom_form_token_' + chat.cid;
  if (typeof chat.formId !== 'undefined') {
    formId = chat.formId;
  }
  var formTokenDomId = '#edit-chatroom-chat-buttons-form-token-' + chat.cid;
  if (typeof chat.formTokenDomId !== 'undefined') {
    formTokenDomId = chat.formTokenDomId;
  }
  $.ajax({
    type: 'POST',
    url: Drupal.settings.chatroom.postMessagePath + '/' + chat.cid,
    dataType: 'json',
    success: function () {},
    data: {
      message: message,
      anonName: anonName,
      formToken: $(formTokenDomId).val(),
      formId: formId
    }
  });
}

Drupal.chatroom.getPreviousMessages = function(cid, cmid, limit) {
  if (limit === undefined) limit = 20;

  $.ajax({
    type: 'GET',
    url: Drupal.settings.chatroom.chatroomPath + '/' + cid + '/messages/previous/' + cmid + '/' + limit,
    success: function(data) {
      /**
       * Shamelessly stolen and adapted from here:
       *
       * http://stackoverflow.com/questions/5688362/how-to-prevent-scrolling-on-prepend
       *
       * Better implementation ideas welcome.
       */
      var currentTop = $('#chatroom-board-' + cid).children().first();
      for (var message in data) {
        Drupal.chatroom.addPreviousMessageToBoard(data[message]);
      }
      var previousHeight = 0;
      currentTop.prevAll().each(function() {
        previousHeight += $(this).outerHeight();
      });
      $('#chatroom-board-' + cid).animate({scrollTop: '+='+ previousHeight +'px'}, 10);
    }
  });
}

Drupal.chatroom.updateUserList = function(message) {
  if ($('#chatroom-user-' + message.cid + '-' + message.uid).length == 0) {
    var userHtml = '<li style="display: none;" id="chatroom-user-'
                   + message.cid + '-' + message.uid + '"><a href="/user/'
                   + message.uid + '">' + message.name + '</a></li>';
    $(userHtml).hide().appendTo($('#chatroom-irc-user-list-' + message.cid)).show('normal');
  }
}

Drupal.chatroom.addMessageToBoard = function(message) {
  $('#chatroom-board-' + message.cid).append(message.msg);
  Drupal.chatroom.scrollToLatestMessage(message.cid);
}

Drupal.chatroom.addPreviousMessageToBoard = function(message) {
  var chat = Drupal.settings.chatroom.chats[message.cid];
  chat.prevMsgId = message.cmid;
  Drupal.settings.chatroom.chats[message.cid] = chat;
  $('#chatroom-board-' + message.cid).prepend(message.msg);
}

Drupal.chatroom.scrollToLatestMessage = function(cid) {
  var boardOffset = $('#chatroom-board-' + cid).offset().top;
  var targetOffset = $('#chatroom-board-' + cid + ' div.new-message:last').offset().top;
  var scrollAmount = targetOffset - boardOffset;
  $('#chatroom-board-' + cid).animate({scrollTop: '+='+ scrollAmount +'px'}, 250);
  $('#chatroom-board-' + cid + ' .new-message').removeClass('new-message');
}

Drupal.Nodejs.callbacks.chatroomMessageHandler = {
  callback: function (message) {
    Drupal.chatroom.addMessageToBoard(message.data);
  }
};

Drupal.Nodejs.callbacks.chatroomUserOnlineHandler = {
  callback: function (message) {
    Drupal.chatroom.updateUserList(message.data);
  }
};

Drupal.Nodejs.contentChannelNotificationCallbacks.chatroom = {
  callback: function (message) {
    if (message.data.type == 'disconnect') {
      var cid = message.channel.replace(/^chatroom_/, '');
      if ($('#chatroom-user-' + cid + '-' + message.data.uid).length == 1) {
        $('#chatroom-user-' + cid + '-' + message.data.uid).hide('normal', function () {
          $('#chatroom-user-' + cid + '-' + message.data.uid).remove();
        });
      }
    }
  }
};

})(jQuery);

// vi:ai:expandtab:sw=2 ts=2

