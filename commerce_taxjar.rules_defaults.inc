<?php

/**
 * @file
 * Default Rules for commerce_taxjar.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_taxjar_default_rules_configuration() {
  $rules = array();

  $rules['commerce_taxjar_save_order'] = rules_import(
    '{ "commerce_taxjar_save_order" : {
        "LABEL" : "Save Order to TaxJar",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Commerce TaxJar" ],
        "REQUIRES" : [ "commerce_taxjar", "commerce_payment" ],
        "ON" : { "commerce_payment_order_paid_in_full" : [] },
        "DO" : [
          { "commerce_taxjar_order_save" : { "commerce_order" : [ "commerce_order" ] } }
        ]
      }
    }');

  $rules['commerce_taxjar_update_order'] = rules_import(
    '{ "commerce_taxjar_update_order" : {
        "LABEL" : "Update order in TaxJar",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Commerce TaxJar" ],
        "REQUIRES" : [ "commerce_taxjar", "entity" ],
        "ON" : { "commerce_order_update" : [] },
        "IF" : [
          { "commerce_order_contains_taxjar_line_item" : { "commerce_order" : [ "commerce_order" ] } },
          { "data_is" : {
              "data" : [ "commerce-order:status" ],
              "op" : "IN",
              "value" : { "value" : {
                  "canceled" : "canceled",
                  "pending" : "pending",
                  "processing" : "processing",
                  "completed" : "completed"
                }
              }
            }
          }
        ],
        "DO" : [
          { "commerce_taxjar_order_update" : { "commerce_order" : [ "commerce_order" ] } }
        ]
      }
    }');

  $rules['commerce_taxjar_refund_order'] = rules_import(
    '{ "commerce_taxjar_refund_order" : {
        "LABEL" : "Refund order in TaxJar",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Commerce TaxJar" ],
        "REQUIRES" : [ "rules", "commerce_taxjar", "entity" ],
        "ON" : { "commerce_order_update" : [] },
        "IF" : [
          { "data_is" : { "data" : [ "commerce-order:status" ], "value" : "canceled" } }
        ],
        "DO" : [
          { "commerce_taxjar_order_refund" : { "commerce_order" : [ "commerce_order" ] } }
        ]
      }
    }');

  $rules['commerce_taxjar_delete_order'] = rules_import(
    '{ "commerce_taxjar_delete_order" : {
        "LABEL" : "Delete order from TaxJar",
        "PLUGIN" : "reaction rule",
        "TAGS" : [ "Commerce TaxJar" ],
        "REQUIRES" : [ "commerce_taxjar", "entity" ],
        "ON" : { "commerce_order_delete" : [] },
        "DO" : [
          { "commerce_taxjar_order_delete" : { "commerce_order" : [ "commerce_order" ] } }
        ]
      }
    }');

  $rules['commerce_taxjar_delete_order_on_tax_line_item_delete'] = rules_import(
    '{ "commerce_taxjar_delete_order_on_tax_line_item_delete" : {
      "LABEL" : "Delete order from TaxJar when tax line item is deleted",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce TaxJar" ],
      "REQUIRES" : [ "commerce_taxjar", "entity" ],
      "ON" : { "commerce_order_update" : [] },
      "IF" : [
        { "NOT commerce_order_contains_taxjar_line_item" : { "commerce_order" : [ "commerce_order" ] } },
        { "data_is" : {
              "data" : [ "commerce-order:status" ],
              "op" : "IN",
              "value" : { "value" : {
                  "canceled" : "canceled",
                  "pending" : "pending",
                  "processing" : "processing",
                  "completed" : "completed"
                }
              }
            }
          }
      ],
      "DO" : [
        { "commerce_taxjar_order_delete" : { "commerce_order" : [ "commerce_order" ] } }
      ]
    }
  }');

  $rules['commerce_taxjar_override_tax_address_for_non_shipped_orders'] = rules_import(
    '{ "commerce_taxjar_override_tax_address_for_non_shipped_orders" : {
      "LABEL" : "Override address used for tax calulation on orders with no physically shipped items",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Commerce TaxJar" ],
      "REQUIRES" : [ "commerce_taxjar" ],
      "ACTIVE" : false,
      "ON" : { "commerce_taxjar_select_tax_address" : [] },
      "IF" : [
        { "commerce_order_contains_products_of_category" : {
            "commerce_order" : [ "commerce_order" ],
            "mode" : "exclusive",
            "category" : { "value" : { "25" : "25", "31" : "31" } }
          }
        }
      ],
      "DO" : [
        { "commerce_taxjar_select_address_profile" : { "commerce_order" : [ "commerce_order" ], "profile" : "billing" } }
      ]
    }
  }');

  return $rules;
}
