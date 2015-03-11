ADD_EXECUTABLE(simple_threads
${EW_PROJECT_ROOT}/test/ew/core/threading/simple_threads.cpp
)
 
TARGET_LINK_LIBRARIES(simple_threads
ew.core.exception
ew.core.object
ew.core.threading
ew.core.time
ew.core.system
ew.console

#ew.core.dll
#ew.core.maths
#ew.graphics
)
