ADD_LIBRARY(marks_mode SHARED
./marks_mode.cpp
)

TARGET_LINK_LIBRARIES(marks_mode
)


INSTALL(TARGETS marks_mode
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)


target_include_directories(marks_mode PUBLIC
  ${EasyWorld_SOURCE_DIR}/app/eedit/api/include/
  ${EasyWorld_SOURCE_DIR}/app/eedit/core
  ${EasyWorld_SOURCE_DIR}/app/eedit/core/text_layout/
)
