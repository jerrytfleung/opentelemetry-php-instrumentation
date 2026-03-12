--TEST--
Check if hook_zend_execute_internal are invoked
--EXTENSIONS--
opentelemetry
--INI--
opentelemetry.function_level_profiling=On
--FILE--
<?php
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE'), fn() => var_dump('POST'));

function helloWorld() {
    var_dump('HELLO');
}

helloWorld();

?>
--EXPECT--
string(3) "PRE"
string(5) "HELLO"
string(4) "POST"
