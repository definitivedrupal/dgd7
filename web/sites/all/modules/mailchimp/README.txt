This module provides integration with the MailChimp email delivery service.
While tools for sending email from your own server, like SimpleNews, are great,
they lack the sophistication and ease of use of dedicated email providers like
MailChimp. Other players in the field are Constant Contact and Campaign Monitor.

mailchimp.module provides provides basic configuration and API integration. 
Specific functionality is provided by a set of submodules that depend upon 
mailchimp.module. See their respective README's for more details.

## Features
  * API integration
  * Support for an unlimited number of mailing lists
  * Having an anonymous sign up form to enroll users in a general newsletter.
  * Each MailChimp list can be assigned to one or more roles
  * Editing of user list subscriptions on the user's edit page
  * Allow users to subscribe during registration
  * Map token and profile values to your MailChimp merge fields
  * Required, optional, and free form list types.
  * Standalone subscribe and unsubscribe forms
  * Subscriptions can be maintained via cron or in real time
  * Individual blocks for each newsletter
  * Create campaigns containing any Drupal entity as content, send them, and
    view statisitcs.
  * Campaign activity for any entity with an email address.

## Installation Notes
  * You need to have a MailChimp API Key.
  * You need to have at least one list created in MailChimp to use the
    mailchimp_list module.
  * For versions 7.x-2.8 and greater, the MCAPI library must be downloaded into
    your libraries folder. IT's available at http://apidocs.mailchimp.com/api/downloads/mailchimp-api-class.zip
    or by using the included example make file. Proper libraries structure:
    - libraries
      - mailchimp
        - MCAPI.class.php
        - README
    The only required files are MCAPI.class.php and README.

## Configuration
  1. Direct your browser to http://example.com/admin/config/services/mailchimp 
  to configure the module.

  2. You will need to put in your Mailchimp API key for your Mailchimp account.
  If you do not have a Mailchimp account, go to 
  [http://www.mailchimp.com]([http://www.mailchimp.com) and sign up for a new 
  account. Once you have set up your account and are logged into your account, 
  Select "API keys and Authorized Apps" from the Account dropdown menu.

  3. Click Add a Key.
  Copy your newly create API key and go to the 
  [Mailchimp config](http://example.com/admin/config/services/mailchimp) page in 
  your Drupal site and paste it into the Mailchimp API Key field. 
  Batch limit - Maximum number of users to process in a single cron run. 
  Mailchimp suggest keeping this below 5000-10000. Use Secure Connection - 
  Communicate with the MailChimp API over a secure connection.

## Submodules
  * mailchimp_lists: Synchronize Drupal users with MailChimp lists and allow
    users to subscribe, unsubscribe, and update member information.
  * mailchimp_campaigns: Create newsletters with Drupal content, send the
    campaigns, and view statistics.
  * mailchimp_activity: View campaign activity for any entity with an email
    address.

## Related Modules
### Mandrill
  * IMPORTANT: The STS submodule has been removed as it's being deprecated by
    MailChimp in favor of their new Mandrill service. STS has been moved into
    it's own project (http://drupal.org/project/mailchimp_sts) for those already
    using the service. New projects and those willing to make the change should
    definitely use Mandrill.
  * Mandrill is MailChimp's new transactional email service. The initial version
    of the module provides the ability to send all site emails through Mandrill
    with reporting available from within Drupal. Please refer to the project
    page for more details.
  * http://drupal.org/project/mandrill
### MCC, an alternative campaign creation tool.
  * http://drupal.org/project/mcc
