ADD_DEFINITIONS(-DDO_EW_CORE_MUTEX_EXPORT)
ADD_DEFINITIONS(-DDO_EW_CORE_THREAD_EXPORT)

IF(UNIX)

ADD_LIBRARY(ew.core.threading SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/core/threading/mutex.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/core/threading/thread.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/core/threading/condition_variable.cpp 
${EW_PROJECT_ROOT}/implementation/platform/unix/core/threading/semaphore.cpp 
)

TARGET_LINK_LIBRARIES(ew.core.threading
ew.core.exception
ew.core.syscall
)


INSTALL(TARGETS ew.core.threading
LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)


ENDIF(UNIX)


IF(WIN32)

ADD_LIBRARY(ew.core.threading SHARED
${EW_PROJECT_ROOT}/implementation/platform/win32/threading/mutex.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/threading/thread.cpp
${EW_PROJECT_ROOT}/implementation/platform/win32/threading/condition_variable.cpp 
)

ENDIF(WIN32)
