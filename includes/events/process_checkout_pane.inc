<?php
function process_checkout_pane_event_info() {
  return array (
    'label' => t('Process checkout pane'),
    'group' => t('Commerce Checkout'),
    'variables' => array (
      'commerce_order' => array (
        'type' => 'commerce_order',
        'label' => t('current order')
      )
    )
  );
}

/**
 * Implements hook_commerce_checkout_pane_info_alter
 *
 * Invoke all rules for process_checkout_pane event and alter panes properties returned by rules
 */
function commerce_rules_extra_commerce_checkout_pane_info_alter(&$checkout_panes) {
  global $pane_changes;
  if (! $pane_changes)
    $pane_changes = array ();
  $urls = array (
    'checkout','system'
  );
  drupal_alter('checkout_pane_urls', $urls);
  if (in_array(arg(0), $urls)) {
    $commerce_order = menu_get_object('commerce_order');
    if (null == $commerce_order) {
      $commerce_order = commerce_cart_get_properties(FALSE, array (), "current_cart_order");
    }
    rules_invoke_all('process_checkout_pane', $commerce_order);
    foreach ( $pane_changes as $pane_id => $changes ) {
      $checkout_panes [$pane_id] ['enabled'] = $changes ["enabled"];
      if ($changes ["weight"] != "")
        $checkout_panes [$pane_id] ['weight'] = $changes ["weight"];
      if ($changes ["page_id"] != "<same>") {
        $checkout_panes [$pane_id] ['page'] = $changes ['page_id'];
      }
    }
  }
}