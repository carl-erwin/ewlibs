
ADD_LIBRARY(module_noop SHARED
./module_noop.cpp
)

TARGET_LINK_LIBRARIES(module_noop
)


INSTALL(TARGETS module_noop
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

# --------------------------------------------------------------------------------

ADD_LIBRARY(module_text_codec_ascii SHARED
./module_text_codec_ascii.cpp
)

TARGET_LINK_LIBRARIES(module_text_codec_ascii
)


INSTALL(TARGETS module_text_codec_ascii
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)
