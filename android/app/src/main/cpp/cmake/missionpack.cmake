# missionpack (Team Arena) game modules - cgame, qagame, ui
include_guard(GLOBAL)

if(NOT BUILD_GAME_LIBRARIES)
    return()
endif()

message(STATUS "Configuring missionpack game modules")

include(utils/set_output_dirs)

# Common definitions for game modules (needed for q_platform.h)
set(GAME_MODULE_DEFINITIONS
    ARCH_STRING="${ARCH_STRING}"
    IOQ3QUEST_VERSION="${IOQ3QUEST_VERSION_STRING}"
)

# Common include directories for game modules (SDL and OpenXR needed for VR headers)
set(GAME_MODULE_INCLUDE_DIRS
    ${SOURCE_DIR}/SDL2/include
    ${SOURCE_DIR}/OpenXR-SDK/include
)

# Shared sources for game modules
set(GAME_MODULE_SHARED_SOURCES
    ${SOURCE_DIR}/qcommon/q_math.c
    ${SOURCE_DIR}/qcommon/q_shared.c
)

# Background/physics shared by cgame and game
set(BG_SOURCES
    ${SOURCE_DIR}/game/bg_hash.c
    ${SOURCE_DIR}/game/bg_misc.c
    ${SOURCE_DIR}/game/bg_gameplay.c
    ${SOURCE_DIR}/game/bg_pmove.c
    ${SOURCE_DIR}/game/bg_slidemove.c
    ${SOURCE_DIR}/game/bg_lib.c
)

#############################################################################
# CGAME (Missionpack)
#############################################################################

set(MP_CGAME_SOURCES
    ${SOURCE_DIR}/cgame/cg_main.c
    ${SOURCE_DIR}/cgame/cg_consolecmds.c
    ${SOURCE_DIR}/cgame/cg_draw.c
    ${SOURCE_DIR}/cgame/cg_drawtools.c
    ${SOURCE_DIR}/cgame/cg_effects.c
    ${SOURCE_DIR}/cgame/cg_ents.c
    ${SOURCE_DIR}/cgame/cg_event.c
    ${SOURCE_DIR}/cgame/cg_info.c
    ${SOURCE_DIR}/cgame/cg_localents.c
    ${SOURCE_DIR}/cgame/cg_marks.c
    ${SOURCE_DIR}/cgame/cg_newdraw.c
    ${SOURCE_DIR}/cgame/cg_particles.c
    ${SOURCE_DIR}/cgame/cg_players.c
    ${SOURCE_DIR}/cgame/cg_playerstate.c
    ${SOURCE_DIR}/cgame/cg_predict.c
    ${SOURCE_DIR}/cgame/cg_scoreboard.c
    ${SOURCE_DIR}/cgame/cg_servercmds.c
    ${SOURCE_DIR}/cgame/cg_snapshot.c
    ${SOURCE_DIR}/cgame/cg_view.c
    ${SOURCE_DIR}/cgame/cg_weapons.c
    ${SOURCE_DIR}/cgame/cg_syscalls.c
    ${SOURCE_DIR}/ui/ui_shared.c
)

set(MP_CGAME_ALL_SOURCES
    ${MP_CGAME_SOURCES}
    ${BG_SOURCES}
    ${GAME_MODULE_SHARED_SOURCES}
)

