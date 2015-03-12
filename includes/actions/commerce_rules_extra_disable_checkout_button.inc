<?php

/**
 * @file
 * Rules action Disable Checkout Button Action.
 */

/**
 * Helper function to return the action info to the main module.
 */
function commerce_rules_extra_disable_checkout_button_action_info() {
  return array(
    'label' => t('Disable Final Checkout Button'),
    'group' => t('Commerce Checkout'),
    'parameter' => array(
      'page_id' => array(
        'type' => 'text',
        'label' => t('Page to change properties for'),
        'options list' => 'commerce_rules_extra_get_pages',
        'default value' => '<same>',
        'restriction' => 'input',
      ),
    ),
  );
}

/**
 * Callback function for rule commerce_rules_extra_disable_checkout_button.
 */
function commerce_rules_extra_disable_checkout_button($page_id, $title, $help, $submit_value) {

}
