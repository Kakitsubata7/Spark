#pragma once

#if defined(SPARK_BUILD_AS_SHARED) && defined(_WIN32)
#   define SPARK_API __declspec(dllexport)
#else
#   define SPARK_API
#endif

extern "C" {
    
}
