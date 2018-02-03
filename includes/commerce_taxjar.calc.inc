<?php

/**
 * @file
 * TaxJar calculation/requests functions.
 */

/**
 * Prepares the tax request array to be sent to TaxJar.
 *
 * @param EntityDrupalWrapper $order_wrapper
 *   The wrapped order entity.
 * @param array $to_address
 *   Optionally supplied to address.
 *
 * @return array|bool
 *   Returns the request body array to be sent, FALSE in case of failure.
 */
function commerce_taxjar_create_tax_request(EntityDrupalWrapper $order_wrapper, array $to_address = []) {
  // Return FALSE in case there are no line items.
  if ($order_wrapper->commerce_line_items->count() === 0) {
    return FALSE;
  }

  // Prepare the Request Body.
  $request_body = _commerce_taxjar_prepare_request_body($order_wrapper, $to_address);

  if (empty($request_body)) {
    return FALSE;
  }

  _commerce_taxjar_request_add_lines($request_body, $order_wrapper->commerce_line_items);
  $order = $order_wrapper->value();
  drupal_alter('commerce_taxjar_tax_request', $request_body, $order);

  return $request_body;
}

/**
 * Prepares the transaction array to be sent to TaxJar.
 *
 * @param EntityDrupalWrapper $order_wrapper
 *   The wrapped order entity.
 *
 * @return array|bool
 *   Returns the request body array to be sent, FALSE in case of failure.
 */
function commerce_taxjar_create_transaction_request(EntityDrupalWrapper $order_wrapper) {
  // Return FALSE in case there are no line items.
  if ($order_wrapper->commerce_line_items->count() === 0) {
    return FALSE;
  }

  // Prepare the Request Body.
  $request_body = _commerce_taxjar_prepare_request_body($order_wrapper);

  if (empty($request_body)) {
    return FALSE;
  }

  if (!_commerce_taxjar_request_add_lines($request_body, $order_wrapper->commerce_line_items, TRUE)) {
    return FALSE;
  }

  $request_body['transaction_id'] = $order_wrapper->getIdentifier();
  $request_body['transaction_date'] = format_date($order_wrapper->created->value(), 'custom', 'c');
  $request_body['amount'] = $order_wrapper->commerce_order_total->amount_decimal->value() - $request_body['sales_tax'];

  $order = $order_wrapper->value();
  drupal_alter('commerce_taxjar_transaction_request', $request_body, $order);

  return $request_body;
}

/**
 * Prepares the request body.
 *
 * @param object $order_wrapper
 *   The order wrapper.
 * @param array $to_address
 *   Optionally supplied to address.
 */
function _commerce_taxjar_prepare_request_body($order_wrapper, array $to_address = []) {
  $request_body = array();

  // Add from address if module is configured not to use the
  // address on file with TaxJar.
  if (variable_get('commerce_taxjar_shipping_mode', 'on_file') == 'primary') {
    $request_body['from_country'] = variable_get('commerce_taxjar_primary_country', 'US');
    $request_body['from_zip'] = variable_get('commerce_taxjar_primary_zip', '');
    $request_body['from_state'] = variable_get('commerce_taxjar_primary_state', '');
    $request_body['from_city'] = variable_get('commerce_taxjar_primary_city', '');
    $request_body['from_street'] = variable_get('commerce_taxjar_primary_street', '');
  }

  // Add to address.
  if (!empty($to_address)) {
    $request_body['to_country'] = $to_address['country'];
    $request_body['to_zip'] = $to_address['postal_code'];
    $request_body['to_state'] = $to_address['administrative_area'];
    $request_body['to_city'] = $to_address['locality'];
    $request_body['to_street'] = $to_address['thoroughfare'];
    // Concatenate street line 2 if supplied.
    if (!empty($to_address['premise'])) {
      $request_body['to_street'] .= ' ' . $to_address['premise'];
    }
  }
  else {
    $customer_profile_field = commerce_taxjar_get_customer_profile_field($order_wrapper);

    // Retrieve the address from the configured customer profile type,
    // return false if none is available.
    if (!empty($customer_profile_field) && isset($order_wrapper->{$customer_profile_field})) {
      if (isset($order_wrapper->{$customer_profile_field}->commerce_customer_address)) {
        $address = $order_wrapper->{$customer_profile_field}->commerce_customer_address->value();
        $request_body['to_country'] = $address['country'];
        $request_body['to_zip'] = $address['postal_code'];
        $request_body['to_state'] = $address['administrative_area'];
        $request_body['to_city'] = $address['locality'];
        $request_body['to_street'] = $address['thoroughfare'];
        // Concatenate street line 2 if supplied.
        if (!empty($address['premise'])) {
          $request_body['to_street'] .= ' ' . $address['premise'];
        }
      }
      else {
        return FALSE;
      }
    }
    else {
      return FALSE;
    }
  }

  return $request_body;
}

