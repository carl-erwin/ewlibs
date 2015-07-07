ADD_LIBRARY(text_mode SHARED
./text_mode.cpp
)

TARGET_LINK_LIBRARIES(text_mode
)


INSTALL(TARGETS text_mode
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)


target_include_directories(text_mode PUBLIC
  ${EasyWorld_SOURCE_DIR}/app/eedit/api/include/
  ${EasyWorld_SOURCE_DIR}/app/eedit/core
  ${EasyWorld_SOURCE_DIR}/app/eedit/core/text_layout/
)
