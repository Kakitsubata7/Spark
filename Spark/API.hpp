#pragma once

#if defined(SPARK_BUILD_AS_SHARED) && defined(_WIN32)
#   define SPARK_API __declspec(dllexport)
#else
#   define SPARK_API
#endif

extern "C" {
    SPARK_API void* spark_new_env();
    SPARK_API void spark_close_env(void* env);

    SPARK_API void spark_push_int();
    SPARK_API void spark_push_float();
    SPARK_API void spark_push_bool();

    SPARK_API void spark_pop(int num);
}
