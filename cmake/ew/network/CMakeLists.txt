# base

#ADD_DEFINITIONS(-DDO_EW_PLATFORM_EXPORT)
ADD_DEFINITIONS(-DDO_EW_NETWORK_EXPORT)

IF(UNIX)
ADD_LIBRARY(ew.network SHARED
${EW_PROJECT_ROOT}/implementation/platform/unix/network/network.cpp
${EW_PROJECT_ROOT}/implementation/platform/unix/network/connection.cpp
)

TARGET_LINK_LIBRARIES(ew.network
ew.core.exception
ew.core.threading
ew.core.dll
ew.core.object
)

# ew.core.device


INSTALL(TARGETS ew.network
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)


ENDIF(UNIX)

IF(WIN32)
ADD_LIBRARY(ew.network SHARED
${EW_PROJECT_ROOT}/implementation/platform/win32/network/network.cpp

)

TARGET_LINK_LIBRARIES(ew.network
ew.core.exception
ew.core.threading
ew.core.dll
ew.core.object
)

ENDIF(WIN32)
