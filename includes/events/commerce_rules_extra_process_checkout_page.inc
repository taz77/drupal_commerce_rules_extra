<?php

/**
 * @file
 * Rules events Checkout Page Event.
 */

/**
 * Helper function to return the event info to the main module.
 */
function commerce_rules_extra_process_checkout_page_event_info() {
  return [
    'label' => t('Process a change to the checkout page'),
    'group' => t('Commerce Checkout'),
  ];
}
