<?php
function commerce_rules_extra_compare_product_quantity_condition_info() {
    return array (
            'group' => t('Commerce Order'),
            'label' => t('Total quantity of selected product comparison'),
            'parameter' => array (
                    'order' => array (
                            'type' => 'commerce_order',
                            'label' => t('Order'),
                            'description' => t('The order whose product line item quantities should be totalled. If the specified order does not exist, the comparison will act as if it is against a quantity of 0.') 
                    ) 
            ) + commerce_rules_extra_terms_parameters(false) + array (
                    'product_type' => array (
                            'type' => 'text',
                            'label' => t('Product Type'),
                            'description' => t('The type of the product to look for on the order.'),
                            'options list' => 'commerce_rules_extra_product_type_options_list',
                            'restriction' => 'input',
                            'optional' => TRUE 
                    ),
                    'exclude' => array (
                            'type' => 'boolean',
                            'label' => t('Exclude content_type'),
                            'description' => t('If checked the total of all products on the order excluding the product type specified above will be calculated.'),
                            'restriction' => 'input' 
                    ),
                    'operator' => array (
                            'type' => 'text',
                            'label' => t('Operator'),
                            'description' => t('The comparison operator to use against the total number of products matching the term on the order.'),
                            'default value' => '>=',
                            'options list' => 'commerce_numeric_comparison_operator_options_list',
                            'restriction' => 'input' 
                    ),
                    'value' => array (
                            'type' => 'integer',
                            'label' => t('Quantity'),
                            'default value' => 1,
                            'description' => t('The value to compare against the total quantity of products matching the term on the order.') 
                    ) 
            ) 
    );
}

/**
 * Alters the form for
 * commerce_rules_extra_compare_termed_product_quantity so that we
 * can require the term reference field be selected first so that the term_id
 * list can be populated.
 */
function commerce_rules_extra_compare_product_quantity_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
    return commerce_rules_extra_rules_condition_has_terms_form_alter($form, $form_state, $options, $element);
}

/**
 * Calculates the quantity of products in an order that have the term $term_id
 * and compares it to a given value
 *
 * @param object $order
 *            A commerce_order containing the products being checked
 * @param string $field_name
 *            A string containing the machine name of a Taxonomy reference field
 * @param integer $term_id
 *            An integer corresponding to a Taxonomy term id
 * @param string $operator
 *            A string containing the operator used comparing the calculated quantity to
 *            $value
 * @param integer $value
 *            An integer to compare to the quantity of products containing $term_id in
 *            the $field_name field
 *            
 * @return The result of evaluating the calculated quantity against $value with the
 *         specified operator.
 */
function commerce_rules_extra_compare_product_quantity($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value) {
    return commerce_rules_extra_compare_product($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value, "quantity");
}
function commerce_rules_extra_compare_product($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value, $type = "quantity") {
    $quantity = 0;
    $field_name = trim($field_name);
    
    if (! empty($order)) {
        $wrapper = entity_metadata_wrapper('commerce_order', $order);
        $line_items = $wrapper->commerce_line_items->value();
        if (! empty($line_items)) {
            // Holds line items that have products with $term_id
            $termed_line_items = array ();
            foreach ( $line_items as $line_item ) {
                
                if (in_array($line_item->type, commerce_product_line_item_types())) {
                    $pwrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
                    $product = $pwrapper->commerce_product->value();
                    if (empty($product_type) || (! $exclude && $product->type == $product_type) || ($exclude && $product->type != $product_type)) {
                        
                        if (empty($term_ids) || commerce_rules_extra_rules_condition_has_terms($line_item, $field_name, $term_ids, $require_all)) {
                            $termed_line_items [] = $line_item;
                        }
                    }
                }
            }
            if (! empty($termed_line_items)) {
                if ($type == "quantity") {
                    $quantity = commerce_line_items_quantity($termed_line_items, commerce_product_line_item_types());
                } else {
                    $result = commerce_line_items_total($termed_line_items, commerce_product_line_item_types());
                    $quantity = $result ["amount"];
                }
            }
        }
    }
    
    // Make a quantity comparison based on the operator.
    switch ($operator) {
        case '<' :
            return $quantity < $value;
        case '<=' :
            return $quantity <= $value;
        case '=' :
            return $quantity == $value;
        case '>=' :
            return $quantity >= $value;
        case '>' :
            return $quantity > $value;
    }
    
    return FALSE;
}
function commerce_rules_extra_product_type_options_list() {
    return array (
            "" => "<" . t('none') . ">" 
    ) + commerce_product_type_options_list();
}
