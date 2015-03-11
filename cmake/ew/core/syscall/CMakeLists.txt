ADD_DEFINITIONS(-DDO_EW_CORE_SYSCALL_EXPORT)


IF(UNIX)

ADD_LIBRARY(ew.core.syscall SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/core/syscall/syscall.cpp
)

TARGET_LINK_LIBRARIES(ew.core.syscall
rt
pthread
)


INSTALL(TARGETS ew.core.syscall
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

ENDIF(UNIX)

##############################################################################

IF(WIN32)

ADD_LIBRARY(ew.core.syscall SHARED
${EW_PROJECT_ROOT}/implementation/platform/win32/core/syscall/syscall.cpp
)


INSTALL(TARGETS ew.core.syscall
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)


ENDIF(WIN32)
