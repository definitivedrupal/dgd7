;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Field Permissions module
;; $Id: README.txt,v 1.1.4.3 2009/11/15 16:04:50 markuspetrux Exp $
;;
;; Original author: markus_petrux (http://drupal.org/user/39593)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CONTENTS OF THIS FILE
=====================
* OVERVIEW
* USAGE
* REQUIREMENTS
* INSTALLATION


OVERVIEW
========

The Field Permissions module allows site administrators set field-level
permissions to edit or view CCK fields in any content, edit field during
content creation, and edit or view permissions for content owned by the
current user.

Permissions for each field are not created by default. Instead, administrators
can enable these permissions explicitly for the fields where this feature is
needed.


USAGE
=====

Once Field Permissions module is installed, you need to edit the field settings
form to enable permissions for each field where you need this feature. You can
enable any of the following permission types:

  * Create FIELD (edit on content creation).
  * Edit any FIELD, regardless of its content author.
  * Edit own FIELD on content created by the user.
  * View any FIELD, regardless of its content author.
  * View own FIELD on content created by the user.

Use these options to enable role based permissions for this field. When
permissions are enabled, access to this field is denied by default and explicit
permissions should be granted to the proper user roles from the permissions
administration page. On the other hand, when these options are disabled, field
permissions are inherited from content view and/or edit permissions. In example,
users allowed to view a particular node will also be able to view this field,
and so on.


REQUIREMENTS
============

- Field UI module (Drupal core).


INSTALLATION
============

- Be sure to install all dependent modules.

- Copy all contents of this package to your modules directory preserving
  subdirectory structure.

- Go to Administer -> Site building -> Modules to install module.

- Review the settings of your fields. You will find a new option labelled
  "Field permissions" that allows you to enable permissions per field. It
  is disabled by default.
