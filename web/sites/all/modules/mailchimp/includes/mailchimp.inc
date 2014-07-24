<?php

class MailChimp extends MCAPI {

  /**
   * Override MCAPI::callServer() to leverage Drupal's core HTTP handling.
   */
  public function callServer($method, $params) {
    $dc = "us1";
    if (strstr($this->api_key, "-")) {
      list($key, $dc) = explode("-", $this->api_key, 2);
      if (!$dc) {
        $dc = "us1";
      }
    }

    $host = $dc . "." . $this->apiUrl["host"];
    $protocol = $this->secure ? 'https://' : 'http://';
    $params["apikey"] = $this->api_key;

    $this->errorMessage = "";
    $this->errorCode = "";
    $post_vars = http_build_query($params, NULL, '&');

    $response = drupal_http_request(
      $protocol . $host . $this->apiUrl["path"] . '?method=' . $method,
      array(
        'method' => 'POST',
        'data' => $post_vars,
        'headers' => array(
          'Content-type' => 'application/x-www-form-urlencoded',
          'Host' => $host,
          'Accept-Language' => language_default()->language,
          'User-Agent' => 'MCAPI/' . $this->version,
        ),
        'timeout' => $this->getTimeout(),
      )
    );

    if (!empty($response->error)) {
      $this->errorMessage = $response->error;
      $this->errorCode = $response->code;
      return FALSE;
    }

    foreach ($response->headers as $h) {
      if (substr($h, 0, 26) === "X-MailChimp-API-Error-Code") {
        $this->errorMessage = "No error message was found";
        $this->errorCode = trim(substr($h, 27));
        return FALSE;
      }
    }

    $return = drupal_json_decode($response->data);
    if ($return === FALSE) {
      $this->errorMessage = "Bad Response.  Got This: " . $response->data;
      $this->errorCode = -99;
      return FALSE;
    }

    if (isset($return['error']) && is_array($return)) {
      $this->errorMessage = $return['error'];
      $this->errorCode = $return['code'];
      return FALSE;
    }

    return $return;
  }

}
