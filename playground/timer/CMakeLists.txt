ADD_EXECUTABLE(timer_test
${EW_PROJECT_ROOT}/playground/timer/main.cpp
${EW_PROJECT_ROOT}/playground/timer/timer.cpp
)

TARGET_LINK_LIBRARIES(timer_test
ew.core.object
ew.core.exception
ew.core.time
)
