# cURL configuration for Android (dynamic loading)
include_guard(GLOBAL)

option(USE_HTTP "HTTP download support" ON)

if(NOT USE_HTTP)
    return()
endif()

message(STATUS "Configuring cURL (dynamic loading)")

set(CURL_DIR ${SOURCE_DIR}/curl-8.11.0)

# cURL is loaded dynamically at runtime via dlopen
list(APPEND CLIENT_DEFINITIONS USE_CURL USE_CURL_DLOPEN)
list(APPEND CLIENT_INCLUDE_DIRS ${CURL_DIR}/include)

# Deploy the library for runtime loading
set(CURL_LIBRARY ${SOURCE_DIR}/libs/android/arm64-v8a/libcurl.so)
list(APPEND CLIENT_DEPLOY_LIBRARIES ${CURL_LIBRARY})
