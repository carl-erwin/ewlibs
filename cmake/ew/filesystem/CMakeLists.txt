ADD_DEFINITIONS(-DDO_EW_FILESYSTEM_EXPORT)

IF(UNIX)
ADD_LIBRARY(ew.filesystem SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/filesystem/file.cpp
)
ENDIF(UNIX)

IF(WIN32)
ADD_LIBRARY(ew.filesystem SHARED
${EW_PROJECT_ROOT}/implementation/platform/win32/filesystem/file.cpp
)
ENDIF(WIN32)


TARGET_LINK_LIBRARIES(ew.filesystem
ew.core.exception
ew.core.threading
ew.core.object
)


IF(UNIX)

INSTALL(TARGETS ew.filesystem
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)
ENDIF(UNIX)

