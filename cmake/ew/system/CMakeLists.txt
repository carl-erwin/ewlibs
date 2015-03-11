ADD_DEFINITIONS(-DDO_EW_SYSTEM_EXPORT)

IF(UNIX)

ADD_LIBRARY(ew.core.system SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/system/exit.cpp
)

INSTALL(TARGETS ew.core.system
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

ENDIF(UNIX)

IF(WIN32)

ADD_LIBRARY(ew.core.system SHARED
${EW_PROJECT_ROOT}/implementation/platform/win32/system/exit.cpp
)

ENDIF(WIN32)

