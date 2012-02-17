<?php

/**
 * @file
 * Implements MailChimp STS as a Drupal MailSystemInterface
 */

/**
 * Modify the drupal mail system to use MailChimp STS when sending emails.
 */
class MailChimpSTSMailSystem implements MailSystemInterface {

  /**
   * Concatenate and wrap the email body for either
   * plain-text or HTML emails.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return
   *   The formatted $message.
   */
  public function format(array$message) {
    // Join the body array into one string.
    if (is_array($message['body'])) {
      $message['body'] = implode("\n\n", $message['body']);
    }
    return $message;
  }

  /**
   * Send the email message.
   *
   * @see drupal_mail()
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return
   *   TRUE if the mail was successfully accepted, otherwise FALSE.
   */
  public function mail(array$message) {
    if (!$from = variable_get('mailchimp_sts_from', '')) {
      drupal_set_message(t('MailChimp STS can\'t send email. Please !link.',
          array('!link' => l('add a verified from address',
              'admin/config/services/mailchimp/sts'
            ))
        ), 'error');
      return FALSE;
    }

    $to = mailchimp_sts_get_to($message['to']);

    // apply input format to body
    $html = $message['body'];
    $format = variable_get('mailchimp_sts_filter_format', '');
    if (!empty($format)) {
      $html = check_markup($message['body'], $format);
    }

    $sts_message = array(
      'html' => $html,
      'text' => drupal_html_to_text($message['body']),
      'subject' => $message['subject'],
      //'from_name' => ,
      'from_email' => $from,
      'to_email' => $to,
      //'to_name' => ,
      //'autogen_html' =>
    );

    // Handle custom headers
    foreach ($message['headers'] as $name => $value) {
      if ($name == 'Reply-To') {
        $sts_message['reply_to'] = $value;
      }
    }

    // send the email passing the message id as the tag for use in reporting
    $mailer = mailchimp_sts_get_api_object();
    $status = $mailer->send_email($sts_message, TRUE, TRUE, array($message['id']));

    if (isset($status->aws_code) && !empty($status->aws_code)) {
      watchdog('mailchimp_sts', 'Error sending email from %from to %to. @code: @message',
        array(
          '%from' => $from, '%to' => print_r($to, TRUE),
          '@code' => $status->aws_code, '@message' => $status->message,
        ),
        WATCHDOG_ERROR
      );
      return FALSE;
    }

    return TRUE;
  }
}

/**
 * A mail sending implementation that captures sent messages to watchdog.
 *
 * This class is for running tests or for development.
 */
class TestingMailChimpSTSMailSystem extends MailChimpSTSMailSystem implements MailSystemInterface {

  /**
   * Accept an e-mail message and store it in a variable.
   *
   * @param $message
   *   An e-mail message.
   */
  public function mail(array$message) {
    if (!$from = variable_get('mailchimp_sts_from', '')) {
      drupal_set_message(t('MailChimp STS can\'t send email. Please !link.',
          array('!link' => l('add a verified from address',
              'admin/config/services/mailchimp/sts'
            ))
        ), 'error');
      return FALSE;
    }
    $to = mailchimp_sts_get_to($message['to']);

    watchdog('mailchimp_sts', 'MailChimp STS test email sent from %from to %to. Message: @message',
      array(
        '%from' => $from, '%to' => implode(', ', $to),
        '@message' => print_r($message),
      ),
      WATCHDOG_NOTICE
    );
    drupal_set_message(t('MailChimp STS test email sent from %from to %to.',
        array('%from' => $from, '%to' => implode(', ', $to))
      ), 'notice');
    return TRUE;
  }
}

/**
 * Helperl to generate an array of recipients.
 *
 * @param mixed $to
 *
 * @return array
 */
function mailchimp_sts_get_to($to) {
  return (count(explode(',', $to) > 1) ? explode(',', $to) : $to);
}

