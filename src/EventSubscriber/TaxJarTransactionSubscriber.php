<?php

namespace Drupal\commerce_taxjar\EventSubscriber;

use Drupal\commerce_order\Adjustment;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\state_machine\Event\WorkflowTransitionEvent;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;

class TaxJarTransactionSubscriber implements EventSubscriberInterface {

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * Constructs a new CommitTransactionSubscriber object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   *
   * @throws \Drupal\Component\Plugin\Exception\InvalidPluginDefinitionException
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager) {
    $this->entityTypeManager = $entity_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function getSubscribedEvents() {
    $events = [
      'commerce_order.place.post_transition' => ['saveTransaction'],
    ];
    return $events;
  }

  /**
   * Saves a transaction to TaxJar.
   *
   * @param \Drupal\state_machine\Event\WorkflowTransitionEvent $event
   *   The workflow transition event.
   */
  public function saveTransaction(WorkflowTransitionEvent $event) {
    $order = $event->getEntity();

    $taxjar_data = $order->getData('taxjar');

    if (!empty($taxjar_data)) {
      $tax_type = $this->entityTypeManager->getStorage('commerce_tax_type')->load($taxjar_data['plugin_id']);
      $plugin = $tax_type->getPlugin();

      if ($plugin->getConfiguration()['enable_reporting']) {
        $plugin->createTransaction($order);
      }
    }
  }

}
