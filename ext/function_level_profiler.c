
#include "php.h"
#include "function_level_profiler.h"
#include "zend_execute.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "zend_attributes.h"
#include "php_opentelemetry.h"

typedef struct function_level_profiler {
    zend_val* pre_hooks;
    zend_val* post_hooks;
} function_level_profiler;

static void free_function_level_profiler(function_level_profiler *profiler) {
    efree(profiler);
}

static void init_function_level_profiler(function_level_profiler *profiler) {
    pre_hooks = NULL;
    post_hooks = NULL;
}

static function_level_profiler *create_function_level_profiler() {
    function_level_profiler *profiler = emalloc(sizeof(function_level_profiler));
    init_function_level_profiler(profiler);
    return profiler;
}

bool add_execute_hook(zval *pre_hook, zval *post_hook) {

/*
    otel_observer *observer = zend_hash_find_ptr(ht, lc);
    if (!observer) {
        observer = create_observer();
        zend_hash_update_ptr(ht, lc, observer);
    }
    zend_string_release(lc);
*/
    if (pre_hook) {
        zval_add_ref(pre_hook);
        // zend_llist_add_element(&observer->pre_hooks, pre_hook);
    }
    if (post_hook) {
        zval_add_ref(post_hook);
        // zend_llist_add_element(&observer->post_hooks, post_hook);
    }
    return true;
}

bool add_execute_internal_hook(zval *pre_hook, zval *post_hook) {
    return true;
}