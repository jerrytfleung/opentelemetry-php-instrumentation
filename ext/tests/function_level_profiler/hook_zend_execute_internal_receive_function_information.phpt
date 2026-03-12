--TEST--
Check if hook_zend_execute_internal hooks receives function information
--EXTENSIONS--
opentelemetry
--INI--
opentelemetry.function_level_profiling=On
--FILE--
<?php
\OpenTelemetry\Instrumentation\hook_zend_execute_internal(fn() => var_dump(func_get_args()), fn() => var_dump(func_get_args()));

function helloWorld() {
    var_dump('CALL');
}

helloWorld();
?>
--EXPECTF--
array(7) {
  [0]=>
  NULL
  [1]=>
  array(1) {
    [0]=>
    string(4) "CALL"
  }
  [2]=>
  NULL
  [3]=>
  string(8) "var_dump"
  [4]=>
  NULL
  [5]=>
  NULL
  [6]=>
  int(1)
}
string(4) "CALL"
array(8) {
  [0]=>
  NULL
  [1]=>
  array(1) {
    [0]=>
    string(4) "CALL"
  }
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  string(8) "var_dump"
  [6]=>
  NULL
  [7]=>
  NULL
}
