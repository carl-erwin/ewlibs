

ADD_EXECUTABLE(time_test
${EW_PROJECT_ROOT}/test/ew/core/time/time/test.cpp
)
 
INCLUDE_DIRECTORIES(
${FREETYPE_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(time_test
ew.core.exception
ew.core.threading
ew.core.time
ew.console
)
