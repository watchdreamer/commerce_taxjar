<?php

/**
 * @file
 * Administrative callbacks for the Commerce TaxJar module.
 */

/**
 * Builds the TaxJar settings form.
 */
function commerce_taxjar_settings_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'commerce_taxjar', 'includes/commerce_taxjar.admin');
  include_once DRUPAL_ROOT . '/includes/locale.inc';

  $form['auth'] = array(
    '#title' => t('Authorization'),
    '#type' => 'fieldset',
  );

  $form['auth']['commerce_taxjar_api_key'] = array(
    '#title' => t('API Token'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get('commerce_taxjar_api_key', ''),
    '#description' => t('Enter your TaxJar API token. If you do not have a token, !login and go to Account > API Access to generate a new one.', array('!login' => '<a href="https://app.taxjar.com" target="_blank">login</a>')),
  );

  $form['general'] = array(
    '#title' => t('General'),
    '#type' => 'fieldset',
  );

  $form['general']['commerce_taxjar_enable_calculation'] = array(
    '#title' => t('Use TaxJar for sales tax calculation'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('commerce_taxjar_enable_calculation', TRUE),
  );

  $form['general']['commerce_taxjar_enable_reporting'] = array(
    '#title' => t('Use TaxJar for sales tax reporting'),
    '#description' => t('Record order transactions to TaxJar for automated reporting / filing.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('commerce_taxjar_enable_reporting', TRUE),
  );

  // Configure address to use for sales tax.
  $form['general']['commerce_taxjar_tax_address'] = array(
    '#title' => t('Destination Address to use for Sales Tax calculation'),
    '#description' => t('Select "Shipping information" if Commerce Shipping is installed.'),
    '#type' => 'select',
    '#options' => commerce_customer_profile_type_options_list(),
    '#default_value' => variable_get('commerce_taxjar_tax_address', 'shipping'),
  );

  $form['general']['commerce_taxjar_shipping_mode'] = array(
    '#title' => t('Ship From Address'),
    '#description' => t('To accurately calculate sales tax, TaxJar must know where an item is shipped from. Even if the order is not physically shipped (ie - delivered electronically), a physical address is required to calculate tax.'),
    '#type' => 'select',
    '#options' => array('on_file' => t('Use address on file with TaxJar'), 'primary' => t('Enter a primary address from which orders will ship.')),
    '#default_value' => variable_get('commerce_taxjar_shipping_mode', 'on_file'),
  );

  $states = array(
    'AL' => t('Alabama'),
    'AK' => t('Alaska'),
    'AZ' => t('Arizona'),
    'AR' => t('Arkansas'),
    'CA' => t('California'),
    'CO' => t('Colorado'),
    'CT' => t('Connecticut'),
    'DE' => t('Delaware'),
    'DC' => t('District Of Columbia'),
    'FL' => t('Florida'),
    'GA' => t('Georgia'),
    'HI' => t('Hawaii'),
    'ID' => t('Idaho'),
    'IL' => t('Illinois'),
    'IN' => t('Indiana'),
    'IA' => t('Iowa'),
    'KS' => t('Kansas'),
    'KY' => t('Kentucky'),
    'LA' => t('Louisiana'),
    'ME' => t('Maine'),
    'MD' => t('Maryland'),
    'MA' => t('Massachusetts'),
    'MI' => t('Michigan'),
    'MN' => t('Minnesota'),
    'MS' => t('Mississippi'),
    'MO' => t('Missouri'),
    'MT' => t('Montana'),
    'NE' => t('Nebraska'),
    'NV' => t('Nevada'),
    'NH' => t('New Hampshire'),
    'NJ' => t('New Jersey'),
    'NM' => t('New Mexico'),
    'NY' => t('New York'),
    'NC' => t('North Carolina'),
    'ND' => t('North Dakota'),
    'OH' => t('Ohio'),
    'OK' => t('Oklahoma'),
    'OR' => t('Oregon'),
    'PA' => t('Pennsylvania'),
    'RI' => t('Rhode Island'),
    'SC' => t('South Carolina'),
    'SD' => t('South Dakota'),
    'TN' => t('Tennessee'),
    'TX' => t('Texas'),
    'UT' => t('Utah'),
    'VT' => t('Vermont'),
    'VA' => t('Virginia'),
    'WA' => t('Washington'),
    'WV' => t('West Virginia'),
    'WI' => t('Wisconsin'),
    'WY' => t('Wyoming'),
    'AA' => t('Armed Forces (Americas)'),
    'AE' => t('Armed Forces (Europe, Canada, Middle East, Africa)'),
    'AP' => t('Armed Forces (Pacific)'),
    'AS' => t('American Samoa'),
    'FM' => t('Federated States of Micronesia'),
    'GU' => t('Guam'),
    'MH' => t('Marshall Islands'),
    'MP' => t('Northern Mariana Islands'),
    'PW' => t('Palau'),
    'PR' => t('Puerto Rico'),
    'VI' => t('Virgin Islands'),
  );

  $form['general']['commerce_taxjar_primary_country'] = array(
    '#title' => t('Primary Business Country'),
    '#description' => t('The primary country your business is located in.'),
    '#type' => 'select',
    '#options' => country_get_list(),
    '#default_value' => variable_get('commerce_taxjar_primary_country', 'US'),
    '#states' => array(
      'visible' => array(
        ':input[name="commerce_taxjar_shipping_mode"]' => array('value' => 'primary'),
      ),
    ),
  );

  $form['general']['commerce_taxjar_primary_street'] = array(
    '#title' => t('Primary Business Street'),
    '#description' => t('The Primary Street your business is located in.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('commerce_taxjar_primary_street', ''),
    '#states' => array(
      'visible' => array(
        ':input[name="commerce_taxjar_shipping_mode"]' => array('value' => 'primary'),
      ),
    ),
  );

  $form['general']['commerce_taxjar_primary_city'] = array(
    '#title' => t('Primary Business City'),
    '#description' => t('The Primary City your business is located in.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('commerce_taxjar_primary_city', ''),
    '#states' => array(
      'visible' => array(
        ':input[name="commerce_taxjar_shipping_mode"]' => array('value' => 'primary'),
      ),
    ),
  );
  $form['general']['commerce_taxjar_primary_state'] = array(
    '#title' => t('Primary Business State'),
    '#description' => t('The Primary State your business is located in.'),
    '#type' => 'select',
    '#options' => $states,
    '#default_value' => variable_get('commerce_taxjar_primary_state', ''),
    '#states' => array(
      'visible' => array(
        ':input[name="commerce_taxjar_primary_country"]' => array('value' => 'US'),
        ':input[name="commerce_taxjar_shipping_mode"]' => array('value' => 'primary'),
      ),
    ),
  );
  $form['general']['commerce_taxjar_primary_zip'] = array(
    '#title' => t('Primary Business Zip'),
    '#description' => t('The Primary Zip Code your business is located in. NB - Must be a Valid 5 digit zip.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('commerce_taxjar_primary_zip', ''),
    '#states' => array(
      'visible' => array(
        ':input[name="commerce_taxjar_shipping_mode"]' => array('value' => 'primary'),
      ),
    ),
  );

  if (!is_null(variable_get('commerce_taxjar_api_key', NULL))) {

    $form['categories'] = array(
      '#title' => t('Product Categories'),
      '#type' => 'fieldset',
      '#description' => t('TaxJar supplies certain product categories which are used to tag products that are either exempt from sales tax in some jurisdictions or are taxed at reduced rates. These categories are fetched at the time of module installation, and will typically not require updating. If a new category has been added by TaxJar which you would like to use, the button below will fetch any updated categories from the TaxJar service.'),
      '#prefix' => '<div id="taxjar-categories">',
      '#suffix' => '</div>',
    );

    $form['categories']['taxjar_categories_update'] = array(
      '#type' => 'submit',
      '#name' => 'update_catagories',
      '#value' => 'Update TaxJar Product Categories',
      '#limit_validation_errors' => array(),
      '#submit' => array('commerce_taxjar_update_categories'),
      '#ajax' => array(
        'callback' => 'commerce_taxjar_settings_form_ajax_categories',
        'wrapper' => 'taxjar-categories',
        'effect' => 'fade',
        'progress' => array(
          'type' => 'throbber',
        ),
      ),
    );

    $form['nexus'] = array(
      '#title' => t('Nexus Regions'),
      '#type' => 'fieldset',
      '#description' => t('TaxJar stores the regions in which you have sales tax nexus. If you have updated your nexus regions in TaxJar, you can sync those changes here.'),
      '#prefix' => '<div id="taxjar-nexus">',
      '#suffix' => '</div>',
    );

    $query = db_query('SELECT n.country, n.region FROM {commerce_taxjar_nexus_regions} n');
    $result = $query->fetchAll();
    $rows = array();
    foreach ($result as $row) {
      $rows[] = array($row->country, $row->region);
    }

    $form['nexus']['table'] = array(
      '#theme' => 'table',
      '#header' => array(t('Country'), t('Region')),
      '#rows' => $rows,
    );

    $form['nexus']['taxjar_nexus_sync'] = array(
      '#type' => 'submit',
      '#name' => 'sync_nexus',
      '#value' => 'Sync Nexus',
      '#limit_validation_errors' => array(),
      '#submit' => array('commerce_taxjar_sync_nexus'),
      '#ajax' => array(
        'callback' => 'commerce_taxjar_settings_form_ajax_nexus',
        'wrapper' => 'taxjar-nexus',
        'effect' => 'fade',
        'progress' => array(
          'type' => 'throbber',
        ),
      ),
    );

  }

  $form = system_settings_form($form);
  $form['#submit'][] = 'commerce_taxjar_settings_form_submit';

  return $form;
}

/**
 * Implements _form_validate().
 */
function commerce_taxjar_settings_form_validate($form, &$form_state) {
  // Strip any whitespace from api key.
  $form_state['values']['commerce_taxjar_api_key'] = trim($form_state['values']['commerce_taxjar_api_key']);
}

/**
 * AJAX callback for categories.
 */
function commerce_taxjar_settings_form_ajax_categories($form, $form_state) {
  $form['categories']['taxjar_categories_update']['#attributes']['disabled'] = 'disabled';
  return $form['categories'];
}

/**
 * AJAX callback for nexus.
 */
function commerce_taxjar_settings_form_ajax_nexus($form, $form_state) {
  $form['nexus']['taxjar_nexus_sync']['#attributes']['disabled'] = 'disabled';
  return $form['nexus'];
}

/**
 * Submit function for nexus sync.
 */
function commerce_taxjar_sync_nexus($form, &$form_state) {
  // Remove all existing regions.
  db_truncate('commerce_taxjar_nexus_regions')->execute();

  // Fetch nexus regions from API.
  $regions = commerce_taxjar_get('nexus/regions');
  $query = db_insert('commerce_taxjar_nexus_regions')->fields(array(
    'country_code',
    'country',
    'region_code',
    'region',
  ));
  foreach ($regions['regions'] as $region) {
    $query->values($region);
  }
  $query->execute();

  $form_state['rebuild'] = TRUE;
}

/**
 * Additional submit handler for settings form.
 */
function commerce_taxjar_settings_form_submit($form, &$form_state) {
  // If this is the first time the form is saved, update categories
  // and nexus regions.
  $vocabulary = taxonomy_vocabulary_machine_name_load('taxjar_categories');
  $query = new EntityFieldQuery();
  $terms = $query->entityCondition('entity_type', 'taxonomy_term')
    ->propertyCondition('vid', $vocabulary->vid)
    ->count()
    ->execute();
  if (empty($terms)) {
    commerce_taxjar_update_categories();

    // Fetch nexus regions from API.
    $regions = commerce_taxjar_get('nexus/regions');
    $query = db_insert('commerce_taxjar_nexus_regions')->fields(array(
      'country_code',
      'country',
      'region_code',
      'region',
    ));
    foreach ($regions['regions'] as $region) {
      $query->values($region);
    }
    $query->execute();
  }

  // Enable / disable rules as necessary.
  $reporting_rules = array(
    'commerce_taxjar_update_order',
    'commerce_taxjar_save_order',
    'commerce_taxjar_refund_order',
    'commerce_taxjar_delete_order',
  );

  if ($form_state['values']['commerce_taxjar_enable_reporting']) {
    foreach ($reporting_rules as $rule) {
      $rules_config = rules_config_load($rule);
      $rules_config->active = TRUE;
      $rules_config->save();
    }
  }
  else {
    foreach ($reporting_rules as $rule) {
      $rules_config = rules_config_load($rule);
      $rules_config->active = FALSE;
      $rules_config->save();
    }
  }
}
