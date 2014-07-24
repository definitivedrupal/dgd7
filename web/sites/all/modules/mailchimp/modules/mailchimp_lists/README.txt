Synchronize Drupal users with MailChimp lists and allow users to subscribe, 
unsubscribe, and update member information. This module requires the
[Entity module](http://www.drupal.org/project/entity).

## Installation

1. Enable the MailChimp Lists module and the Entity Module

2. To use MailChimp Lists module, you will need to install and enable the Entity
API module [http://drupal.org/project/entity]([http://drupal.org/project/entity)

3. If you haven't done so already, add a list in your MailChimp account. Follow 
these directions provided by MailChimp on how to 
[add or import a list](http://kb.mailchimp.com/article/how-do-i-create-and-import-my-list).

4. Direct your browser to: http://example.com/admin/config/services/mailchimp 
You will now see a "Lists and Users" tab. http://example.com/admin/config/services/mailchimp/lists


## Usage

Adding a list - click "Add a List". Lists are exportable and have a machine
name, so will play nice with features or your own codified configurations. Each
list is also given a unique Subscription block, which you can place as desired.
The block names match the labels you provide to your list, and they render
intelligently based on the User and the List settings (see below). 

### Mailchimp List & Merge Fields

* MailChimp List
The dropdown is populated with options based on the lists that exist on your
MailChimp account. If you see no lists here, make sure you have created lists in
your MailChimp account, and try clicking "Refresh Lists from MailChimp" on the
"Lists and Users" tab. 

* Merge Fields
You will see Merge Field options based on the configuration of your list through
MailChimp. You can match these fields up to fields on Drupal Users to keep them
synced. 

### Settings

* Allow anonymous registration
Checking this box will allow anonymous visitors to your site to input their
email address and any merge field options you select to add themselves to a list
via the List's block, without creating a Drupal Account. If this option is not
checked, Anonymous Users will not see the List's registration block. If it is,
they see a version of the block with an Email input field, and any Merge Fields
that you specify in the Form & Subscribe Block Options (below). 

* Automatically add all eligible users
This option will automatically synchronize the List with the site's Users based
on the Roles specified in the Roles section. When enabled, the Block for this
list will present Interest Group options only to logged-in Users. Anonymous
Users will see the normal Anonymous registration block if that option is also
enabled for the list. 

* Roles
Configures which users are allowed to join this list. If "Automatically Add" is
enabled, this instead configures which users are _forced_ to be added to this
list. The Anonymous Role is hidden, but is controlled by the "Allow Anonymous"
setting above. 

* Require subscribers to Double Opt-in
New subscribers will be sent a link with an email from MailChimp that they must 
follow to confirm their subscription. 

* Show subscription options on the user registration form.
This will only apply for lists when the "Authenticated User" role is selected
under Roles. 

* Show subscription options on the user edit screen.
Presents a tab for managing newsletter subscriptions when editing an account.
The user can subscribe and unsubscribe from the MailChimp lists to which they
belong, or configure interest groups, based on other settings. 

* Sync List During Cron
If this is set, subscription changes will take place during cron runs.
Otherwise, subscription changes all take place immediately. 

* Enable MailChimp webhooks for this list
When a user unsubscribes from a list or updates their profile outside of Drupal, 
MailChimp will trigger an event to update the user's cached MailChimp member 
information. This will not update any of their Drupal user information. Web 
hooks are the only way to maintain a two-way sync with your lists. Any 
information updated outside of the Drupal environment, e.g., email footer, 
another website, MailChimp site directly, etc. will be trigger an update of the 
cached member information within Drupal. This cached data means Drupal doesn't 
have to contact MailChimp every time it wants to determine a user's status, get 
their other info, etc. Generally this should be enabled if possible. Otherwise, 
lists could get out of sync. Cron runs have no impact on cached member data. 
Member info is loaded and cached the first time it's needed and is cleared when 
a user updates their info via Drupal, via a hook_user event, or when a web hook 
event updates it. It is also important to note that the web hook doesn't just
clear it, but actually updates the cached data. *Note: You cannot test webhooks
if developing locally, as the system can't access your local computer.* 

### Form & Subscribe Block Options

* List Label
This is the label used for the list in Blocks and Forms, differentiating the
labeling in the administrative interface from what you present to your Users. If
you leave this blank, your forms might look quite strange.

* Submit Button Label
The text to attach to the "subscribe" button on blocks. This does not apply to
screens where multiple lists are displayed on a single form: just on the
individual form blocks.

* Default to opt-in on registration form
Auto-checks the subscribe checkbox for this list on the user registration form.

* Include interest groups on subscription form
If set, users will be able to select applicable interest groups when registering 
or editing their accounts. Interest Groups are set up in MailChimp. Read more 
about [MailChimp Groups](http://mailchimp.com/features/groups/). If your groups 
are not showing up on your subscribe form in Drupal, you may need to clear your 
cache.

* Merge Field Form Display Settings
This option does not appear unless "Allow anonymous" is checked. This controls
which of the MailChimp List's Merge Fields show up on the Anonymous subscription
block for this list. Fields marked as Required in MailChimp show up
automatically, so you'll only see non-required fields here for configuration.
