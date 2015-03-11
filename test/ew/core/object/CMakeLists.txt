ADD_EXECUTABLE(core_object
${EW_PROJECT_ROOT}/test/ew/core/object/test.cpp
)
 
TARGET_LINK_LIBRARIES(core_object
ew.core.exception
ew.core.object
)
