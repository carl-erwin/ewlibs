# base

ADD_DEFINITIONS(-DDO_EW_PLATFORM_EXPORT)
ADD_DEFINITIONS(-DDO_EW_GRAPHICS_EXPORT)
ADD_DEFINITIONS(-DDO_EW_GRAPHICS_RENDERER_OPENGL_EXPORT)

IF(UNIX)
ADD_LIBRARY(ew.graphics SHARED
${EW_PROJECT_ROOT}/implementation/common/graphics/irenderer/irenderer.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/libGL.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/libGLU.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/renderer.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/framebuffer.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/gui/display/display.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/gui/widget/widget.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/gui/event/push.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/gui/event/keys.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/event/event.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/gui.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/display.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/display_private_data.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/window.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/pixmap.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/renderer/rendering_context.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/renderer/renderer_init.cpp

${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/x11_libX11.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/x11_libGLX.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/x11/x11_event.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/x11/x11_pointer.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/gui/x11/x11_keyboard.cpp

${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/font/font.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/graphics/font/libfreetype2.cpp
)


INCLUDE_DIRECTORIES(
${FREETYPE_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(ew.graphics
ew.core.exception
ew.core.system
ew.core.threading
ew.core.dll
ew.core.device
ew.core.object
ew.console
ew.codecs.text.unicode.utf8
)


#${FREETYPE_LIBRARY}

INSTALL(TARGETS ew.graphics
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)



ENDIF(UNIX)


IF(WIN32)
ADD_LIBRARY(ew.graphics SHARED
${EW_PROJECT_ROOT}/implementation/common/graphics/irenderer/irenderer.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/gui/display/display.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/gui/event/event.cpp
# missing renderer_init
# missing libWgl
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/gui/display.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/gui/gui.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/gui/widget.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/gui/window.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/renderer/renderer.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/graphics/renderer/rendering_context.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/libGL.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/libGLU.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/renderer.cpp
${EW_PROJECT_ROOT}/implementation/common/graphics/renderer/opengl/framebuffer.cpp
)

TARGET_LINK_LIBRARIES(ew.graphics
opengl32
glu32
ew.core.exception
ew.core.threading
ew.core.dll
ew.core.device
ew.core.object
)

ENDIF(WIN32)
