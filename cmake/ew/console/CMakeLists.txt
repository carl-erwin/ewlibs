ADD_DEFINITIONS(-DDO_EW_CONSOLE_EXPORT)

IF(UNIX)

ADD_LIBRARY(ew.console SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/console/console.cpp
)


TARGET_LINK_LIBRARIES(ew.console
ew.core.syscall
ew.core.program
ew.core.threading
)


INSTALL(TARGETS ew.console
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)



ENDIF(UNIX)

IF(WIN32)
ADD_LIBRARY(ew.console SHARED
${EW_PROJECT_ROOT}/implementation/platform/win32/console/console.cpp
)


TARGET_LINK_LIBRARIES(ew.console
ew.core.threading
)


ENDIF(WIN32)
