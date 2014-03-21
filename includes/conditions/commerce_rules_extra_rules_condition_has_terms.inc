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
            ) + commerce_rules_extra_terms_parameters() 
    );
}
function commerce_rules_extra_terms_parameters($required = true) {
    return array (
            'voc_name' => array (
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
                    'optional' => TRUE, //! $required 
            ),
            'product_display' => array (
                    'type' => 'boolean',
                    'label' => t('Search terms in product display, not in product'),
                    'description' => t('If checked search will be made on product display.'),
                    'restriction' => 'input' 
            ),
            'term_operator' => array (
                    'type' => 'text',
                    'label' => t('Operator'),
                    'description' => t('Selection mode of terms'),
                    'options list' => "commerce_rules_extra_get_operators",
                    'restriction' => 'input', 
                    'optional' => ! $required
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
    $form['parameter']['voc_name']['settings']['voc_name']['#ajax'] = array (
            'event' => 'change',
            'callback' => 'commerce_rules_extra_ajax_load_terms',
            'wrapper' => 'cre_terms' 
    );
    $form['parameter']['term_operator']['settings']['term_operator']['#prefix'] = "<div style='float:left'>";
    $form['parameter']['term_operator']['settings']['term_operator']['#title'] = "Operator";
    $form['parameter']['term_operator']['settings']['term_operator']['#suffix'] = "</div>";
    $form['parameter']['term_id']['settings']['term_id']['#prefix'] = "<div id='cre_terms' style='float:left;margin-left:10px'>";
    $form['parameter']['term_id']['settings']['term_id']['#options'] = (empty($element->settings['voc_name']) ? array () : commerce_rules_extra_get_terms_list($element->settings['voc_name']));
    $form['parameter']['term_id']['settings']['term_id']['#suffix'] = "</div>";
    /*
    $form['parameter']['term_id']['#states'] = array (
            'invisible' => array (
                    ':input[name="parameter[voc_name][settings][voc_name]"]' => array (
                            'value' => '' 
                    ) 
            ) 
    );
    $form ['parameter'] ['term_id']['settings']['term_id'] ['#states'] = array (
            'required' => array (
                    ':input[name="parameter[term_operator][settings][term_operator]"]' => array (
                            array('value' => 'IOO'),
                            array ('value' => 'IAO'),
                            array ('value' => 'INO'),
                     )
                    
                    )
        
            
    );
    */
    $form["#theme"] = "form--cre_condition";
}
function commerce_rules_extra_theme($existing, $type, $theme, $path) {
    return array (
            
            'form--cre_condition' => array (
                    'template' => 'form--cre_condition',
                    'render element' => 'form',
                    'path' => $path . "/templates" 
            ) 
    );
}


/**
 * Check if a line item has a product with certain term ids
 *
 * @param object $line_item
 *            A commerce_line_item containing the product being checked
 * @param string $voc_name
 *            A string containing the machine name of a Taxonomy reference field
 * @param array $term_ids
 *            An array containing Taxonomy term ids to check for
 * @param boolean $require_all
 *            A boolean variable indicating whether or not all term ids must be present
 *            
 * @return TRUE if the product has the term applied to it on the field $voc_name
 *         Otherwise FALSE
 */
function commerce_rules_extra_rules_condition_has_terms($line_item, $voc_name, $term_ids, $product_display, $term_operator) {
    if (! empty($line_item)) {
        $wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
        $product = $wrapper->commerce_product->value();
        $product_terms = array();
        if ($product_display) {
            if (empty($line_item->line_item_id) && (isset($product->display_context))) {
                $line_item->data["context"] = $product->display_context;
            }
            $node = commerce_rules_extra_node_from_line_item($line_item);
            $node = $node["referencing_node"];
            if (null == $node) return false;
            $query = db_select('taxonomy_index', 't')->fields('t', array (
                    'tid' 
            ))->condition('nid', $node->nid, '=');
            $results = $query->execute();
            foreach ( $results as $t ) {
                $product_terms[] = $t;
            }
        } else {
            foreach ( commerce_rules_extra_get_fields_for_vocabulary($voc_name) as $field ) {
                if (isset($product->$field)) {
                    $product_terms = $wrapper->commerce_product->$field->value();
                    break;
                }
            }
        }
        if ($term_operator == "IE") return empty($product_terms);
        if ($term_operator == "INE") return ! empty($product_terms);
        if (! empty($product_terms)) {
            
            if (! is_array($product_terms)) {$t = $product_terms; $product_terms = array(0 => $t);}
            if ($term_operator == "IAO") {
                foreach ( $term_ids as $term_id ) {
                    // If ther term reference field is set to allow more than one term
                    // $product_terms will be an array
                    $has_term = FALSE;
                    foreach ( $product_terms as $product_term ) {
                        if ($product_term->tid == $term_id) {
                            $has_term = TRUE;
                        }
                    }
                    if (! $has_term) {
                        return FALSE;
                    }
                }
                
                // If we haven't returned FALSE already then TRUE must be the answer.
                return TRUE;
            } else if ($term_operator == "IOO") {
                foreach ( $term_ids as $term_id ) {
                    foreach ( $product_terms as $product_term ) {
                        if ($product_term->tid == $term_id) {
                            return TRUE;
                        }
                    }
                }
            } else if ($term_operator == "INO") {
                foreach ( $term_ids as $term_id ) {
                    $has_term = FALSE;
                    foreach ( $product_terms as $product_term ) {
                        if ($product_term->tid == $term_id) {
                            return FALSE;
                        }
                    }
                }
                    
                return TRUE;
            }
        }
    }
    
    return FALSE;
}
function commerce_rules_extra_get_fields_for_vocabulary($voc) {
    $fields = &drupal_static(__FUNCTION__);
    
    if (! isset($fields)) {
        $termfields = field_read_fields(array (
                'type' => 'taxonomy_term_reference' 
        ));
        foreach ( $termfields as $voc_name => $infos ) {
            if ($infos["settings"]["allowed_values"][0]["vocabulary"] == $voc) {
                $fields[] = $voc_name;
            }
        }
    }
    return $fields;
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
            // $field_options_list [$key] = $key;
        }
    }
    
    $vocs = taxonomy_get_vocabularies();
    foreach ( $vocs as $voc ) {
        $field_options_list[$voc->machine_name] = $voc->name;
    }
    
    return $field_options_list;
}
function commerce_rules_extra_ajax_load_terms($form, $form_state) {
    return array (
            '#type' => 'ajax',
            '#commands' => array (
                    ajax_command_insert(NULL, drupal_render($form["parameter"]['term_id']['settings'])) 
            ) 
    );
}

