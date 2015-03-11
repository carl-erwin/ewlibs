
ADD_EXECUTABLE(test_stdvector
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_stdvector PROPERTIES COMPILE_FLAGS -DTEST_STD_VECTOR)


TARGET_LINK_LIBRARIES(test_stdvector
ew.core.object
ew.core.exception
ew.core.time
)


###

ADD_EXECUTABLE(test_stddeque
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_stddeque PROPERTIES COMPILE_FLAGS -DTEST_STD_DEQUE)


TARGET_LINK_LIBRARIES(test_stddeque
ew.core.object
ew.core.exception
ew.core.time
)


###

ADD_EXECUTABLE(test_simpleArray
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_simpleArray PROPERTIES COMPILE_FLAGS "-DTEST_SIMPLE_ARRAY -DTEST_ERASE")

TARGET_LINK_LIBRARIES(test_simpleArray
ew.core.object
ew.core.exception
ew.core.time
)

###

ADD_EXECUTABLE(test_flexArray
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_flexArray PROPERTIES COMPILE_FLAGS -DTEST_FLEX_ARRAY)

TARGET_LINK_LIBRARIES(test_flexArray
ew.core.object
ew.core.exception
ew.core.time
)


###

ADD_EXECUTABLE(test_simpleBsp
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_simpleBsp PROPERTIES COMPILE_FLAGS -DTEST_SIMPLE_BSP)

TARGET_LINK_LIBRARIES(test_simpleBsp
ew.core.object
ew.core.exception
ew.core.time
)


###

ADD_EXECUTABLE(test_qtVector
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_qtVector PROPERTIES COMPILE_FLAGS -DTEST_QT_VECTOR)

TARGET_LINK_LIBRARIES(test_qtVector
ew.core.object
ew.core.exception
ew.core.time
Qt5Core
)


###

ADD_EXECUTABLE(test_boostArray
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_boostArray PROPERTIES COMPILE_FLAGS -DTEST_BOOST_ARRAY)

TARGET_LINK_LIBRARIES(test_boostArray
ew.core.object
ew.core.exception
ew.core.time
)



ADD_EXECUTABLE(test_all
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_all PROPERTIES COMPILE_FLAGS "-DTEST_STD_VECTOR -DTEST_STD_DEQUE -DTEST_SIMPLE_ARRAY -DTEST_FLEX_ARRAY -DTEST_QT_VECTOR -DTEST_SIMPLE_BSP")


TARGET_LINK_LIBRARIES(test_all
ew.core.object
ew.core.exception
ew.core.time
ew.filesystem
Qt5Core
)



ADD_EXECUTABLE(test_mapped
${EW_PROJECT_ROOT}/playground/flexible_array/test_main.cpp
)
SET_TARGET_PROPERTIES(test_mapped PROPERTIES COMPILE_FLAGS "-DTEST_FLEX_ARRAY -DTEST_MAPPED_FILE")


TARGET_LINK_LIBRARIES(test_mapped
ew.core.object
ew.core.exception
ew.core.time
ew.filesystem
)

