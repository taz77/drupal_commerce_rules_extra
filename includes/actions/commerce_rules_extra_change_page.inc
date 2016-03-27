<?php

/**
 * @file
 * Rules action Change Page Properties Action.
 */

/**
 * Helper function to return the action info to the main module.
 */
function commerce_rules_extra_change_page_action_info() {
  return [
    'label' => t('Change page properties'),
    'group' => t('Commerce Checkout'),
    'parameter' => [
      'page_id' => [
        'type' => 'text',
        'label' => t('Page to change properties for'),
        'options list' => 'commerce_rules_extra_get_pages',
        'default value' => '<same>',
        'restriction' => 'input',
      ],
      'title' => [
        'type' => 'text',
        'label' => t('Page title'),
        'description' => t('Set the page title.'),
        'optional' => TRUE,
        'restriction' => 'input',
        'translatable' => TRUE,
      ],
      'help' => [
        'type' => 'text',
        'label' => t('Help text'),
        'description' => t('Set the help text displayed at the top of the page.'),
        'optional' => TRUE,
        'restriction' => 'input',
        'translatable' => TRUE,
      ],
      'submit_value' => [
        'type' => 'text',
        'label' => t('Submit button value'),
        'description' => t('Set the value of the submit button.'),
        'optional' => TRUE,
        'restriction' => 'input',
        'translatable' => TRUE,
      ],
    ],
  ];
}

/**
 * Callback function for rule commerce_rules_extra_change_page.
 *
 * Store in global variables all properties changes for a page.
 */
function commerce_rules_extra_change_page($page_id, $title, $help, $submit_value) {
  global $_commerce_rules_extra_page_changes;
  if (!$_commerce_rules_extra_page_changes) {
    $_commerce_rules_extra_page_changes = [];
  }
  $_commerce_rules_extra_page_changes[$page_id] = [
    'title' => $title,
    'help' => $help,
    'submit_value' => $submit_value,
  ];
}
