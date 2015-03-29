ADD_LIBRARY(marks_mode SHARED
./marks_mode.cpp
)

TARGET_LINK_LIBRARIES(marks_mode
)


INSTALL(TARGETS marks_mode
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)
