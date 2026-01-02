# Set output directories for targets
include_guard(GLOBAL)

function(set_output_dirs TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "SUBDIRECTORY" "")

    if(ARG_SUBDIRECTORY)
        set(OUTPUT_SUBDIR "/${ARG_SUBDIRECTORY}")
    else()
        set(OUTPUT_SUBDIR "")
    endif()

    set_target_properties(${TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_SUBDIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_SUBDIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}${OUTPUT_SUBDIR}"
    )

    # Also set per-config directories
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set_target_properties(${TARGET} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}${OUTPUT_SUBDIR}"
            LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}${OUTPUT_SUBDIR}"
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}${OUTPUT_SUBDIR}"
        )
    endforeach()
endfunction()