/**
 * Takes the machine name of a Taxonomy reference field and retrieves the terms
 * for the associated vocabulary.
 *
 * @param string $voc_name
 *            A string containing the machine name of a Taxonomy reference field.
 *            
 * @return An array containing the term names of the vocabulary tied to $voc_name
 *         prefixed with a certain number of dashes(-) corresponding to the depth of
 *         the term. Term names are keyed by term id.
 */
function commerce_rules_extra_get_terms_list($voc_name) {
    if (empty($voc_name))
        return array ();
        // $field = field_read_field($voc_name);
        // $vocabulary = taxonomy_vocabulary_machine_name_load($field ['settings'] ['allowed_values'] [0] ['vocabulary']);
        // $terms = taxonomy_get_tree($vocabulary->vid);
    $voc = taxonomy_vocabulary_machine_name_load($voc_name);
    $terms = taxonomy_get_tree($voc->vid);
    $term_list = array ();
    if (! empty($terms)) {
        foreach ( $terms as $term ) {
            $term_list[$term->tid] = str_repeat('-', $term->depth) . $term->name;
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
function commerce_rules_extra_get_operators() {
    return array (
            "IOO" => "is one of",
            "IAO" => "is all of",
            "INO" => "is none of",
            "IE" => "is empty (null)",
            "INE" => "is not empty(not null)" 
    );
}