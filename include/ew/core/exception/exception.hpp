#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

namespace ew
{
namespace core
{

// exception management
EW_CORE_EXCEPTION_EXPORT void disable_exceptions();
EW_CORE_EXCEPTION_EXPORT void enable_exceptions();
EW_CORE_EXCEPTION_EXPORT bool are_exceptions_enabled();

class EW_CORE_EXCEPTION_EXPORT exception
{
public:
	exception(const char * str = "unknown exception");
	exception(const exception &);
	exception & operator=(const exception &);

	virtual ~exception();
	const char * get_name() const ;
	const char * get_class_name() const ;
	const char * what() const;
private:
	class private_data;
	private_data * d;
};

namespace exceptions
{

// list of basic exceptions
class EW_CORE_EXCEPTION_EXPORT invalid_parameter : public ew::core::exception
{
public:
	invalid_parameter() : ew::core::exception("invalid parameter") { }
};

class EW_CORE_EXCEPTION_EXPORT permission_denied : public ew::core::exception
{
public:
	permission_denied() : exception("permission denied") { }
};

class EW_CORE_EXCEPTION_EXPORT no_memory : public ew::core::exception
{
public:
	no_memory() : exception("no memory available") { }
};

class EW_CORE_EXCEPTION_EXPORT runtime_error : public ew::core::exception
{
public:
	runtime_error() : exception("runtime error") { }
};

class EW_CORE_EXCEPTION_EXPORT fake_function_exec : public ew::core::exception
{
public:
	fake_function_exec() : exception("fakefunctionexec error") { }
};

class EW_CORE_EXCEPTION_EXPORT object_creation_error : public ew::core::exception
{
public:
	object_creation_error() : exception("objectcreationerror error") { }
};

class EW_CORE_EXCEPTION_EXPORT object_destruction_error : public ew::core::exception
{
public:
	object_destruction_error() : exception("objectdestructionerror") { }
};

class EW_CORE_EXCEPTION_EXPORT internal_error : public ew::core::exception
{
public:
	internal_error() : exception("internal error") { }
};

class EW_CORE_EXCEPTION_EXPORT object_name_too_long : public ew::core::exception
{
public:
	object_name_too_long() : exception("object name too long") { }
};

class EW_CORE_EXCEPTION_EXPORT too_many_object_of_same_type : public ew::core::exception
{
public:
	too_many_object_of_same_type() : exception("too many object of same type") { }
};

class EW_CORE_EXCEPTION_EXPORT does_not_exists : public ew::core::exception
{
public:
	does_not_exists() : exception("does not exists") { }
};

class EW_CORE_EXCEPTION_EXPORT ressource_too_large : public ew::core::exception
{
public:
	ressource_too_large() : exception("ressource too large") { }
};

} // ! exceptions


} // ! core
} // ! ew

namespace ew
{

inline void Throw(ew::core::exception * e)
{
	if (ew::core::are_exceptions_enabled())
		throw e;
}

inline void Throw(ew::core::exception e)
{
	if (ew::core::are_exceptions_enabled())
		throw e;
}

}
