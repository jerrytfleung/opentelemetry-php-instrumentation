--TEST--
Check if hook_zend_execute_internal multiple hooks are invoked (more hooks)
--EXTENSIONS--
opentelemetry
--INI--
opentelemetry.function_level_profiling=On
--FILE--
<?php
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE_1'), fn() => var_dump('POST_1'));
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE_2'), fn() => var_dump('POST_2'));
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE_3'), fn() => var_dump('POST_3'));
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE_4'), fn() => var_dump('POST_4'));
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump('PRE_5'), fn() => var_dump('POST_5'));

function helloWorld() {
    var_dump('CALL');
}

helloWorld();
?>
--EXPECT--
string(5) "PRE_1"
string(6) "POST_1"
string(5) "PRE_1"
string(5) "PRE_2"
string(6) "POST_2"
string(6) "POST_1"
string(5) "PRE_1"
string(5) "PRE_2"
string(5) "PRE_3"
string(6) "POST_3"
string(6) "POST_2"
string(6) "POST_1"
string(5) "PRE_1"
string(5) "PRE_2"
string(5) "PRE_3"
string(5) "PRE_4"
string(6) "POST_4"
string(6) "POST_3"
string(6) "POST_2"
string(6) "POST_1"
string(5) "PRE_1"
string(5) "PRE_2"
string(5) "PRE_3"
string(5) "PRE_4"
string(5) "PRE_5"
string(4) "CALL"
string(6) "POST_5"
string(6) "POST_4"
string(6) "POST_3"
string(6) "POST_2"
string(6) "POST_1"
