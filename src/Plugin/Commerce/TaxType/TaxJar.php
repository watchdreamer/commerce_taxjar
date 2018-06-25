<?php

namespace Drupal\commerce_taxjar\Plugin\Commerce\TaxType;

use Drupal\commerce_taxjar\ClientFactory;
use Drupal\commerce_order\Entity\OrderInterface;
use Drupal\commerce_order\Adjustment;
use Drupal\commerce_price\Price;
use Drupal\commerce_tax\Plugin\Commerce\TaxType\RemoteTaxTypeBase;
use Drupal\Component\Serialization\Json;
use Drupal\Component\Utility\Html;
use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Logger\LoggerChannelFactoryInterface;
use GuzzleHttp\Exception\ClientException;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\EventDispatcher\EventDispatcherInterface;

/**
 * Provides the TaxJar remote tax type.
 *
 * @CommerceTaxType(
 *   id = "taxjar",
 *   label = "TaxJar",
 * )
 */
class TaxJar extends RemoteTaxTypeBase {

  /**
   * The client.
   *
   * @var \GuzzleHttp\Client
   */
  protected $client;

  /**
   * The module handler.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

  /**
   * The logger.
   *
   * @var \Drupal\Core\Logger\LoggerChannelInterface
   */
  protected $logger;

  /**
   * Constructs a new TaxJar object.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   * @param \Symfony\Component\EventDispatcher\EventDispatcherInterface $event_dispatcher
   *   The event dispatcher.
   * @param \Drupal\commerce_taxjar\ClientFactory $client_factory
   *   The client.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param \Drupal\Core\Logger\LoggerChannelFactoryInterface $logger_factory
   *   The logger.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityTypeManagerInterface $entity_type_manager, EventDispatcherInterface $event_dispatcher, ClientFactory $client_factory, ModuleHandlerInterface $module_handler, LoggerChannelFactoryInterface $logger_factory) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $entity_type_manager, $event_dispatcher);
    $this->client = $client_factory->createInstance($this->configuration);
    $this->moduleHandler = $module_handler;
    $this->logger = $logger_factory->get('commerce_taxjar');
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity_type.manager'),
      $container->get('event_dispatcher'),
      $container->get('commerce_taxjar.client_factory'),
      $container->get('module_handler'),
      $container->get('logger.factory')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return [
      'display_inclusive' => FALSE,
      'api_key' => '',
      'sandbox_key' => '',
      'api_mode' => 'production',
      'enable_reporting' => TRUE,
    ] + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $form = parent::buildConfigurationForm($form, $form_state);

    $form['api_mode'] = [
      '#type' => 'radios',
      '#title' => $this->t('API mode:'),
      '#default_value' => $this->configuration['api_mode'],
      '#options' => [
        'production' => $this->t('Production'),
        'sandbox' => $this->t('Sandbox'),
      ],
      '#required' => TRUE,
      '#description' => $this->t('The mode to use when calculating taxes. Note: Sandbox mode is only available with TaxJar Plus.'),
    ];

    $form['api_key'] = [
      '#type' => 'textfield',
      '#title' => $this->t('API Token'),
      '#default_value' => $this->configuration['api_key'],
      '#required' => TRUE,
      '#description' => $this->t('Enter your TaxJar API token. If you do not have a token, <a href="@login" target="_blank">login</a> and go to Account > API Access to generate a new one.', array('@login' => 'https://app.taxjar.com')),
    ];

    $form['sandbox_key'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Sandbox API Token'),
      '#default_value' => $this->configuration['sandbox_key'],
      '#description' => $this->t('Enter your sandbox API token for testing.'),
      '#states' => [
        'visible' => [
          ':input[name="configuration[taxjar][api_mode]"]' => ['value' => 'sandbox'],
        ],
        'required' => [
          ':input[name="configuration[taxjar][api_mode]"]' => ['value' => 'sandbox'],
        ],
      ],
    ];

    $form['enable_reporting'] = [
      '#type' => 'checkbox',
      '#title' => t('Use TaxJar for sales tax reporting'),
      '#description' => t('Record order transactions to TaxJar for automated reporting / filing.'),
      '#default_value' => $this->configuration['enable_reporting'],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    parent::submitConfigurationForm($form, $form_state);
    $values = $form_state->getValue($form['#parents']);
    $this->configuration['api_mode'] = $values['api_mode'];
    $this->configuration['api_key'] = $values['api_key'];
    $this->configuration['sandbox_key'] = $values['sandbox_key'];
    $this->configuration['enable_reporting'] = (bool) $values['enable_reporting'];
  }

  /**
   * {@inheritdoc}
   */
  public function apply(OrderInterface $order) {
  }

  /**
   * Get the API client, configured for this instance.
   *
   * @return \GuzzleHttp\Client
   */
  public function getClient() {
    return $this->client;
  }

}
