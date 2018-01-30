Introduction
------------

The Commerce TaxJar module provides integration with the TaxJar automated sales tax calculation, reporting, and filing platform for Drupal Commerce.

 * For a full description of the module, visit the [project page on drupal.org](https://drupal.org/project/commerce_taxjar).

 * To submit bug reports and feature suggestions, or to track changes:
   [drupal.org/project/issues/commerce_taxjar](https://drupal.org/project/issues/commerce_taxjar)
   
 * For more information about TaxJar, visit the website: [taxjar.com](https://www.taxjar.com)


Requirements
------------

 * Drupal Commerce.

 * The server PHP configuration must support cURL for sending requests to the TaxJar API.


Installation
------------
 
 * Install as you would normally install a contributed Drupal module. See the [instructions on drupal.org](https://drupal.org/documentation/install/modules-themes/modules-7)
   for further information.
   
 * Once the module is installed, enter your TaxJar SmartCalcs API Token on the module configuration page.


Configuration
-------------
 
 * Configure user permissions in Administration » People » Permissions:

   - **Administer Commerce TaxJar**
   Users in roles with the "Administer Commerce TaxJar" permission will be able to
   modify the TaxJar configuration for the site.

 * Configure the module in
   Administration » Store » Configuration » TaxJar.
   
 * Once an API token from TaxJar is saved on the configuration page, the module will automatically
   fetch and download the available product tax categories from the TaxJar service. These will be
   saved in the "TaxJar Categories" taxonomy vocabulary, and can be referenced from any commerce
   products which fall under a special tax category.