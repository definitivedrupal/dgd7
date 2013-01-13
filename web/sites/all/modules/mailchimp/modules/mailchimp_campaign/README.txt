Integrates with MailChimp campaigns, enabling creating and sending campaigns,
and viewing statistics on those already sent.

## Creating Campaigns
1. Click Add a Campaign in the overview
2. Fill out the required fields
  * **Title:** An internal name to identify the campaign
  * **Subject:** Message subject
  * **List:** The MailChimp list to use
  * **From Email:** Email address for the campaign
  * **From Name:** Name for the campaign messages
  * **Template:** MailChimp template to use
  * **Content Sections:** Each section (if applicable) of content for the
  campaign
     * **Text format:** Choose between MailChimp campaign and Plain Text (see
     _Using MailChimp Macros_)
3. Click Save as draft

Once saved as a draft, the new campaign will be available in both Drupal and
MailChimp as a draft until it is sent. After sending, campaigns can no longer
be edited.

## Using MailChimp Macros
This module includes a special text format called _MailChimp campaign_,
allowing the use of macros to insert Drupal content into campaigns.

### Format
`[mailchimp_campaign|entity_type=node|entity_id=1|view_mode=teaser]`

#### Options

* The first item, `mailchimp_campaign`, identifies the macro
* `entity_type` specifies which type of entity to use
* `entity_id` identifies the entity by ID
* `view_mode` specifies which view mode to use

### Usage
Macros may be inserted into any text area that is set to use the _MailChimp
campaign_ text format. Upon saving a campaign, macros will automatically be
replaced with the specified node content.

## Troubleshooting
### Campaigns, lists, or templates have not updated from MailChimp
  Try clearing the Drupal cache and reloading the page.
### Cannot edit a campaign
  Check to see if the campaign has already been sent. Sent campaigns cannot be
  edited.