
Chat Room allows users to talk together in real time. Users enter chat rooms
which can have multiple chats. Permissions can be set to restrict who can
access, edit, and create chats and chat rooms.

INSTALLATION
============

1. Download the module from Drupal.org/project/chatroom and save it to your
   modules folder.
2. Enable the module at admin/build/modules.
3. Copy chatroomread.php from the chatroom folder to the webroot of your site.
   chatroomread.php should sit next to your index.php file.
4. If your module folder is somewhere other than 'sites/all/modules/chatroom',
   set the path to the folder in settings.php:
   $conf['chatroom_module_dir'] = 'path/to/the/chatroom/module/directory';
5. Enable access to chatrooms and chat for some roles on your site.

HACKING
=======
The DRUPAL-6--2 branch is stable, and is now bug-fix only. The DRUPAL-6--3 is 
where features are developed, please post feature requests against this branch. 
Features may be backported from DRUPAL-6--3 to DRUPAL-6--2, but this will be 
done on a case-by-case basis only.

Patches, testers, feature suggestions, bug reports etc all welcome!

