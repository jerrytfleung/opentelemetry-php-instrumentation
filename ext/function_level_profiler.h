
#ifndef OPENTELEMETRY_FUNCTION_LEVEL_PROFILER_H
#define OPENTELEMETRY_FUNCTION_LEVEL_PROFILER_H

void function_level_profiler_globals_init(void);
void function_level_profiler_globals_cleanup(void);
bool function_level_profiler_begin(char *fn, zend_execute_data *execute_data);
bool function_level_profiler_end(char *fn, zend_execute_data *execute_data, zval *retval);

bool add_function_level_profiler(char *fn, zval *pre_hook, zval *post_hook);

#endif //OPENTELEMETRY_FUNCTION_LEVEL_PROFILER_H
