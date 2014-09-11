
Commerce Rules Extra (CRE) module for Drupal 7.x.
This module adds many useful rules for Drupal Commerce

REQUIREMENTS
------------
* Drupal Commerce
* Rules
* Taxonomy

INSTALLATION INSTRUCTIONS
-------------------------








This module currently provides the following rules components:

Rules events
Process checkout pane : Fires when a pane is processed during checkout process
Line item quantity has changed (occurs when adding ou removing a product to cart and when a quantity has been modified in cart form).
Rules conditions
Line item product has term : Test if a line item has a product with a specified term
Line item product has terms : Test if a line item has a product with one or all specified terms
Total product with term quantity comparison : Total of products with term and with a specified quantity
Total product of type quantity comparison : Total of products of specified type and with a specified quantity
Total product of type amount comparison : Total of products of specified type and
with a specified amount
Rules actions
Change pane properties : Change visibility, page, weight of a pane
Get the referencing node from the line item : Set the referenced node (product display) into a variable to be reused in another action