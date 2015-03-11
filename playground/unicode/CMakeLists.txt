ADD_EXECUTABLE(unicode_test
${EW_PROJECT_ROOT}/playground/unicode/unicode_test.cpp
)

TARGET_LINK_LIBRARIES(unicode_test
ew.core.object
ew.core.exception
ew.filesystem
ew.codecs.text.unicode.utf8
)


ADD_EXECUTABLE(unicode_reverse_decode_test
${EW_PROJECT_ROOT}/playground/unicode/unicode_reverse_decode_test.cpp
)

TARGET_LINK_LIBRARIES(unicode_reverse_decode_test
ew.core.object
ew.core.exception
ew.filesystem
ew.codecs.text.unicode.utf8
)
