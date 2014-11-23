<?php

/**
 * @file
 * Rules condition Compare Total Product Quantity.
 */

/**
 * Helper function to return the condition info to the main module.
 */
function commerce_rules_extra_compare_total_products_condition_info() {
  $info = array(
    'group' => t('Commerce Order'),
    'label' => t('Total quantity of products in the cart'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order whose product line item quantities should be totalled. If the specified order does not exist, the comparison will act as if it is against a quantity of 0.'),
      ),
      'operator' => array(
        'type' => 'text',
        'label' => t('Operator'),
        'description' => t('The comparison operator to use against the total number of products matching the term on the order.'),
        'default value' => '>=',
        'options list' => 'commerce_numeric_comparison_operator_options_list',
        'restriction' => 'input',
      ),
      'value' => array(
        'type' => 'integer',
        'label' => t('Quantity'),
        'default value' => 1,
        'description' => t('The value to compare against the total quantity of products matching the term on the order.'),
      ),
    ),
    'callbacks' => array('execute' => 'commerce_rules_extra_compare_total_product_count'),
  );
  $condition = array_merge_recursive($info, commerce_rules_extra_terms_parameters(FALSE));
  return $info;
}

/**
 * Callback function for Commerce Rules Extra Compare Total Products.
 */
function commerce_rules_extra_compare_total_product_count($order, $operator, $value) {
  // Don't do anything without a valid order.
  if (!empty($order)) {
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    $line_items = $order_wrapper->commerce_line_items->value();
    // We only want to count line items that are of the type "product".
    $types = array('product');
    $quantity = commerce_line_items_quantity($line_items, $types);
  }
  // Make a quantity comparison based on the operator.
  switch ($operator) {
    case '<':
      return $quantity < $value;

      break;

    case '<=':
      return $quantity <= $value;

      break;

    case '=':
      return $quantity == $value;

      break;

    case '>=':
      return $quantity >= $value;

      break;

    case '>':
      return $quantity > $value;

      break;
  }
  return FALSE;
}
