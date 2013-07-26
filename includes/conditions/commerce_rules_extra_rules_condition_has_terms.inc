<?php
function commerce_rules_extra_rules_condition_has_terms_condition_info() {
    return array (
            'group' => t('Commerce Line Item'),
            'label' => t('Line item product has term(s)'),
            'parameter' => array (
                    'line_item' => array (
                            'type' => 'commerce_line_item',
                            'label' => t('Commere Line Item') 
                    ) 
            ) 
            + commerce_rules_extra_terms_parameters() 
    );
}
function commerce_rules_extra_terms_parameters($required = true) {
    return array (
            'field_name' => array (
                    'type' => 'text',
                    'label' => t('Term Reference Field'),
                    'description' => t('The machine-name of the expected product\'s term reference field'),
                    'options list' => ($required ? 'commerce_rules_extra_term_fields_options_list' : 'commerce_rules_extra_term_fields_optional_options_list'),
                    'restriction' => 'input',
                    'optional' => ! $required 
            ),
            'term_id' => array (
                    'type' => 'list<integer>',
                    'label' => t('Taxonomy Term(s)'),
                    'description' => t('The term(s) being checked for'),
                    'restriction' => 'input',
                    'options list' => 'commerce_rules_extra_empty_options_list',
                    'optional' => ! $required 
            ),
            'require_all' => array (
                    'type' => 'boolean',
                    'label' => t('Require All'),
                    'description' => t('If checked the condition will only return TRUE if the product has all of the selected terms.'),
                    'restriction' => 'input' 
            ) 
    );
}

/**
 * Alters the form for commerce_rules_extra_rules_condition_has_terms
 * so that we can require the term reference field be selected first so that
 * the term_ids list can be populated.
 */
function commerce_rules_extra_rules_condition_has_terms_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
    // Change the list parameter to be not editable any more.
    $form ['parameter'] ['field_name'] ['settings'] ['field_name'] ['#ajax'] = array (
            'event' => 'change',
            'callback' => 'commerce_rules_extra_ajax_load_terms',
            'wrapper' => 'cre_terms' 
    );
    $form ['parameter'] ['term_id'] ['settings'] ['term_id'] ['#prefix'] = "<div id='cre_terms'>";
    $form ['parameter'] ['term_id'] ['settings'] ['term_id'] ['#options'] = (empty($element->settings ['field_name']) ? array () : commerce_rules_extra_get_terms_list($element->settings ['field_name']));
    $form ['parameter'] ['term_id'] ['settings'] ['term_id'] ['#suffix'] = "</div>";
    $form ['parameter'] ['term_id'] ['#states'] = array (
            'invisible' => array (
                    ':input[name="parameter[field_name][settings][field_name]"]' => array (
                            'value' => '' 
                    ) 
            ) 
    );
    $form ['parameter'] ['require_all'] ['#states'] = array (
            'invisible' => array (
                    ':input[name="parameter[field_name][settings][field_name]"]' => array (
                            'value' => '' 
                    ) 
            ) 
    );
}

/**
 * Check if a line item has a product with certain term ids
 *
 * @param object $line_item
 *            A commerce_line_item containing the product being checked
 * @param string $field_name
 *            A string containing the machine name of a Taxonomy reference field
 * @param array $term_ids
 *            An array containing Taxonomy term ids to check for
 * @param boolean $require_all
 *            A boolean variable indicating whether or not all term ids must be present
 *            
 * @return TRUE if the product has the term applied to it on the field $field_name
 *         Otherwise FALSE
 */
function commerce_rules_extra_rules_condition_has_terms($line_item, $field_name, $term_ids, $require_all) {
    if (! empty($line_item)) {
        $wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
        $product = $wrapper->commerce_product->value();
        if (isset($product->$field_name)) {
            $product_terms = $wrapper->commerce_product->$field_name->value();
        }
        if (! empty($product_terms)) {
            if ($require_all) {
                foreach ( $term_ids as $term_id ) {
                    // If ther term reference field is set to allow more than one term
                    // $product_terms will be an array
                    if (is_array($product_terms)) {
                        $has_term = FALSE;
                        foreach ( $product_terms as $product_term ) {
                            if ($product_term->tid == $term_id) {
                                $has_term = TRUE;
                            }
                        }
                        
                        if (! $has_term) {
                            return FALSE;
                        }
                    } else {
                        if (! $product_terms->tid == $term_id) {
                            return FALSE;
                        }
                    }
                }
                
                // If we haven't returned FALSE already then TRUE must be the answer.
                return TRUE;
            } else {
                foreach ( $term_ids as $term_id ) {
                    // If ther term reference field is set to allow more than one term
                    // $product_terms will be an array
                    if (is_array($product_terms)) {
                        foreach ( $product_terms as $product_term ) {
                            if ($product_term->tid == $term_id) {
                                return TRUE;
                            }
                        }
                    } else {
                        if ($product_terms->tid == $term_id) {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    
    return FALSE;
}

/**
 * Returns an array of machine_names for taxonomy term reference fields
 */
function commerce_rules_extra_term_fields_optional_options_list() {
    $field_options_list = array (
            "" => "<" . t('none') . ">" 
    ) + commerce_rules_extra_term_fields_options_list();
    return $field_options_list;
}
function commerce_rules_extra_term_fields_options_list() {
    $field_options_list = array ();
    $fields = field_read_fields(array (
            'type' => 'taxonomy_term_reference' 
    ));
    if (! empty($fields)) {
        foreach ( $fields as $key => $value ) {
            $field_options_list [$key] = $key;
        }
    }
    
    return $field_options_list;
}
function commerce_rules_extra_ajax_load_terms($form, $form_state) {
    return array (
            '#type' => 'ajax',
            '#commands' => array (
                    ajax_command_insert(NULL, drupal_render($form ["parameter"] ['term_id'] ['settings'])) 
            ) 
    );
}

/**
 * Takes the machine name of a Taxonomy reference field and retrieves the terms
 * for the associated vocabulary.
 *
 * @param string $field_name
 *            A string containing the machine name of a Taxonomy reference field.
 *            
 * @return An array containing the term names of the vocabulary tied to $field_name
 *         prefixed with a certain number of dashes(-) corresponding to the depth of
 *         the term. Term names are keyed by term id.
 */
function commerce_rules_extra_get_terms_list($field_name) {
    $field = field_read_field($field_name);
    $vocabulary = taxonomy_vocabulary_machine_name_load($field ['settings'] ['allowed_values'] [0] ['vocabulary']);
    $terms = taxonomy_get_tree($vocabulary->vid);
    $term_list = array ();
    if (! empty($terms)) {
        foreach ( $terms as $term ) {
            $term_list [$term->tid] = str_repeat('-', $term->depth) . $term->name;
        }
    }
    
    return $term_list;
}

/**
 * An empty options list so that Rules conditions that need manually populated
 * options sets will have their form variables populated correctly.
 */
function commerce_rules_extra_empty_options_list() {
    return array ();
}
