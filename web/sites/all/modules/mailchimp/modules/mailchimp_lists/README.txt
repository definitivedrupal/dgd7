Synchronize Drupal users with MailChimp lists and allow users to subscribe, 
unsubscribe, and update member information. This module requires the
[Entity module](http://www.drupal.org/project/entity).

## Installation

1. Enable the MailChimp Lists module and the Entity Module

2. To use MailChimp Lists module, you will need to install and enable the Entity
API module [http://drupal.org/project/entity]([http://drupal.org/project/entity)

3. If you haven't done so already, add a list in your MailChimp account. Follow 
these directions provided by MailChimp on how to 
[add or import a list](http://kb.mailchimp.com/article/how-do-i-create-and-import-my-list)
or [watch this video](http://bcove.me/n9bh6ek9) created by MailChimp

4. Direct your browser to: http://example.com/admin/config/services/mailchimp 
You will now see a "Lists and Users" tab. http://example.com/admin/config/services/mailchimp/lists


## Usage

Adding a list - click "Add a List". List settings vary depending on the type of 
list being created. Lists are now exportable and have a machine name, so play
nice with features or your own codified configurations.

### Required lists

Required lists are automatically synchronized with the sites users. You can 
choose the following settings for the required lists:

* Sync List During Cron
If this is set, users will be subscribed to the required list during cron runs. 
If you do not select this option, subscription will take place when a user is 
added/edited.

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
an user updates their info via Drupal, via a hook_user event, or when a web hook 
event updates it. Also important to note that the web hook doesn't just clear 
it, it actually updates the cached data. *Note: You cannot test webhooks if 
developing locally, the system can't access your local computer.*


### Optional lists

Optional lists provide a checkbox allowing users to subscribe during 
registration or when updating their account. They have the following settings:

* Require subscribers to Double Opt-in
New subscribers will be sent a link with an email from MailChimp that they must 
follow to confirm their subscription. 

* Show subscription options on the user registration form.
This will only apply for lists granted to an authenticated role. 

* Show Subscription Options on User Edit Screen
If set, a tab will be presented for managing newsletter subscriptions when 
editing an account. Here the user can subscribe and unsubscribe from the 
MailChimp lists to which they belong.

* Include interest groups on subscription form
If set, users will be able to select applicable interest groups when registering 
or editing their accounts. Interest Groups are set up in MailChimp. Read more 
about [MailChimp Groups](http://mailchimp.com/features/groups/). If your groups 
are not showing up on your subscribe form in Drupal, you may need to clear your 
cache.

* Webhooks, see above.

Creating an Optional list will provide you with a block called Mailchimp 
Subscription Form: [list title].

### Free form lists 

This is the only type of list allowed for the Anonymous role and has the 
following options.

* Require subscribers to Double Opt-in
New subscribers will be sent a link with an email from MailChimp that they must 
follow to confirm their subscription.

* Include interest groups on subscription form.
If set, users will be able to select applicable interest groups when registering 
or editing their accounts. Interest Groups are set up in MailChimp. Read more 
about [MailChimp Groups](http://mailchimp.com/features/groups/).

If your groups are not showing up on your subscribe form in Drupal, you may need 
to clear your cache.

Creating an Free From list will provide you with a block called Mailchimp 
Subscription Form: [list title]. This block contains a signup form with all 
MailChimp merge fields displayed. 

Default values are allowed for authenticated users based on token mappings. 
There for if you map First Name to username, the module will fill in that 
information from the database.
