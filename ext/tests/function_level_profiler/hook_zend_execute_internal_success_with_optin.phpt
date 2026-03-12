--TEST--
Check if hook_zend_execute_internal returns true
--EXTENSIONS--
opentelemetry
--INI--
opentelemetry.function_level_profiling=On
--FILE--
<?php
$ret = \OpenTelemetry\Instrumentation\hook_zend_execute_internal();

var_dump($ret);
?>
--EXPECT--
bool(true)
