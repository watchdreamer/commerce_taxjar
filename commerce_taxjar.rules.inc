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
    'commerce_taxjar_select_address_profile' => array(
      'label' => t('Specify an address profile to use for sales tax calculation'),
      'group' => t('Commerce TaxJar'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
        ),
        'profile' => array(
          'type' => 'text',
          'label' => t('Profile'),
          'description' => t('The address profile to use for sales tax calculation.'),
          'options list' => 'commerce_taxjar_profile_options_list',
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
    'commerce_order_contains_products_of_category' => array(
      'label' => t('Order contains products belonging to category'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
          'wrapped' => TRUE,
        ),
        'mode' => array(
          'label' => t('Selection Mode'),
          'type' => 'text',
          'options list' => 'commerce_taxjar_mode_options_list',
          'default value' => 'inclusive',
          'restriction' => 'input',
        ),
        'category' => array(
          'label' => t('Product Category'),
          'type' => 'list<text>',
          'description' => t('The product category/categories to look for in the order.'),
          'options list' => 'commerce_taxjar_category_options_list',
        ),
      ),
      'group' => t('Commerce TaxJar'),
      'callbacks' => array(
        'execute' => 'commerce_taxjar_rules_contains_products_category',
      ),
    ),
  );
}

/**
 * Implements hook_rules_event_info().
 */
function commerce_taxjar_rules_event_info() {
  return array(
    'commerce_taxjar_select_tax_address' => array(
      'label' => t('On address selection for tax calculation'),
      'group' => t('Commerce TaxJar'),
      'variables' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Commerce Order'),
        ),
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

/**
 * Provides options for exclusion mode.
 */
function commerce_taxjar_mode_options_list() {
  return array('exclusive' => 'Order contains ONLY products of the selected category', 'inclusive' => 'Order contains ONE OR MORE products of the selected category');
}

/**
 * Provides options for product categories.
 */
function commerce_taxjar_category_options_list() {
  $options = array();
  $vocabulary = taxonomy_vocabulary_machine_name_load('taxjar_categories');
  $terms = taxonomy_get_tree($vocabulary->vid);
  foreach ($terms as $term) {
    $options[$term->tid] = $term->name;
  }
  return $options;
}

/**
 * Provides options for address profiles.
 */
function commerce_taxjar_profile_options_list() {
  $options = array();
  $profiles = commerce_customer_profile_types();
  foreach ($profiles as $key => $type) {
    $options[$key] = $type['name'];
  }
  return $options;
}

/**
 * Check whether an order has product(s) of the given category.
 *
 * @param EntityDrupalWrapper $order_wrapper
 *   The wrapped order entity.
 * @param string $mode
 *   The selection mode.
 * @param array $categories
 *   The selected product categories.
 *
 * @return bool
 *   Whether a the condition is true.
 */
function commerce_taxjar_rules_contains_products_category(EntityDrupalWrapper $order_wrapper, $mode, array $categories) {
  // Get all product categories found in order.
  $order_categories = array();
  foreach ($order_wrapper->commerce_line_items as $line_item_wrapper) {
    $line_item = $line_item_wrapper->value();
    if (in_array($line_item->type, commerce_product_line_item_types())) {
      if (isset($line_item_wrapper->commerce_product->commerce_taxjar_tax_code)
          && $category = $line_item_wrapper->commerce_product->commerce_taxjar_tax_code->value()) {
        if ($mode == 'exclusive' && !isset($categories[$category->tid])) {
          // If we are looking for an order consisting of ONLY the specified
          // categories, and there is a product with a category which is not
          // in our list, return false.
          return FALSE;
        }
        $order_categories[$category->tid] = $category->tid;
      }
      elseif ($mode == 'exclusive') {
        // If we are looking for an order consisting of ONLY the specified
        // categories, and there is a product without a category, return false.
        return FALSE;
      }
    }
  }
  return (count(array_intersect($categories, $order_categories)) > 0);
}
