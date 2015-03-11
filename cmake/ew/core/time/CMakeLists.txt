ADD_DEFINITIONS(-DDO_EW_CORE_TIME_EXPORT)

IF(UNIX)
SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/platform/unix/core/time/)

ADD_LIBRARY(ew.core.time SHARED
${MOD_SRC_DIR}/time_unix.cpp
)

TARGET_LINK_LIBRARIES(ew.core.time
ew.core.syscall
)

INSTALL(TARGETS ew.core.time
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)



ENDIF(UNIX)


IF(WIN32)
SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/platform/win32/time/)

ADD_LIBRARY(ew.core.time SHARED
${MOD_SRC_DIR}/time_win32.cpp
)

TARGET_LINK_LIBRARIES(ew.core.time
ew.core.object
)

ENDIF(WIN32)
