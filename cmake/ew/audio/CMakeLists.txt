ADD_DEFINITIONS(-DDO_EW_AUDIO_EXPORT)

SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/common/audio)

ADD_LIBRARY(ew.audio SHARED
${EW_PROJECT_ROOT}/implementation/common/empty.cpp
)


TARGET_LINK_LIBRARIES(ew.audio
ew.core.object
ew.core.device
)
