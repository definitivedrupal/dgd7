
(function ($) {

Drupal.Nodejs.callbacks.chatroomNodejsMessageHandler = {
  callback: function (message) {
    switch (message.type) {
      case 'newMessage':
        Drupal.chatroom.addMessagesToBoard([message.data]);
        break;
      case 'userSeen':
        Drupal.chatroom.updateUserList(message.data);
        break;
      case 'newCommandMessage':
        Drupal.chatroom.addCommandMessage(response.data);
        break;
    }
  }
};

})(jQuery);

