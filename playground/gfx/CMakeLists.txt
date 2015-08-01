# -------

SET(EW_WIDGET_FACTORY widget_factory)

ADD_EXECUTABLE(${EW_WIDGET_FACTORY}.exe
${EW_PROJECT_ROOT}/playground/gfx/widget_factory/widget_factory.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/common/gears.cpp
)

TARGET_LINK_LIBRARIES(${EW_WIDGET_FACTORY}.exe
ew.core.exception
ew.core.dll
ew.core.time
ew.core.system
ew.core.maths
ew.graphics
ew.codecs.text.unicode.utf8
)
