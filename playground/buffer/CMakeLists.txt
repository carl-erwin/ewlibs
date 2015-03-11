ADD_EXECUTABLE(buffer_test
${EW_PROJECT_ROOT}/playground/buffer/main.cpp
)

TARGET_LINK_LIBRARIES(buffer_test
ew.core.object
ew.core.exception
ew.core.time
ew.filesystem
)

ADD_EXECUTABLE(flexible_array_test
${EW_PROJECT_ROOT}/playground/buffer/flexible_array_test.cpp
)

TARGET_LINK_LIBRARIES(flexible_array_test
ew.core.object
ew.core.exception
ew.core.time
ew.filesystem
)

