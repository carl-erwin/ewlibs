ADD_DEFINITIONS(-DDO_EW_CORE_DLL_EXPORT)

IF(UNIX)
SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/platform/unix/core/dll/)

ADD_LIBRARY(ew.core.dll SHARED ${MOD_SRC_DIR}/dll.cpp)

SET(DSO_LIB_LOADER, '')

IF (LINUX_OS MATCHES 1)
SET(DSO_LIB_LOADER dl)
ENDIF()

TARGET_LINK_LIBRARIES(ew.core.dll
${DSO_LIB_LOADER}
ew.core.exception
ew.core.object
)


INSTALL(TARGETS ew.core.dll
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

ENDIF(UNIX)

IF(WIN32)
SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/platform/win32/dll/)

ADD_LIBRARY(ew.core.dll SHARED ${MOD_SRC_DIR}/dll_win32.cpp)

TARGET_LINK_LIBRARIES(ew.core.dll
ew.core.exception
ew.core.object
)

ENDIF(WIN32)

