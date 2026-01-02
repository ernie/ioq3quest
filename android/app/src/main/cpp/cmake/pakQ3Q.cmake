# pakQ3Q.cmake - Build pakQ3Q.pk3 from source directory
#
# This creates the VR-specific asset pack that contains:
# - Custom UI elements
# - VR-specific scripts and shaders
# - Other Quest VR modifications

set(PAKQ3Q_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../pakQ3Q")
set(PAKQ3Q_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/../assets")
set(PAKQ3Q_OUTPUT_FILE "${PAKQ3Q_OUTPUT_DIR}/pakQ3Q.pk3")

# Build pakQ3Q.pk3 from source (always regenerate to catch new files)
add_custom_target(pakQ3Q ALL
    COMMAND ${CMAKE_COMMAND} -E remove -f "${PAKQ3Q_OUTPUT_FILE}"
    COMMAND ${CMAKE_COMMAND} -E tar cf
        "${PAKQ3Q_OUTPUT_FILE}" --format=zip
        .
    WORKING_DIRECTORY "${PAKQ3Q_SOURCE_DIR}"
    COMMENT "Building pakQ3Q.pk3 from source"
)

message(STATUS "pakQ3Q.pk3 will be built from: ${PAKQ3Q_SOURCE_DIR}")
message(STATUS "pakQ3Q.pk3 output: ${PAKQ3Q_OUTPUT_FILE}")
