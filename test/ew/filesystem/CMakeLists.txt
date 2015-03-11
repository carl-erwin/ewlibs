

ADD_EXECUTABLE(file_basic_tests
${EW_PROJECT_ROOT}/test/ew/filesystem/basic_operations.cpp
)

TARGET_LINK_LIBRARIES(file_basic_tests
ew.core.exception
ew.core.object
ew.console
ew.filesystem
)
