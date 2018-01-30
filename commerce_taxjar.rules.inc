<?php

/**
 * @file
 * Rules supporting TaxJar integration.
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_taxjar_rules_action_info() {
  return array(
    'commerce_taxjar_order_save' => array(
      'label' => t('Save an order transaction to TaxJar'),
      'group' => t('Commerce TaxJar'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
        ),
      ),
    ),
    'commerce_taxjar_order_update' => array(
      'label' => t('Update an order transaction in TaxJar'),
      'group' => t('Commerce TaxJar'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
        ),
      ),
    ),
    'commerce_taxjar_order_refund' => array(
      'label' => t('Refund an order transaction in TaxJar'),
      'group' => t('Commerce TaxJar'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
        ),
      ),
    ),
    'commerce_taxjar_order_delete' => array(
      'label' => t('Delete an order transaction in TaxJar'),
      'group' => t('Commerce TaxJar'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
        ),
      ),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function commerce_taxjar_rules_condition_info() {
  return array(
    'commerce_order_contains_taxjar_line_item' => array(
      'label' => t('Order contains a tax line item'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
          'wrapped' => TRUE,
        ),
      ),
      'group' => t('Commerce TaxJar'),
      'callbacks' => array(
        'execute' => 'commerce_taxjar_rules_contains_tax_line',
      ),
    ),
  );
}

/**
 * Check whether an order has a tax line item.
 *
 * @param EntityDrupalWrapper $order_wrapper
 *   The wrapped order entity.
 *
 * @return bool
 *   Whether a tax line item exists.
 */
function commerce_taxjar_rules_contains_tax_line(EntityDrupalWrapper $order_wrapper) {
  foreach ($order_wrapper->commerce_line_items as $line_item_wrapper) {
    if ($line_item_wrapper->getBundle() == 'taxjar') {
      return TRUE;
    }
  }
  return FALSE;
}
