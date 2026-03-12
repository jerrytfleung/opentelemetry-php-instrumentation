#include "sdk_c_wrapper.h"

#include <iostream>
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

void InitTracer() {
    auto exporter  = opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create();
    auto processor = opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter));
    std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processor));
    //set the global trace provider
    opentelemetry::trace::Provider::SetTracerProvider(provider);
}

void CleanupTracer() {
    std::shared_ptr<opentelemetry::trace::TracerProvider> none;
    trace_api::Provider::SetTracerProvider(none);
}

void PreCodeProfiling(bool userland, char *function_name, char *scope_name, char *filename, char *class_name, int lineno) {

    // std::string function_name_ = function_name ? function_name : "undefined";
    // std::string scope_name_ = scope_name ? scope_name : "undefined";
    // std::string filename_ = filename ? filename : "undefined";
    // std::string class_name_ = class_name ? class_name : "undefined";
    // int lineno_ = lineno;

    // std::cout << function_name_ << " " << scope_name_ << " " << filename_ << " " << class_name_ << " " << lineno_ << std::endl;

    std::cout << "Hello" << std::endl;
}
/*
void prehook(zend_execute_data *execute_data) {
    if (execute_data != nullptr) {
        if (execute_data->func != nullptr) {
            bool userland = execute_data->func->type == ZEND_USER_FUNCTION;
            std::cout << "userland: " << userland << std::endl;
            if (execute_data->func->common.function_name != nullptr) {
                std::string function_name = execute_data->func->common.function_name->val;
                std::cout << "function_name: " << function_name << std::endl;
            }
            if (execute_data->func->common.scope != nullptr && execute_data->func->common.scope->name != nullptr) {
                std::string scope_name = execute_data->func->common.scope->name->val;
                std::cout << "scope_name: " << scope_name << std::endl;
            }
            if (execute_data->func->op_array.filename != nullptr) {
                std::string filename = execute_data->func->op_array.filename->val;
                std::cout << "filename: " << filename << std::endl;
            }
        }
        if (execute_data->This.value.obj != nullptr && execute_data->This.value.obj->ce != nullptr && execute_data->This.value.obj->ce->name != nullptr) {
            std::string class_name = execute_data->This.value.obj->ce->name->val;
            std::cout << "class_name: " << class_name << std::endl;
        }
        if (execute_data->opline != nullptr) {
            int lineno = execute_data->opline->lineno;
            std::cout << "lineno: " << lineno << std::endl;
        }
    }
}

void posthook(zend_execute_data *execute_data, zval *return_value) {
    if (execute_data != nullptr) {
    }
    if (return_value != nullptr) {
    }
}
*/
/*
bool Sdk_Work() {
    auto tracer = opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("opentelemetry-php-instrumentation");
    auto span = tracer->StartSpan("RollDiceServer");
    span->End();
    return true;
}
*/