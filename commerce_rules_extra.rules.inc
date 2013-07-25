<?php

/**
 *  Implements hook_rules_event_info()
 **/  
function commerce_rules_extra_rules_event_info() {
  return array(
    'process_checkout_pane' => array(
      'label' => t('Process checkout pane'),
      'group' => t('Commerce Checkout'),
      
      'variables' => array(
        'commerce_order' => array(
           'type' => 'commerce_order',
          'label' => t('current order'),
        ),
      ),
      
  ),
  'line_item_quantity_changed' => array(
      'label' => t('Quantity of line item has changed'),
      'group' => t('Commerce Checkout'),
      'variables' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Shopping cart order'),
        ),
       'commerce_product' => array(
          'label' => t('Product'),
          'type' => 'commerce_product',
        ),
        'commerce_line_item' => array(
          'label' => t('Line item'),
          'type' => 'commerce_line_item',
        ),
      ),
    ),  
  );
}
 

 
/**
 * Implements hook_rules_conditions_info()
 */
function commerce_rules_extra_rules_condition_info() {
  // Define the condition for checking if a line item has a product with a
  // specified term
   
  // Define the condition for checking if a line item has a product with
  // specified terms
  $conditions['commerce_rules_extra_rules_condition_has_terms'] = array(
    'group' => t('Commerce Line Item'),
    'label' => t('Line item product has term(s)'),
    'parameter' => array(
  		'line_item' => array(
        'type' => 'commerce_line_item', 
        'label' => t('Commere Line Item'),
      )) + commerce_rules_extra_terms_parameters());
  
      
      
      
  // Define the condition for comparing the quantity of products with a
  // specified term in an order to a value
  $conditions['commerce_rules_extra_compare_product_quantity'] = array(
    'group' => t('Commerce Order'),
    'label' => t('Total product with quantity comparison'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order whose product line item quantities should be totalled. If the specified order does not exist, the comparison will act as if it is against a quantity of 0.'),
      )) 
      + commerce_rules_extra_terms_parameters(false)
      + array(
      'product_type' => array(
        'type' => 'text',
        'label' => t('Product Type'),
        'description' => t('The type of the product to look for on the order.'),
        'options list' => 'commerce_rules_extra_product_type_options_list',
        'restriction' => 'input',
        'optional' => TRUE,
      ),
      'exclude' => array(
        'type' => 'boolean',
        'label' => t('Exlude content_type'),
        'description' => t('If checked the 
          quantity of all products on the order excluding the product type 
          specified above will be calculated.'),
        'restriction' => 'input'
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
    
  );
  
  $conditions['commerce_rules_extra_compare_product_amount'] = $conditions['commerce_rules_extra_compare_product_quantity'];
  $conditions['commerce_rules_extra_compare_product_amount']['label'] = t('Total product amount value comparison');

  
  return $conditions;
}

function commerce_rules_extra_terms_parameters($required=true) {
    return array(
      'field_name' => array(
        'type' => 'text',
        'label' => t('Term Reference Field'),
        'description' => t('The machine-name of the expected product\'s term reference field'),
        'options list' => ($required ? 'commerce_rules_extra_term_fields_options_list' : 'commerce_rules_extra_term_fields_optional_options_list'),
        'restriction' => 'input',
        'optional' => !$required,
      ),
      'term_id' => array(
        'type' => 'list<integer>',
        'label' => t('Taxonomy Term(s)'),
        'description' => t('The term(s) being checked for'),
        'restriction' => 'input',
        'options list' => 'commerce_rules_extra_empty_options_list',
        'optional' => !$required,
            ),
      'require_all' => array(
        'type' => 'boolean',
        'label' => t('Require All'),
        'description' => t('If checked the condition will only return TRUE if
          the product has all of the selected terms.'),
        'restriction' => 'input'
      ),
    );
}

/**
 * Implements hook_rules_action_info().
 */
function commerce_rules_extra_rules_action_info() {
  return array(
    'commerce_rules_extra_change_pane' => array(
      'label' => t('Change pane properties'),
      'group' => t('Commerce Checkout'),
      'parameter' => array(
        'pane_id' => array(
          'type' => 'text',
          'label' => t('Id of pane to change'),
          'options list' => 'commerce_rules_extra_get_panes',
          'restriction' => "input",
        ),
        'page_id' => array(
          'type' => 'text',
          'label' => t('Page to move to'),
          'options list' => 'commerce_rules_extra_get_pages',
          'default value'=>"<same>",
          'restriction' => "input",
        ),
        'enabled' => array(
          'type' => 'boolean',
          'label' => t('Enabled'),
          'default value' => TRUE,
          'restriction' => "input",
        ),
        'weight' => array(
          'type' => 'integer',
          'label' => t('Weight of pane in the page'),
          'description' => t("Set to empty to keep pre-defined weight"),
          'optional' => TRUE,
          'restriction' => "input",
        ),
      ),
    ),
    'commerce_rules_extra_node_from_line_item' => array(
      'label' => t('Get the referencing node from the line item'),
      'group' => t('Commerce Line Item'),
      'parameter' => array(
        'line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Commere Line Item'),
        ),
      ),
      'provides' => array(
        'referencing_node' => array(
          'type' => 'node',
          'label' => t('Referencing node'),
        ),
      ),
    ),
  );
}

/**
 * Option list callback for commerce_rules_extra_change_pane
 * 
 * Return list of checkout pages   
 **/ 
function commerce_rules_extra_get_pages() {
   $pages = commerce_checkout_pages();
   $return = array("<same>" => "<do not change>");
   foreach ($pages as $page_id => $page) {
      $return[$page_id] = $page["title"];
   }
   return $return;
}

/**
 * Option list callback for commerce_rules_extra_change_pane  
 *
 * Return list of checkout panes    
 **/ 
function commerce_rules_extra_get_panes() {
   $panes = commerce_checkout_panes();
   $return = array();
   foreach ($panes as $pane_id => $pane) {
      $return[$pane_id] = $pane["title"];
   }
   return $return;
}

/**
 * Callback function for rule commerce_rules_extra_change_pane
 * 
 * Store in global variables all properties changes for a pane  
 **/  

function commerce_rules_extra_change_pane($pane_id, $page_id, $enabled, $weight) {
  global $pane_changes;
  if (! $pane_changes) $pane_changes = array();
  $pane_changes[$pane_id] = array("page_id" => ($enabled ? $page_id : "disabled"), "enabled" => $enabled, "weight" => $weight);
   
}

/**
 * Alters the form for commerce_rules_extra_rules_condition_has_terms
 * so that we can require the term reference field be selected first so that
 * the term_ids list can be populated.
 */ 
function commerce_rules_extra_rules_condition_has_terms_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
    // Change the list parameter to be not editable any more.
    $form['parameter']['field_name']['settings']['field_name']['#ajax'] = array(
        'event' => 'change',
        'callback' => 'commerce_rules_extra_load_terms',
        'wrapper' => 'cre_terms',
    );
    $form['parameter']['term_id']['settings']['term_id']['#prefix'] = "<div id='cre_terms'>";
    $form['parameter']['term_id']['settings']['term_id']['#options'] = (empty($element->settings['field_name']) ? array() : commerce_rules_extra_get_terms_list($element->settings['field_name']));
    $form['parameter']['term_id']['settings']['term_id']['#suffix'] = "</div>";
    $form['parameter']['term_id']['#states'] = array(
        'invisible' => array(
      		':input[name="parameter[field_name][settings][field_name]"]' => array('value' => '')
        ),
     );
    $form['parameter']['require_all']['#states'] = array(
        'invisible' => array(
      		':input[name="parameter[field_name][settings][field_name]"]' => array('value' => '')
        ),
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
function commerce_rules_extra_compare_product_amount_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
    return commerce_rules_extra_rules_condition_has_terms_form_alter($form, $form_state, $options, $element);
    
}

/**
 * Callback function for rule commerce_rules_extra_node_from_line_item
 *
 * Return referencing node from line item.
 **/
function commerce_rules_extra_node_from_line_item($line_item) {
  $node = NULL;

  $entity = $line_item->data['context']['entity'];
  if (isset($entity['entity_id']) && $entity['entity_type'] == 'node') {
    $node = node_load($entity['entity_id']);
  }

  return array('referencing_node' => $node);
}



/**
 * Check if a line item has a product with certain term ids
 * 
 * @param object $line_item
 *   A commerce_line_item containing the product being checked
 * @param string $field_name
 *   A string containing the machine name of a Taxonomy reference field
 * @param array $term_ids
 *   An array containing Taxonomy term ids to check for
 * @param boolean $require_all
 *   A boolean variable indicating whether or not all term ids must be present
 * 
 * @return
 *   TRUE if the product has the term applied to it on the field $field_name
 *   Otherwise FALSE
 */
function commerce_rules_extra_rules_condition_has_terms($line_item, $field_name, $term_ids, $require_all) {
  if (!empty($line_item)) {
    $wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
    $product = $wrapper->commerce_product->value();
    if (isset($product->$field_name)) {
      $product_terms = $wrapper->commerce_product->$field_name->value();
    }
    if (!empty($product_terms)) {
        if ($require_all) {
        foreach($term_ids as $term_id){
          // If ther term reference field is set to allow more than one term
          // $product_terms will be an array
          if (is_array($product_terms)) {
            $has_term = FALSE;
            foreach ($product_terms as $product_term) {
              if ($product_term->tid == $term_id) {
                $has_term = TRUE;
              }
            }
            
            if (!$has_term) {
              return FALSE;
            }
          }
          else {
            if (!$product_terms->tid == $term_id) {
              return FALSE;
            }
          }
        }
        
        //If we haven't returned FALSE already then TRUE must be the answer.
        return TRUE;
      }
      else {
        foreach($term_ids as $term_id){
          // If ther term reference field is set to allow more than one term
          // $product_terms will be an array
          if (is_array($product_terms)) {
            foreach ($product_terms as $product_term) {
              if ($product_term->tid == $term_id) {
                return TRUE;
              }
            }
          }
          else {
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
 * Calculates the quantity of products in an order that have the term $term_id
 * and compares it to a given value
 * 
 * @param object $order
 *   A commerce_order containing the products being checked
 * @param string $field_name
 *   A string containing the machine name of a Taxonomy reference field
 * @param integer $term_id
 *   An integer corresponding to a Taxonomy term id
 * @param string $operator
 *   A string containing the operator used comparing the calculated quantity to 
 *   $value
 * @param integer $value
 *   An integer to compare to the quantity of products containing $term_id in
 *   the $field_name field
 * 
 * @return
 *   The result of evaluating the calculated quantity against $value with the
 *   specified operator.
 */
function commerce_rules_extra_compare_product_quantity($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value, $type="quantity") {
  $quantity = 0;
  $field_name = trim($field_name);
  
  if (!empty($order)) {
    $wrapper = entity_metadata_wrapper('commerce_order', $order);
    $line_items = $wrapper->commerce_line_items->value();
    if (!empty($line_items)) {
      //Holds line items that have products with $term_id
      $termed_line_items = array();
      foreach ($line_items as $line_item) {
          
        if (in_array($line_item->type, commerce_product_line_item_types())) {
            $pwrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
            $product = $pwrapper->commerce_product->value();
            if (empty($product_type) || 
               (!$exclude && $product->type == $product_type) || 
               ($exclude && $product->type != $product_type)) {
               
               if (empty($term_ids) || commerce_rules_extra_rules_condition_has_terms($line_item, $field_name, $term_ids,$require_all)) {
                    $termed_line_items[] = $line_item;
                }
            }
            print_r($line_item);
        }
      }
      if (!empty($termed_line_items)) {
        if ($type == "quantity") {
            $quantity = commerce_line_items_quantity($termed_line_items, commerce_product_line_item_types());
        }
        else {
            $result = commerce_line_items_total($termed_line_items, commerce_product_line_item_types());
            $quantity = $result["amount"];
        }
      }
      watchdog("cre",$type . " = $quantity for $product_type and " . print_r($term_ids,true));
    }
  }

  // Make a quantity comparison based on the operator.
  switch ($operator) {
    case '<':
      return $quantity < $value;
    case '<=':
      return $quantity <= $value;
    case '=':
      return $quantity == $value;
    case '>=':
      return $quantity >= $value;
    case '>':
      return $quantity > $value;
  }

  return FALSE;
}

function commerce_rules_extra_compare_product_amount($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value) {
    return commerce_rules_extra_compare_product_quantity($order, $field_name, $term_ids, $require_all, $product_type, $exclude, $operator, $value,"amount");
}
/**
 * Returns an array of machine_names for taxonomy term reference fields
 */
function commerce_rules_extra_term_fields_optional_options_list() {
  $field_options_list = array("" => "<" . t('none') . ">") + commerce_rules_extra_term_fields_options_list();
  return $field_options_list;
}

function commerce_rules_extra_term_fields_options_list() {
  $field_options_list = array();
  $fields = field_read_fields(array('type' => 'taxonomy_term_reference'));
  if (!empty($fields)) {
    foreach ($fields as $key => $value) {
      $field_options_list[$key] = $key;
    }
  }

  return $field_options_list;
}

function commerce_rules_extra_product_type_options_list() {
    
  return array("" => "<" . t('none') . ">") + commerce_product_type_options_list();   
    
}


function commerce_rules_extra_load_terms($form, $form_state) {
   return array(
    '#type' => 'ajax',
    '#commands' => array(ajax_command_insert(NULL, drupal_render($form["parameter"]['term_id']['settings']))));
}


/**
 * Takes the machine name of a Taxonomy reference field and retrieves the terms
 * for the associated vocabulary.
 * 
 * @param string $field_name
 *   A string containing the machine name of a Taxonomy reference field.
 * 
 * @return
 *   An array containing the term names of the vocabulary tied to $field_name
 *   prefixed with a certain number of dashes(-) corresponding to the depth of
 *   the term. Term names are keyed by term id.
 */
function commerce_rules_extra_get_terms_list($field_name) {
  $field = field_read_field($field_name);
  $vocabulary = taxonomy_vocabulary_machine_name_load($field['settings']['allowed_values'][0]['vocabulary']);
  $terms = taxonomy_get_tree($vocabulary->vid);
  $term_list = array();
  if (!empty($terms)) {
    foreach ($terms as $term) {
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
  return array();
}
