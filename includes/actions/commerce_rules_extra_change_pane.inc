<?php

/**
 * @file
 * Rules action Change Pane Properties Action.
 */

/**
 * Helper function to return the action info to the main module.
 */
function commerce_rules_extra_change_pane_action_info() {
  return [
    'label' => t('Change pane properties'),
    'group' => t('Commerce Checkout'),
    'parameter' => [
      'pane_id' => [
        'type' => 'text',
        'label' => t('Id of pane to change'),
        'options list' => 'commerce_rules_extra_get_panes',
        'restriction' => 'input',
      ],
      'page_id' => [
        'type' => 'text',
        'label' => t('Page to move to'),
        'options list' => 'commerce_rules_extra_get_pages',
        'default value' => "<same>",
        'restriction' => 'input',
      ],
      'enabled' => [
        'type' => 'boolean',
        'label' => t('Enabled'),
        'default value' => TRUE,
        'restriction' => 'input',
      ],
      'weight' => [
        'type' => 'integer',
        'label' => t('Weight of pane in the page'),
        'description' => t('Set to empty to keep pre-defined weight'),
        'optional' => TRUE,
        'restriction' => 'input',
      ],
    ],
  ];
}

/**
 * Option list callback for commerce_rules_extra_change_pane.
 *
 * Return list of checkout panes.
 */
function commerce_rules_extra_get_panes() {
  $panes = commerce_checkout_panes();
  $return = [];
  foreach ($panes as $pane_id => $pane) {
    $return[$pane_id] = $pane['name'];
  }
  return $return;
}

/**
 * Callback function for rule commerce_rules_extra_change_pane.
 *
 * Store in global variables all properties changes for a pane.
 */
function commerce_rules_extra_change_pane($pane_id, $page_id, $enabled, $weight) {
  global $_commerce_rules_extra_pane_changes;
  if (!$_commerce_rules_extra_pane_changes) {
    $_commerce_rules_extra_pane_changes = [];
  }
  $_commerce_rules_extra_pane_changes[$pane_id] = [
    'page_id' => ($enabled ? $page_id : 'disabled'),
    'enabled' => $enabled,
    'weight' => $weight,
  ];
}

/**
 * Helper function to Show/hide panes when viewing command.
 */
function commerce_rules_extra_commerce_order_view_alter(&$infos, $entity_type) {
  global $_commerce_rules_extra_pane_changes;
  if (!$_commerce_rules_extra_pane_changes) {
    $_commerce_rules_extra_pane_changes = [];
  }
  $commerce_order = $infos['#entity'];
  rules_invoke_all('process_checkout_pane', $commerce_order);

  foreach ($_commerce_rules_extra_pane_changes as $pane_id => $changes) {
    $field_name = variable_get('commerce_' . $pane_id . '_field', '');
    if (empty($field_name)) {
      $field_name = $pane_id;
    }
    if (!array_key_exists($field_name, $infos)) {
      continue;
    }
    if ($changes['enabled'] == FALSE) {
      unset($infos[$field_name]);
    }
    elseif ($changes['weight'] != '') {
      $infos[$field_name]['#weight'] = $changes['weight'];
    }
  }
}
