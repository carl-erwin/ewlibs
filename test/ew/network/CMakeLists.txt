
ADD_EXECUTABLE(network_client
${EW_PROJECT_ROOT}/test/ew/network/connection/client.cpp
${EW_PROJECT_ROOT}/test/ew/network/connection/tftp.cpp
)

INCLUDE_DIRECTORIES(
${FREETYPE_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(network_client
ew.core.exception
ew.core.object
ew.core.time
ew.core.system
ew.network
ew.filesystem
)

ADD_EXECUTABLE(network_server
${EW_PROJECT_ROOT}/test/ew/network/connection/server.cpp
)

INCLUDE_DIRECTORIES(
${FREETYPE_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(network_server
ew.core.exception
ew.core.object
ew.core.time
ew.core.system
ew.network
)
