
#include "php.h"
#include "function_level_profiler.h"
#include "zend_execute.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "zend_attributes.h"
#include "php_opentelemetry.h"

typedef struct function_level_profiler {
    zval* pre_hook;
    zval* post_hook;
} function_level_profiler;

static void profiler_begin(zend_execute_data *execute_data, zval *hook) {
/*
    if (!zend_llist_count(hooks)) {
        return;
    }

    zval params[8];
    uint32_t param_count = 8;
    HashTable *attributes;
    ALLOC_HASHTABLE(attributes);
    zend_hash_init(attributes, 0, NULL, ZVAL_PTR_DTOR, 0);
    bool check_for_attributes =
        OTEL_G(attr_hooks_enabled) && func_has_withspan_attribute(execute_data);

    func_get_this_or_called_scope(&params[0], execute_data);
    func_get_attribute_args(&params[6], attributes, execute_data);
    func_get_args(&params[1], attributes, execute_data, check_for_attributes);
    func_get_declaring_scope(&params[2], execute_data);
    func_get_function_name(&params[3], execute_data);
    func_get_filename(&params[4], execute_data);
    func_get_lineno(&params[5], execute_data);

    ZVAL_ARR(&params[7], attributes);

    for (zend_llist_element *element = hooks->head; element;
         element = element->next) {
        zend_fcall_info fci = empty_fcall_info;
        zend_fcall_info_cache fcc = empty_fcall_info_cache;
        if (UNEXPECTED(zend_fcall_info_init((zval *)element->data, 0, &fci,
                                            &fcc, NULL, NULL) != SUCCESS)) {
            php_error_docref(NULL, E_WARNING,
                             "Failed to initialize pre hook callable");
            continue;
        }

        zval ret = {.u1.type_info = IS_UNDEF};
        fci.param_count = param_count;
        fci.params = params;
        fci.named_params = NULL;
        fci.retval = &ret;

        if (!is_valid_signature(fci, fcc)) {
            php_error_docref(NULL, E_CORE_WARNING,
                             "OpenTelemetry: pre hook invalid signature,"
                             " class=%s function=%s",
                             (Z_TYPE_P(&params[2]) == IS_NULL)
                                 ? "null"
                                 : Z_STRVAL_P(&params[2]),
                             Z_STRVAL_P(&params[3]));
            continue;
        }

        otel_exception_state save_state;
        exception_isolation_start(&save_state);

        if (zend_call_function(&fci, &fcc) == SUCCESS) {
            if (Z_TYPE(ret) == IS_ARRAY &&
                !zend_is_identical(&ret, &params[1])) {
                zend_ulong idx;
                zend_string *str_idx;
                zval *val;
                bool invalid_arg_warned = false;

                otel_arg_locator arg_locator;
                arg_locator_initialize(&arg_locator, execute_data);
                uint32_t args_initialized = arg_locator.provided;

                ZEND_HASH_FOREACH_KEY_VAL(Z_ARR(ret), idx, str_idx, val) {
                    const char *failure_reason = "";

                    if (str_idx != NULL) {
                        idx = func_get_arg_index_by_name(execute_data, str_idx);

                        if (idx == (uint32_t)-1) {
                            php_error_docref(
                                NULL, E_CORE_WARNING,
                                "OpenTelemetry: pre hook unknown "
                                "named arg %s, class=%s function=%s",
                                ZSTR_VAL(str_idx), zval_get_chars(&params[2]),
                                zval_get_chars(&params[3]));
                            continue;
                        }
                    }

                    zval *target = arg_locator_get_slot(&arg_locator, idx,
                                                        &failure_reason);

                    if (target == NULL) {
                        if (invalid_arg_warned) {
                            continue;
                        }

                        php_error_docref(NULL, E_CORE_WARNING,
                                         "OpenTelemetry: pre hook invalid "
                                         "argument index " ZEND_ULONG_FMT
                                         " - %s, class=%s function=%s",
                                         idx, failure_reason,
                                         zval_get_chars(&params[2]),
                                         zval_get_chars(&params[3]));
                        invalid_arg_warned = true;
                        continue;
                    }

                    if (idx >= args_initialized) {
                        // This slot was not initialized, need to initialize
                        // all slots between current and the last initialized
                        // one
                        for (uint32_t i = args_initialized; i < idx; i++) {
                            ZVAL_UNDEF(
                                arg_locator_get_slot(&arg_locator, i, NULL));
                            ZEND_ADD_CALL_FLAG(execute_data,
                                               ZEND_CALL_MAY_HAVE_UNDEF);
                        }

                        args_initialized = idx + 1;
                    } else {
                        // This slot was already initialized, need to
                        // decrement refcount before overwriting
                        zval_dtor(target);
                    }

                    if (idx >= arg_locator.reserved && Z_REFCOUNTED_P(val)) {
                        // If there are any "extra parameters" that are
                        // refcounted, then this flag must be set. While we
                        // cannot add any new extra parameter slots, this flag
                        // may not have been present because all the values
                        // were previously not refcounted
                        ZEND_ADD_CALL_FLAG(execute_data,
                                           ZEND_CALL_FREE_EXTRA_ARGS);
                    }

                    ZVAL_COPY(target, val);

                    if (idx < arg_locator.provided &&
                        Z_TYPE(params[1]) == IS_ARRAY) {
                        // This index is present in the array provided to begin
                        // hook, update it in that array as well
                        Z_TRY_ADDREF_P(val);
                        zend_hash_index_update(Z_ARR(params[1]), idx, val);
                    }
                }
                ZEND_HASH_FOREACH_END();

                arg_locator_store_extended(&arg_locator);

                // Update provided argument count if begin hook added arguments
                // that were not provided originally
                if (args_initialized > arg_locator.provided) {
                    ZEND_CALL_NUM_ARGS(execute_data) = args_initialized;
                }
            }
        }

        zend_object *suppressed = exception_isolation_end(&save_state);
        exception_isolation_handle_exception(suppressed, &params[2], &params[3],
                                             "pre hook");

        zval_dtor(&ret);
    }

    if (UNEXPECTED(ZEND_CALL_INFO(execute_data) & ZEND_CALL_MAY_HAVE_UNDEF)) {
        zend_object *exception = EG(exception);
        EG(exception) = (void *)(uintptr_t)-1;
        if (zend_handle_undef_args(execute_data) == FAILURE) {
            uint32_t arg_count = ZEND_CALL_NUM_ARGS(execute_data);
            for (uint32_t i = 0; i < arg_count; i++) {
                zval *arg = ZEND_CALL_VAR_NUM(execute_data, i);
                if (!Z_ISUNDEF_P(arg)) {
                    continue;
                }

                ZVAL_NULL(arg);
            }
        }
        EG(exception) = exception;
    }

    for (size_t i = 0; i < param_count; i++) {
        zval_dtor(&params[i]);
    }
*/
}

