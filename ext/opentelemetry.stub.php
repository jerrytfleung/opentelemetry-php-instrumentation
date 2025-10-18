<?php

/** @generate-class-entries */

namespace OpenTelemetry\Instrumentation;

/**
 * @param string|null $class The (optional) hooked function's class. Null for a global/built-in function.
 * @param string $function The hooked function's name.
 * @param \Closure|null $pre function($class, array $params, ?string $class, string $function, ?string $filename, ?int $lineno, ?array $span_args, ?array $span_attributes): $params
 *        You may optionally return modified parameters.
 * @param \Closure|null $post function($class, array $params, $returnValue, ?Throwable $exception): $returnValue
 *        You may optionally return modified return value.
 * @return bool Whether the observer was successfully added
 *
 * @see https://github.com/open-telemetry/opentelemetry-php-instrumentation
 */
function hook(
    string|null $class,
    string $function,
    ?\Closure $pre = null,
    ?\Closure $post = null,
): bool {}

/**
 * @param \Closure|null $pre function($class, array $params, ?string $class, ?string $function, ?string $filename, ?int $lineno, ?string $scope, ?array $sample_args, ?array $sample_attributes): $params
 *        You may optionally return modified parameters.
 * @param \Closure|null $post function($class, array $params, $returnValue, ?Throwable $exception): $returnValue
 *        You may optionally return modified return value.
 * @return bool Whether the hook was successfully added
 *
 * @see https://github.com/open-telemetry/opentelemetry-php-instrumentation
 */
function hook_zend_execute_ex(
    ?\Closure $pre = null,
    ?\Closure $post = null,
): bool {}

/**
 * @param \Closure|null $pre function($class, array $params, ?string $filename, ?int $lineno, ?array $sample_args, ?array $sample_attributes): $params
 *        You may optionally return modified parameters.
 * @param \Closure|null $post function($class, array $params, $returnValue, ?Throwable $exception): $returnValue
 *        You may optionally return modified return value.
 * @return bool Whether the hook was successfully added
 *
 * @see https://github.com/open-telemetry/opentelemetry-php-instrumentation
 */
function hook_zend_execute_internal(
    ?\Closure $pre = null,
    ?\Closure $post = null,
): bool {}
