# ------- 

SET(EW_TEST_NAME gfx_single_thread)

ADD_EXECUTABLE(${EW_TEST_NAME}.exe
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/test.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/common/gears.cpp
)

TARGET_LINK_LIBRARIES(${EW_TEST_NAME}.exe
ew.core.exception
ew.core.threading
ew.core.dll
ew.core.time
ew.core.system
ew.console
ew.core.maths
ew.graphics
ew.codecs.text.unicode.utf8
)


# --------------------------------

SET(EW_TEST_NAME gfx_v01)

ADD_EXECUTABLE(${EW_TEST_NAME}.exe
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/test.cpp
)

TARGET_LINK_LIBRARIES(${EW_TEST_NAME}.exe
ew.core.exception
ew.core.threading
ew.core.dll
ew.core.time
ew.core.system
ew.core.maths
ew.graphics
)



# --------------------------------

SET(EW_TEST_NAME gfx_v02)

ADD_EXECUTABLE(${EW_TEST_NAME}.exe
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/test.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/common/gears.cpp
)

TARGET_LINK_LIBRARIES(${EW_TEST_NAME}.exe
ew.core.exception
ew.core.threading
ew.core.dll
ew.core.time
ew.core.system
ew.core.maths
ew.graphics
ew.console
)


# --------------------------------

SET(EW_TEST_NAME gfx_v06)

ADD_EXECUTABLE(${EW_TEST_NAME}.exe
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/test.cpp
)

TARGET_LINK_LIBRARIES(${EW_TEST_NAME}.exe
ew.core.exception
ew.core.threading
ew.core.object
ew.core.time
ew.core.system
ew.core.maths
ew.graphics
)



# ----------

SET(EW_TEST_NAME gfx_v07)

ADD_EXECUTABLE(${EW_TEST_NAME}.exe
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/test.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/mymenu.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/mywindow.cpp
)

TARGET_LINK_LIBRARIES(${EW_TEST_NAME}.exe
ew.core.exception
ew.core.threading
ew.core.object
ew.core.time
ew.core.system
ew.core.maths
ew.graphics
)


# ----------------

SET(EW_TEST_NAME gfx_v09)

ADD_EXECUTABLE(${EW_TEST_NAME}.exe
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/test.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/mywindow.cpp
${EW_PROJECT_ROOT}/test/ew/graphics/${EW_TEST_NAME}/mymenu.cpp
)


TARGET_LINK_LIBRARIES(${EW_TEST_NAME}.exe
ew.core.exception
ew.core.threading
ew.core.object
ew.core.time
ew.core.system
ew.core.maths
ew.graphics
)
