--TEST--
Check if hook_zend_execute_ex are invoked
--EXTENSIONS--
opentelemetry
--INI--
opentelemetry.function_level_profiling=On
--FILE--
<?php
// \OpenTelemetry\Instrumentation\hook_zend_execute_ex(fn() => var_dump('PRE ex'), fn() => var_dump('POST ex'));
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE internal'), fn() => var_dump('POST internal'));

var_dump('HELLO');

?>
--EXPECT--
string(3) "PRE"
string(5) "HELLO"
string(4) "POST"
