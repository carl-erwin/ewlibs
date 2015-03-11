/*

*/

#include <ew/core/exception/exception.hpp>

#undef DECLARE_wrapper

#ifdef EW_DEFINE_WRAPPER_FUNC_POINTER

# define DECLARE_wrapper(RET, NAME, ...) \
    RET NAME##_fake ( __VA_ARGS__ ) \
    { \
        throw ( fake_function_exec() ); \
        return (RET)0; \
    }  \
    \
    EW_EXPORT_SYM RET (*ew_##NAME) ( __VA_ARGS__ ) = NAME##_fake

#else

# define DECLARE_wrapper(RET, NAME, ...) \
    extern RET NAME##_fake ( __VA_ARGS__ ); \
    extern RET (*ew_##NAME) ( __VA_ARGS__ )

#endif
