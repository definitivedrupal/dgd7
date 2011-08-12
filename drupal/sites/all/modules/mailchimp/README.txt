This module provides integration with the MailChimp email delivery service.
While tools for sending email from your own server, like SimpleNews, are great,
they lack the sophistication and ease of use of dedicated email providers like
MailChimp. Other players in this field are Constant Contact and Campaign Monitor.

Features
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
  * Send all your site emails through the MailChimp STS API and see all of
    those email statistics.

Installation Notes
  * You need to have a MailChimp API Key.
  * You need to have at least one list created in MailChimp to use the
    mailchimp_list module.
  * The mailchimp_sts module requires a paid MailChimp account (as of this
    writing) as well as an account with Amazon AWS. MailChimp STS is a wrapper
    arround Amazon SES, so all SES requirements apply.

What each module does
  * mailchimp_module: Core integration with the MailChimp API. Required by all
    other modules and a tool for developers.
  * mailchimp_lists: Synchronize Drupal users with MailChimp lists and allow
    users to subscribe, unsubscribe, and update member information.
  * mailchimp_sts: Expose the MailChimp STS gateway as a Drupal mail interface
    and associated settings.