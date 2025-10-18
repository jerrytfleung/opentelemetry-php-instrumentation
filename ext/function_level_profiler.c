
#include "php.h"
#include "function_level_profiler.h"
#include "zend_execute.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "zend_attributes.h"
#include "php_opentelemetry.h"

typedef struct function_level_profiler {
    zval* pre_hooks;
    zval* post_hooks;
} function_level_profiler;

static void free_function_level_profiler(function_level_profiler *profiler) {
    efree(profiler);
}

static void init_function_level_profiler(function_level_profiler *profiler) {
    if (profiler) {
        profiler->pre_hooks = NULL;
        profiler->post_hooks = NULL;
    }
}

static function_level_profiler *create_function_level_profiler() {
    function_level_profiler *profiler = emalloc(sizeof(function_level_profiler));
    init_function_level_profiler(profiler);
    return profiler;
}

bool add_function_level_profiler(char *fn, zval *pre_hook, zval *post_hook) {
    zend_string *lc = zend_string_init(fn, strlen(fn), 0);
    function_level_profiler *profiler = zend_hash_find_ptr(OTEL_G(function_level_profiler_lookup), lc);
    if (!profiler) {
        profiler = create_function_level_profiler();
        zend_hash_update_ptr(OTEL_G(function_level_profiler_lookup), lc, profiler);
    }
    zend_string_release(lc);

    if (pre_hook) {
        zval_add_ref(pre_hook);
        profiler->pre_hooks = pre_hook;
    }
    if (post_hook) {
        zval_add_ref(post_hook);
        profiler->post_hooks = post_hook;
    }
    return true;
}

static void destroy_function_level_profiler_lookup(zval *zv) { free_function_level_profiler(Z_PTR_P(zv)); }

void function_level_profiler_globals_init(void) {
    if (!OTEL_G(function_level_profiler_lookup)) {
        ALLOC_HASHTABLE(OTEL_G(function_level_profiler_lookup));
        zend_hash_init(OTEL_G(function_level_profiler_lookup), 8, NULL,
                       destroy_function_level_profiler_lookup, 0);
    }
}

void function_level_profiler_globals_cleanup(void) {
    if (OTEL_G(function_level_profiler_lookup)) {
        zend_hash_destroy(OTEL_G(function_level_profiler_lookup));
        FREE_HASHTABLE(OTEL_G(function_level_profiler_lookup));
        OTEL_G(function_level_profiler_lookup) = NULL;
    }
}
