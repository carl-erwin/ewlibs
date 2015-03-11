ADD_DEFINITIONS(-DDO_EW_CORE_PROGRAM_EXPORT)

IF(UNIX)

ADD_LIBRARY(ew.core.program SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/core/program/environment.cpp
)

#TARGET_LINK_LIBRARIES(ew.core.program
#ew.core.syscall
#)

INSTALL(TARGETS ew.core.program
LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

ENDIF(UNIX)
