--TEST--
Check if hook_zend_execute_ex returns true
--EXTENSIONS--
opentelemetry
--FILE--
<?php
$ret = \OpenTelemetry\Instrumentation\hook_zend_execute_ex();

var_dump($ret);
?>
--EXPECT--
bool(true)
