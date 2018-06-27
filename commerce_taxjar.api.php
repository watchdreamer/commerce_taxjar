<?php

use Drupal\commerce_order\Entity\OrderInterface;

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Allows modules to alter the tax quote request before it is sent to the
 * TaxJar API.
 *
 * @param array $request_body
 *   The request body array.
 * @param \Drupal\commerce_order\Entity\OrderInterface $order
 *   The order object.
 *
 */
function hook_commerce_taxjar_tax_request_alter(array &$request_body, OrderInterface $order) {
}

/**
 * Allows modules to alter the transaction request before its sent to the
 * TaxJar API.
 *
 * @param array $request_body
 *   The request body array.
 * @param \Drupal\commerce_order\Entity\OrderInterface $order
 *   The order object.
 *
 */
function hook_commerce_taxjar_transaction_request_alter(array &$request_body, OrderInterface $order) {
}
