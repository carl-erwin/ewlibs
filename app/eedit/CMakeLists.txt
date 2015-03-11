SET(EW_APP_NAME eedit)

INCLUDE_DIRECTORIES(${EW_PROJECT_ROOT}/app/eedit/)
INCLUDE_DIRECTORIES(${EW_PROJECT_ROOT}/app/eedit/api/include)

ADD_EXECUTABLE(${EW_APP_NAME}.exe


# TODO: app/CMakeLists.txt
${EW_PROJECT_ROOT}/app/eedit/api/src/byte_buffer.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/buffer_log.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/screen.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/codec.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/text_codec.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/mark.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/editor_buffer.cpp
${EW_PROJECT_ROOT}/app/eedit/api/src/editor_view.cpp

${EW_PROJECT_ROOT}/app/eedit/main/main.cpp
${EW_PROJECT_ROOT}/app/eedit/application/application.cpp


# TODO: core/CMakeLists.txt
${EW_PROJECT_ROOT}/app/eedit/core/core.cpp
${EW_PROJECT_ROOT}/app/eedit/core/log.cpp
${EW_PROJECT_ROOT}/app/eedit/core/input/event/input_event_map.cpp
${EW_PROJECT_ROOT}/app/eedit/core/module/module.cpp
${EW_PROJECT_ROOT}/app/eedit/core/rpc/rpc.cpp

${EW_PROJECT_ROOT}/app/eedit/core/core.cpp

${EW_PROJECT_ROOT}/app/eedit/core/mode/text/text_mode.cpp
${EW_PROJECT_ROOT}/app/eedit/core/mode/marks/marks_mode.cpp

${EW_PROJECT_ROOT}/app/eedit/core/text_layout.cpp


${EW_PROJECT_ROOT}/app/eedit/core/filters/byte_mode.cpp
${EW_PROJECT_ROOT}/app/eedit/core/filters/text_decoder.cpp
${EW_PROJECT_ROOT}/app/eedit/core/filters/hex_layout_mode.cpp
${EW_PROJECT_ROOT}/app/eedit/core/filters/unicode_layout_mode.cpp
${EW_PROJECT_ROOT}/app/eedit/core/filters/tab_expansion_layout_mode.cpp
${EW_PROJECT_ROOT}/app/eedit/core/filters/screen_layout.cpp

${EW_PROJECT_ROOT}/app/eedit/core/process_event_ctx.cpp  # TODO: rename in core/event/process_event_ctx.cpp ?
${EW_PROJECT_ROOT}/app/eedit/core/event/process_event.cpp

${EW_PROJECT_ROOT}/app/eedit/core/undo/undo.cpp


# TODO: ui/CMakeLists.txt
${EW_PROJECT_ROOT}/app/eedit/ui/debug/debug.cpp

${EW_PROJECT_ROOT}/app/eedit/ui/ew/ui.cpp
${EW_PROJECT_ROOT}/app/eedit/ui/ew/main_window.cpp
${EW_PROJECT_ROOT}/app/eedit/ui/ew/tab_bar.cpp
${EW_PROJECT_ROOT}/app/eedit/ui/ew/status_bar.cpp
${EW_PROJECT_ROOT}/app/eedit/ui/ew/scroll_bar.cpp

${EW_PROJECT_ROOT}/app/eedit/ui/ncurses/ncurses.cpp
)


TARGET_LINK_LIBRARIES(${EW_APP_NAME}.exe
ew.core.exception
ew.core.system
ew.core.threading
ew.core.object
ew.core.time
ew.console
ew.core.maths
ew.graphics
ew.filesystem
ncursesw
)

INSTALL(TARGETS ${EW_APP_NAME}.exe
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)

# core modules
ADD_SUBDIRECTORY(${EW_PROJECT_ROOT}/app/eedit/modules)