static void profiler_end(zend_execute_data *execute_data, zval *retval,
                         zval *hook) {
/*
    if (!zend_llist_count(hooks)) {
        return;
    }

    zval params[8];
    uint32_t param_count = 8;

    func_get_this_or_called_scope(&params[0], execute_data);
    func_get_args(&params[1], NULL, execute_data, false);
    func_get_retval(&params[2], retval);
    func_get_exception(&params[3]);
    func_get_declaring_scope(&params[4], execute_data);
    func_get_function_name(&params[5], execute_data);
    func_get_filename(&params[6], execute_data);
    func_get_lineno(&params[7], execute_data);

    for (zend_llist_element *element = hooks->tail; element;
         element = element->prev) {
        zend_fcall_info fci = empty_fcall_info;
        zend_fcall_info_cache fcc = empty_fcall_info_cache;
        if (UNEXPECTED(zend_fcall_info_init((zval *)element->data, 0, &fci,
                                            &fcc, NULL, NULL) != SUCCESS)) {
            php_error_docref(NULL, E_WARNING,
                             "Failed to initialize post hook callable");
            continue;
        }

        zval ret = {.u1.type_info = IS_UNDEF};
        fci.param_count = param_count;
        fci.params = params;
        fci.named_params = NULL;
        fci.retval = &ret;

        if (!is_valid_signature(fci, fcc)) {
            php_error_docref(NULL, E_CORE_WARNING,
                             "OpenTelemetry: post hook invalid signature, "
                             "class=%s function=%s",
                             (Z_TYPE_P(&params[4]) == IS_NULL)
                                 ? "null"
                                 : Z_STRVAL_P(&params[4]),
                             Z_STRVAL_P(&params[5]));
            continue;
        }

        otel_exception_state save_state;
        exception_isolation_start(&save_state);

        if (zend_call_function(&fci, &fcc) == SUCCESS) {

            if (!Z_ISUNDEF(ret) &&
                (fcc.function_handler->op_array.fn_flags &
                 ZEND_ACC_HAS_RETURN_TYPE) &&
                !(ZEND_TYPE_PURE_MASK(
                      fcc.function_handler->common.arg_info[-1].type) &
                  MAY_BE_VOID)) {
                if (execute_data->return_value) {
                    zval_ptr_dtor(execute_data->return_value);
                    ZVAL_COPY(execute_data->return_value, &ret);
                    zval_ptr_dtor(&params[2]);
                    ZVAL_COPY_VALUE(&params[2], &ret);
                    ZVAL_UNDEF(&ret);
                }
            }
        }

        zend_object *suppressed = exception_isolation_end(&save_state);
        exception_isolation_handle_exception(suppressed, &params[4], &params[5],
                                             "post hook");

        zval_dtor(&ret);
    }

    for (size_t i = 0; i < param_count; i++) {
        zval_dtor(&params[i]);
    }
*/
}

void function_level_profiler_begin(char *fn, zend_execute_data *execute_data) {
    zend_string *lc = zend_string_init(fn, strlen(fn), 0);
    function_level_profiler *profiler = zend_hash_find_ptr(OTEL_G(function_level_profiler_lookup), lc);
    if (!profiler || !profiler->pre_hook) {
        return;
    }
    zend_string_release(lc);
    profiler_begin(execute_data, profiler->pre_hook);
}

void function_level_profiler_end(char *fn, zend_execute_data *execute_data, zval *retval) {
    zend_string *lc = zend_string_init(fn, strlen(fn), 0);
    function_level_profiler *profiler = zend_hash_find_ptr(OTEL_G(function_level_profiler_lookup), lc);
    if (!profiler || !profiler->post_hook) {
        return;
    }
    zend_string_release(lc);
    profiler_end(execute_data, retval, profiler->post_hook);
}

static void free_function_level_profiler(function_level_profiler *profiler) {
    efree(profiler);
}

static void init_function_level_profiler(function_level_profiler *profiler) {
    if (profiler) {
        profiler->pre_hook = NULL;
        profiler->post_hook = NULL;
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
        profiler->pre_hook = pre_hook;
    }
    if (post_hook) {
        zval_add_ref(post_hook);
        profiler->post_hook = post_hook;
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
