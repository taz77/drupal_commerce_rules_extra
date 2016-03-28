<?php

/**
 * @file
 * Rules events Checkout Pane Event.
 */

/**
 * Helper function to return the event info to the main module.
 */
function commerce_rules_extra_process_checkout_pane_event_info() {
  return [
    'label' => t('Process change to a checkout pane'),
    'group' => t('Commerce Checkout'),
    'variables' => [
      'commerce_order' => [
        'type' => 'commerce_order',
        'label' => t('current order'),
      ],
    ],
  ];
}
