<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Allows modules to alter the create tax quote request before its sent to the
 * TaxJar API.
 *
 * @param array $request_body
 *   The request body array.
 * @param object $order
 *   The order object.
 *
 * @see commerce_taxjar_create_tax_request()
 */
function hook_commerce_taxjar_tax_request_alter(array &$request_body, $order) {
}

/**
 * Allows modules to alter the create transaction request before its sent to the
 * TaxJar API.
 *
 * @param array $request_body
 *   The request body array.
 * @param object $order
 *   The order object.
 *
 * @see commerce_taxjar_create_transaction_request()
 */
function hook_commerce_taxjar_transaction_request_alter(array &$request_body, $order) {
}
