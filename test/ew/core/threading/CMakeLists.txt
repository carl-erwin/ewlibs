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


ADD_EXECUTABLE(simple_threads2
${EW_PROJECT_ROOT}/test/ew/core/threading/simple_threads2.cpp
)

TARGET_LINK_LIBRARIES(simple_threads2
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



ADD_EXECUTABLE(fifo_test
${EW_PROJECT_ROOT}/test/ew/core/threading/fifo_test.cpp
)

TARGET_LINK_LIBRARIES(fifo_test
ew.core.exception
ew.core.object
ew.core.threading
ew.core.time
ew.core.system
ew.console
)
