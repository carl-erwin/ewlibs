
SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/common/core/device)

ADD_DEFINITIONS(
-D_REENTRANT
-DDO_EW_CORE_DEVICE_EXPORT
)


ADD_LIBRARY(ew.core.device SHARED
${EW_PROJECT_ROOT}/implementation/common/empty.cpp
)

TARGET_LINK_LIBRARIES(ew.core.device
ew.core.object
)

IF(UNIX)

INSTALL(TARGETS ew.core.device
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

ENDIF(UNIX)