add_library(cgame${ARCH}_${MISSIONPACK} SHARED ${MP_CGAME_ALL_SOURCES})
target_compile_definitions(cgame${ARCH}_${MISSIONPACK} PRIVATE
    CGAME
    MISSIONPACK
    ${GAME_MODULE_DEFINITIONS}
)
target_include_directories(cgame${ARCH}_${MISSIONPACK} PRIVATE
    ${SOURCE_DIR}/cgame
    ${SOURCE_DIR}/game
    ${SOURCE_DIR}/ui
    ${SOURCE_DIR}/qcommon
    ${GAME_MODULE_INCLUDE_DIRS}
)
target_compile_options(cgame${ARCH}_${MISSIONPACK} PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)
set_target_properties(cgame${ARCH}_${MISSIONPACK} PROPERTIES
    PREFIX ""
    OUTPUT_NAME cgame${ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${MISSIONPACK}
)

#############################################################################
# QAGAME (Missionpack)
#############################################################################

set(MP_GAME_SOURCES
    ${SOURCE_DIR}/game/g_main.c
    ${SOURCE_DIR}/game/ai_chat.c
    ${SOURCE_DIR}/game/ai_cmd.c
    ${SOURCE_DIR}/game/ai_dmnet.c
    ${SOURCE_DIR}/game/ai_dmq3.c
    ${SOURCE_DIR}/game/ai_main.c
    ${SOURCE_DIR}/game/ai_team.c
    ${SOURCE_DIR}/game/ai_vcmd.c
    ${SOURCE_DIR}/game/g_active.c
    ${SOURCE_DIR}/game/g_arenas.c
    ${SOURCE_DIR}/game/g_bot.c
    ${SOURCE_DIR}/game/g_client.c
    ${SOURCE_DIR}/game/g_cmds.c
    ${SOURCE_DIR}/game/g_combat.c
    ${SOURCE_DIR}/game/g_items.c
    ${SOURCE_DIR}/game/g_mem.c
    ${SOURCE_DIR}/game/g_misc.c
    ${SOURCE_DIR}/game/g_missile.c
    ${SOURCE_DIR}/game/g_mover.c
    ${SOURCE_DIR}/game/g_rotation.c
    ${SOURCE_DIR}/game/g_session.c
    ${SOURCE_DIR}/game/g_spawn.c
    ${SOURCE_DIR}/game/g_svcmds.c
    ${SOURCE_DIR}/game/g_target.c
    ${SOURCE_DIR}/game/g_team.c
    ${SOURCE_DIR}/game/g_trigger.c
    ${SOURCE_DIR}/game/g_unlagged.c
    ${SOURCE_DIR}/game/g_utils.c
    ${SOURCE_DIR}/game/g_weapon.c
    ${SOURCE_DIR}/game/g_syscalls.c
)

set(MP_GAME_ALL_SOURCES
    ${MP_GAME_SOURCES}
    ${BG_SOURCES}
    ${GAME_MODULE_SHARED_SOURCES}
)

add_library(qagame${ARCH}_${MISSIONPACK} SHARED ${MP_GAME_ALL_SOURCES})
target_compile_definitions(qagame${ARCH}_${MISSIONPACK} PRIVATE
    QAGAME
    MISSIONPACK
    ${GAME_MODULE_DEFINITIONS}
)
target_include_directories(qagame${ARCH}_${MISSIONPACK} PRIVATE
    ${SOURCE_DIR}/game
    ${SOURCE_DIR}/qcommon
    ${GAME_MODULE_INCLUDE_DIRS}
)
target_compile_options(qagame${ARCH}_${MISSIONPACK} PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)
set_target_properties(qagame${ARCH}_${MISSIONPACK} PROPERTIES
    PREFIX ""
    OUTPUT_NAME qagame${ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${MISSIONPACK}
)

#############################################################################
# UI (Missionpack - uses ui/ not q3_ui/)
#############################################################################

set(MP_UI_SOURCES
    ${SOURCE_DIR}/ui/ui_main.c
    ${SOURCE_DIR}/ui/ui_atoms.c
    ${SOURCE_DIR}/ui/ui_gameinfo.c
    ${SOURCE_DIR}/ui/ui_players.c
    ${SOURCE_DIR}/ui/ui_shared.c
    ${SOURCE_DIR}/ui/ui_syscalls.c
    ${SOURCE_DIR}/game/bg_misc.c
    ${SOURCE_DIR}/game/bg_gameplay.c
    ${SOURCE_DIR}/game/bg_lib.c
)

set(MP_UI_ALL_SOURCES
    ${MP_UI_SOURCES}
    ${GAME_MODULE_SHARED_SOURCES}
)

add_library(ui${ARCH}_${MISSIONPACK} SHARED ${MP_UI_ALL_SOURCES})
target_compile_definitions(ui${ARCH}_${MISSIONPACK} PRIVATE
    UI
    MISSIONPACK
    ${GAME_MODULE_DEFINITIONS}
)
target_include_directories(ui${ARCH}_${MISSIONPACK} PRIVATE
    ${SOURCE_DIR}/ui
    ${SOURCE_DIR}/game
    ${SOURCE_DIR}/qcommon
    ${GAME_MODULE_INCLUDE_DIRS}
)
target_compile_options(ui${ARCH}_${MISSIONPACK} PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)
set_target_properties(ui${ARCH}_${MISSIONPACK} PROPERTIES
    PREFIX ""
    OUTPUT_NAME ui${ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${MISSIONPACK}
)
