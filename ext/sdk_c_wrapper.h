#ifndef OPENTELEMETRY_PHP_INSTRUMENTATION_SDK_C_WRAPPER_H
#define OPENTELEMETRY_PHP_INSTRUMENTATION_SDK_C_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void Sdk_Init();

bool Sdk_Work();


#ifdef __cplusplus
}
#endif

#endif //OPENTELEMETRY_PHP_INSTRUMENTATION_SDK_C_WRAPPER_H