ADD_DEFINITIONS(-DDO_EW_CODECS_TEXT_UNICODE_UTF8)

SET(MOD_SRC_DIR ${EW_PROJECT_ROOT}/implementation/common/codecs/text/unicode/utf8/)

ADD_LIBRARY(ew.codecs.text.unicode.utf8 SHARED ${MOD_SRC_DIR}/utf8.cpp)

TARGET_LINK_LIBRARIES(ew.codecs.text.unicode.utf8
ew.core.exception
)

IF(UNIX)
INSTALL(TARGETS ew.codecs.text.unicode.utf8
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)
ENDIF(UNIX)