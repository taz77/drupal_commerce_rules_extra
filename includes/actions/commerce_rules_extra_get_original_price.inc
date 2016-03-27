<?php

/**
 * @file
 * Rules action CRE get original price of item.
 */

/**
 * Helper function to return the action info to the main module.
 */
function commerce_rules_extra_get_original_price_action_info() {
  return [
    'label' => t('Get the original price of the product from the line item'),
    'group' => t('Commerce Line Item'),
    'parameter' => [
      'line_item' => [
        'type' => 'commerce_line_item',
        'label' => t('Commerce Line Item'),
      ],
    ],
    'provides' => [
      'orig_price' => [
        'type' => 'integer',
        'label' => t('Original Price'),
      ],
    ],
  ];
}

/**
 * Callback function for rule commerce_rules_extra_get_original_price
 *
 * Return original price from line item.
 */
function commerce_rules_extra_get_original_price($line_item) {
  $price = NULL;

  $old_price = $line_item->commerce_unit_price[LANGUAGE_NONE][0]['data']['components'][0]['price']['amount'];
  if (isset($old_price)) {
    $price = $old_price;
  }
  return ['orig_price' => $price];
}