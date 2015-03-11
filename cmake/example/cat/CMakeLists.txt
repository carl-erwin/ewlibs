ADD_EXECUTABLE(cat
${EW_PROJECT_ROOT}/test/example/cat.cpp
)
 
TARGET_LINK_LIBRARIES(cat
ew.core.exception
ew.core.object
ew.console
ew.filesystem
)
