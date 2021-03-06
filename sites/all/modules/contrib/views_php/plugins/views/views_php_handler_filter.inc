<?php

/**
 * A handler to filter a view using PHP defined by the administrator.
 *
 * @ingroup views_filter_handlers
 */
class views_php_handler_filter extends views_handler_filter {

  protected $php_static_variable = NULL;

  /**
   * Implements views_handler#can_expose().
   */
  function can_expose() { return FALSE; }

  /**
   * Implements views_object#admin_summary().
   */
  function admin_summary() {
    return t('PHP');
  }

  /**
   * Implements views_object#option_definition().
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['use_php_setup'] = array('default' => FALSE);
    $options['php_setup'] = array('default' => '');
    $options['php_filter'] = array('default' => '');
    return $options;
  }

  /**
   * Implements views_handler#option_definition().
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form += views_php_form_element($this,
      array('use_php_setup', t('Use setup code'), t('If checked, you can provide PHP code to be run once right before execution of the view. This may be useful to define functions to be re-used in the value and/or output code.')),
      array('php_setup', t('Setup code'), t('Code to run right before execution of the view.'), FALSE),
      array('$view', '$handler', '$static')
    );
    $form += views_php_form_element($this,
      FALSE,
      array('php_filter', t('Filter code'), t('If the code returns TRUE the current row is removed from the results.'), FALSE),
      array('$view', '$handler', '$static', '$row', '$data')
    );
  }

  /**
   * Implements views_handler_filter#query().
   */
  function query() {
    // Inform views_php_views_pre_execute() to seize control over the query.
    $this->view->views_php = TRUE;
  }

  /**
   *
   * @see views_php_views_pre_execute()
   */
  function php_pre_execute() {
    // Execute static PHP code.
    if (!empty($this->options['php_setup'])) {
      $function = views_php_create_function('$view, $handler, &$static', $this->options['php_setup'] . ';');
      ob_start();
      $closure = $function->getClosure();
      $closure($this->view, $this, $this->php_static_variable);
      ob_end_clean();
    }
  }

  /**
   *
   * @see views_php_plugin_pager::render()
   */
  function php_pre_render() {
    // Evaluate the PHP code.
    if (!empty($this->options['php_filter'])) {
      $function = views_php_create_function('$view, $handler, &$static, $row, $data', $this->options['php_filter'] . ';');
      ob_start();

      $normalized_row = new stdClass;
      foreach ($this->view->result as $i => $result) {
        foreach ($this->view->field as $id => $field) {
          $normalized_row->$id = $this->view->field[$id]->theme($result);
        }
        $closure = $function->getClosure();
        if ($closure($this->view, $this, $this->php_static_variable, $normalized_row, $result)) {
          unset($this->view->result[$i]);
        }
      }
      ob_end_clean();
    }
  }
}
