# baseq3 game modules - cgame, qagame, ui
include_guard(GLOBAL)

if(NOT BUILD_GAME_LIBRARIES)
    return()
endif()

message(STATUS "Configuring baseq3 game modules")

include(utils/set_output_dirs)

# Common definitions for game modules (needed for q_platform.h)
set(GAME_MODULE_DEFINITIONS
    ARCH_STRING="${ARCH_STRING}"
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
    ${SOURCE_DIR}/game/bg_misc.c
    ${SOURCE_DIR}/game/bg_pmove.c
    ${SOURCE_DIR}/game/bg_slidemove.c
    ${SOURCE_DIR}/game/bg_lib.c
)

#############################################################################
# CGAME
#############################################################################

set(CGAME_SOURCES
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
)

set(CGAME_ALL_SOURCES
    ${CGAME_SOURCES}
    ${BG_SOURCES}
    ${GAME_MODULE_SHARED_SOURCES}
)

add_library(cgame${ARCH}_${BASEGAME} SHARED ${CGAME_ALL_SOURCES})
target_compile_definitions(cgame${ARCH}_${BASEGAME} PRIVATE
    CGAME
    ${GAME_MODULE_DEFINITIONS}
)
target_include_directories(cgame${ARCH}_${BASEGAME} PRIVATE
    ${SOURCE_DIR}/cgame
    ${SOURCE_DIR}/game
    ${SOURCE_DIR}/qcommon
    ${GAME_MODULE_INCLUDE_DIRS}
)
target_compile_options(cgame${ARCH}_${BASEGAME} PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)
set_target_properties(cgame${ARCH}_${BASEGAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME cgame${ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BASEGAME}
)

#############################################################################
# QAGAME
#############################################################################

set(GAME_SOURCES
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

set(GAME_ALL_SOURCES
    ${GAME_SOURCES}
    ${BG_SOURCES}
    ${GAME_MODULE_SHARED_SOURCES}
)

add_library(qagame${ARCH}_${BASEGAME} SHARED ${GAME_ALL_SOURCES})
target_compile_definitions(qagame${ARCH}_${BASEGAME} PRIVATE
    QAGAME
    ${GAME_MODULE_DEFINITIONS}
)
target_include_directories(qagame${ARCH}_${BASEGAME} PRIVATE
    ${SOURCE_DIR}/game
    ${SOURCE_DIR}/qcommon
    ${GAME_MODULE_INCLUDE_DIRS}
)
target_compile_options(qagame${ARCH}_${BASEGAME} PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)
set_target_properties(qagame${ARCH}_${BASEGAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME qagame${ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BASEGAME}
)

#############################################################################
# UI (q3_ui)
#############################################################################

set(UI_SOURCES
    ${SOURCE_DIR}/q3_ui/ui_main.c
    ${SOURCE_DIR}/q3_ui/ui_addbots.c
    ${SOURCE_DIR}/q3_ui/ui_atoms.c
    ${SOURCE_DIR}/q3_ui/ui_cdkey.c
    ${SOURCE_DIR}/q3_ui/ui_cinematics.c
    ${SOURCE_DIR}/q3_ui/ui_comfort.c
    ${SOURCE_DIR}/q3_ui/ui_confirm.c
    ${SOURCE_DIR}/q3_ui/ui_connect.c
    ${SOURCE_DIR}/q3_ui/ui_controls2.c
    ${SOURCE_DIR}/q3_ui/ui_controls3.c
    ${SOURCE_DIR}/q3_ui/ui_credits.c
    ${SOURCE_DIR}/q3_ui/ui_demo2.c
    ${SOURCE_DIR}/q3_ui/ui_display.c
    ${SOURCE_DIR}/q3_ui/ui_gameinfo.c
    ${SOURCE_DIR}/q3_ui/ui_ingame.c
    ${SOURCE_DIR}/q3_ui/ui_loadconfig.c
    ${SOURCE_DIR}/q3_ui/ui_menu.c
    ${SOURCE_DIR}/q3_ui/ui_mfield.c
    ${SOURCE_DIR}/q3_ui/ui_mods.c
    ${SOURCE_DIR}/q3_ui/ui_network.c
    ${SOURCE_DIR}/q3_ui/ui_options.c
    ${SOURCE_DIR}/q3_ui/ui_playermodel.c
    ${SOURCE_DIR}/q3_ui/ui_players.c
    ${SOURCE_DIR}/q3_ui/ui_playersettings.c
    ${SOURCE_DIR}/q3_ui/ui_preferences.c
    ${SOURCE_DIR}/q3_ui/ui_qmenu.c
    ${SOURCE_DIR}/q3_ui/ui_removebots.c
    ${SOURCE_DIR}/q3_ui/ui_saveconfig.c
    ${SOURCE_DIR}/q3_ui/ui_serverinfo.c
    ${SOURCE_DIR}/q3_ui/ui_servers2.c
    ${SOURCE_DIR}/q3_ui/ui_setup.c
    ${SOURCE_DIR}/q3_ui/ui_sound.c
    ${SOURCE_DIR}/q3_ui/ui_sparena.c
    ${SOURCE_DIR}/q3_ui/ui_specifyserver.c
    ${SOURCE_DIR}/q3_ui/ui_splevel.c
    ${SOURCE_DIR}/q3_ui/ui_sppostgame.c
    ${SOURCE_DIR}/q3_ui/ui_spskill.c
    ${SOURCE_DIR}/q3_ui/ui_startserver.c
    ${SOURCE_DIR}/q3_ui/ui_team.c
    ${SOURCE_DIR}/q3_ui/ui_teamorders.c
    ${SOURCE_DIR}/q3_ui/ui_video.c
    ${SOURCE_DIR}/ui/ui_syscalls.c
    ${SOURCE_DIR}/game/bg_misc.c
    ${SOURCE_DIR}/game/bg_lib.c
)

set(UI_ALL_SOURCES
    ${UI_SOURCES}
    ${GAME_MODULE_SHARED_SOURCES}
)

add_library(ui${ARCH}_${BASEGAME} SHARED ${UI_ALL_SOURCES})
target_compile_definitions(ui${ARCH}_${BASEGAME} PRIVATE
    UI
    ${GAME_MODULE_DEFINITIONS}
)
target_include_directories(ui${ARCH}_${BASEGAME} PRIVATE
    ${SOURCE_DIR}/q3_ui
    ${SOURCE_DIR}/ui
    ${SOURCE_DIR}/game
    ${SOURCE_DIR}/qcommon
    ${GAME_MODULE_INCLUDE_DIRS}
)
target_compile_options(ui${ARCH}_${BASEGAME} PRIVATE
    ${COMMON_COMPILE_OPTIONS}
)
set_target_properties(ui${ARCH}_${BASEGAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME ui${ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BASEGAME}
)
