--TEST--
Check if hook_zend_execute_internal returns true
--EXTENSIONS--
opentelemetry
--FILE--
<?php
$ret = \OpenTelemetry\Instrumentation\hook_zend_execute_internal();

var_dump($ret);
?>
--EXPECT--
bool(false)