/**
 * Returns the transaction line items that will be sent to the API.
 *
 * @param array $request_body
 *   The request body that needs to be altered.
 * @param $line_items
 *   An array of line items wrapper that need to be added to the transaction.
 * @param bool $is_transaction
 *   A flag which indicates whether we are building for a
 *   tax request or transaction request.
 */
function _commerce_taxjar_request_add_lines(array &$request_body, $line_items, $is_transaction = FALSE) {
  $lines = array();
  $shipping = 0;

  // If we are creating a transaction, extract the TaxJar lineitem data.
  if ($is_transaction) {
    foreach ($line_items as $line_item_wrapper) {
      $line_item = $line_item_wrapper->value();
      if ($line_item->type != 'taxjar') {
        continue;
      }
      $tax_data = unserialize($line_item_wrapper->taxjar_details->value());
      $request_body['sales_tax'] = $line_item_wrapper->commerce_total->amount_decimal->value();
      if ($request_body['sales_tax'] != 0) {
        $line_items_tax_collected = array();
        foreach ($tax_data['breakdown']['line_items'] as $item) {
          $line_items_tax_collected[$item['id']] = $item['tax_collectable'];
        }
      }
      break;
    }
    if (!isset($request_body['sales_tax'])) {
      return FALSE;
    }
  }

  // Loop over the line items passed.
  foreach ($line_items as $line_item_wrapper) {
    // Ensure the line item still exists.
    if (!$line_item_wrapper->value()) {
      continue;
    }
    $line_item = $line_item_wrapper->value();

    // Handles products.
    if (in_array($line_item->type, commerce_product_line_item_types())) {

      $newline = array(
        'id' => $line_item->line_item_id,
        'quantity' => (int) $line_item->quantity,
      );

      if ($is_transaction) {
        $newline['product_identifier'] = $line_item_wrapper->commerce_product->sku->value();
        $newline['description'] = $line_item_wrapper->commerce_product->title->value();
        $newline['sales_tax'] = isset($line_items_tax_collected[$line_item->line_item_id]) ? $line_items_tax_collected[$line_item->line_item_id] : 0;
      }

      // Get the tax code from the "Tax code" term referenced by the product.
      if (isset($line_item_wrapper->commerce_product->commerce_taxjar_tax_code)) {
        if ($category = $line_item_wrapper->commerce_product->commerce_taxjar_tax_code->value()) {
          $newline['product_tax_code'] = entity_metadata_wrapper('taxonomy_term', $category)->taxjar_category_code->value();
        }
      }

      $unit_price = 0;
      $discount = 0;
      // Calculate product price and any discounts.
      $price_components = $line_item_wrapper->commerce_total->data->value()['components'];
      foreach ($price_components as $component) {
        if ($component['name'] == 'base_price') {
          $unit_price = ($component['price']['amount'] / (int) $line_item->quantity) / 100;
        }
        else {
          $discount -= $component['price']['amount'] / 100;
        }
      }

      $newline['unit_price'] = $unit_price;
      if ($discount != 0) {
        $newline['discount'] = $discount;
      }

      $lines[] = $newline;
    }
    // Handles shipping.
    elseif ($line_item->type === 'shipping') {
      $shipping += $line_item_wrapper->commerce_total->amount_decimal->value();
    }
    // Handles discounts.
    elseif ($line_item->type === 'commerce_discount') {
      $line = array(
        'id' => $line_item->line_item_id,
        'discount' => $line_item_wrapper->commerce_total->amount_decimal->value() * -1,
      );
      if ($is_transaction) {
        $line['description'] = t('Order-level Discount');
      }
      $lines[] = $line;
    }
  }

  $request_body['shipping'] = $shipping;

  if ($lines) {
    $request_body['line_items'] = $lines;
  }
  return TRUE;
}
