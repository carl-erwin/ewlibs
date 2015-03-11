ADD_DEFINITIONS(-DDO_EW_CORE_OBJECT_EXPORT)


ADD_LIBRARY(ew.core.object SHARED
${EW_PROJECT_ROOT}/implementation/common/core/object/object.cpp
)

IF(UNIX)
TARGET_LINK_LIBRARIES(ew.core.object
ew.core.exception
ew.core.syscall
ew.console
)
ENDIF(UNIX)

IF(WIN32)

TARGET_LINK_LIBRARIES(ew.core.object
ew.core.exception
ew.console
)

ENDIF(WIN32)

IF(UNIX)
INSTALL(TARGETS ew.core.object
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)
ENDIF(UNIX)
