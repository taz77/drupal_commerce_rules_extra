<?php
function commerce_rules_extra_compare_product_amount_condition_info() {
    $conditions = commerce_rules_extra_compare_product_quantity_condition_info();
    $conditions ['label'] = t('Total amount of selected products comparison');
    $conditions ['parameter'] ['value'] = array (
            'type' => 'integer',
            'label' => t('Total amount'),
            'default value' => 100,
            'description' => t('The value to compare against the total amount of products of the type on the order. Amount without decimal point (ie : 50.65 US ==> 5065') 
    );
    return $conditions;
}
function commerce_rules_extra_compare_product_amount_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
    return commerce_rules_extra_rules_condition_has_terms_form_alter($form, $form_state, $options, $element);
}
function commerce_rules_extra_compare_product_amount($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value) {
    return commerce_rules_extra_compare_product($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value, "amount");
}

