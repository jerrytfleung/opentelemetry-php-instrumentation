
#ifndef OPENTELEMETRY_FUNCTION_LEVEL_PROFILER_H
#define OPENTELEMETRY_FUNCTION_LEVEL_PROFILER_H

// void opentelemetry_observer_init(INIT_FUNC_ARGS);
// void observer_globals_init(void);
// void observer_globals_cleanup(void);

bool add_execute_hook(zval *pre_hook, zval *post_hook);
bool add_execute_internal_hook(zval *pre_hook, zval *post_hook);

#endif //OPENTELEMETRY_FUNCTION_LEVEL_PROFILER_H
