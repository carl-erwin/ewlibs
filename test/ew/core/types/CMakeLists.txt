ADD_EXECUTABLE(core_types
${EW_PROJECT_ROOT}/test/ew/core/types/test.cpp
)
 
TARGET_LINK_LIBRARIES(core_types
ew.core.exception
ew.core.object
ew.console
)
