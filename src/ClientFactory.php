<?php

namespace Drupal\commerce_taxjar;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Http\ClientFactory as CoreClientFactory;

/**
 * API Client factory.
 */
class ClientFactory {

  protected $clientFactory;

  /**
   * Constructs a new TaxJar ClientFactory object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   * @param \Drupal\Core\Http\ClientFactory $client_factory
   *   The client factory.
   */
  public function __construct(ConfigFactoryInterface $config_factory, CoreClientFactory $client_factory) {
    $this->clientFactory = $client_factory;
  }

  /**
   * Gets an API client instance.
   *
   * @param array $config
   *   The config for the client.
   *
   * @return \GuzzleHttp\Client
   *   The API client.
   */
  public function createInstance(array $config) {
    switch ($config['api_mode']) {
      case 'production':
        $base_uri = 'https://api.taxjar.com/';
        $token = $config['api_key'];
        break;

      case 'development':
      default:
        $base_uri = 'https://api.sandbox.taxjar.com/';
        $token = $config['sandbox_key'];
        break;
    }

    $options = [
      'base_uri' => $base_uri,
      'headers' => [
        'Authorization' => 'Token token=' . $token,
        'Content-Type' => 'application/json',
      ],
    ];

    return $this->clientFactory->fromOptions($options);
  }

}
