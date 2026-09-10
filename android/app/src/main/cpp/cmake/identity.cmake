# Project identity - sets project name and version
include_guard(GLOBAL)

set(PROJECT_NAME "trinity-standalone")
set(PROJECT_VERSION "1.2.0")
set(CLIENT_NAME "ioquake3")

# Full version string from CI tag (GITHUB_REF_NAME) or git describe
# CMAKE_SOURCE_DIR is android/app/src/main/cpp, so navigate up to repo root for .git
set(REPO_ROOT "${CMAKE_SOURCE_DIR}/../../../../..")
if(DEFINED ENV{GITHUB_REF_NAME} AND "$ENV{GITHUB_REF_NAME}" MATCHES "^v[0-9]")
    set(TRINITY_STANDALONE_VERSION_STRING "$ENV{GITHUB_REF_NAME}")
elseif(EXISTS "${REPO_ROOT}/.git")
    execute_process(
        COMMAND git describe --tags --always --dirty
        WORKING_DIRECTORY "${REPO_ROOT}"
        OUTPUT_VARIABLE TRINITY_STANDALONE_VERSION_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_DESCRIBE_RESULT)

    if(NOT GIT_DESCRIBE_RESULT EQUAL 0 OR NOT TRINITY_STANDALONE_VERSION_STRING)
        set(TRINITY_STANDALONE_VERSION_STRING "unknown")
    endif()
else()
    set(TRINITY_STANDALONE_VERSION_STRING "unknown")
endif()

# Game directories
set(BASEGAME "baseq3")
set(MISSIONPACK "missionpack")

# Architecture for Android arm64
set(ARCH "aarch64")
set(ARCH_STRING "aarch64")

# Module names for game libraries
set(CGAME_MODULE "cgame${ARCH}")
set(GAME_MODULE "qagame${ARCH}")
set(UI_MODULE "ui${ARCH}")
