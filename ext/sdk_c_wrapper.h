#ifndef OPENTELEMETRY_PHP_INSTRUMENTATION_SDK_C_WRAPPER_H
#define OPENTELEMETRY_PHP_INSTRUMENTATION_SDK_C_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void InitTracer();

void CleanupTracer();

void PreCodeProfiling(bool userland, char *function_name, char *scope_name, char *filename, char *class_name, int lineno);

// void prehook(zend_execute_data *execute_data);

// void posthook(zend_execute_data *execute_data, zval *return_value);

#ifdef __cplusplus
}
#endif

#endif //OPENTELEMETRY_PHP_INSTRUMENTATION_SDK_C_WRAPPER_H