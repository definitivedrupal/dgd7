$Id: README.txt,v 1.3.4.2 2010/09/17 18:17:40 tristanoneil Exp $


Mark module for Drupal
---------------------------------------

The Mark module allows users to "mark" any kind of content that is accessible
to Views. The module a View field that can be used on any type of view.

Building Views-powered listing pages that show lists of "marked" pages does
require familiarity with using views "relationships". Mark doesn't provide any
specialized views capability beyond what is already possible with votingapi,
which can be difficult to understand at first but is very powerful.


Usage
---------------------------------------

Marks can be created at "Administer > Structure > Mark" (aka 
`admin/structure/mark`).  Each mark must declare what type of items it can
reference and what messages it should present to end users. Permission to
assign marks is determined by whether a uses has the 'mark content'
permission. 

Once a mark has been created a UI to access it must be exposed to the user.
Mark provides views field (for any type) for this
purpose.

Mark is designed to be used by authenticated users. It's possible to assign the
proper permissions for marking content to anonymous users, but the behavior of
the module in such circumstances is "unspecified", at best.

Security note: Even users who have the "mark content " permission  are
prevented from just marking any content by token which is generated based on
the content type, content id and user's session.

Creating views of marked content requires usage of "relationships". By way of
a quick example; a view that shows users who have "marked" nodes would created
need the following basic setup:

1. Make a new view of type node.
2. Add the "Node: Votes" relationship (require this).
3. Add the "Votes: User" relationship.

Getting the exact display desired would clearly require additional setup.


Installation
---------------------------------------

The Mark module requires no unusual steps and should be installed like any
other Drupal module.

Mark depends on three other Drupal modules: Views[1], Choas tools[2] and
Votingapi[3].

[1]: http://drupal.org/project/views
[2]: http://drupal.org/project/ctools
[3]: http://drupal.org/project/votingapi
