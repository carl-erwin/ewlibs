#pragma once

// Ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/object/iloadable_object.hpp>
#include <ew/core/exception/exception.hpp>

namespace ew
{
namespace core
{

using namespace ew::core::types;

// sub system init
EW_CORE_DLL_EXPORT bool init(void);
EW_CORE_DLL_EXPORT bool quit(void);

class EW_CORE_DLL_EXPORT dll : public ew::core::objects::loadable_object
{
public:
	dll(const char * fileName);
	virtual ~dll();

public:
	// object
	virtual const char * class_name(void) const;   /** returns ew::core::DLL:Dll */
	//
	virtual const char * filename();                /** returns the dll on-disk filename */

	virtual bool load();                               /** load the dll returns false on error */
	virtual bool unload();                             /** unload the dll returns false on error */
	virtual bool is_loaded();                           /** returns loading status */

	virtual void * symbol_by_name(const char * sym);     /** returns the running virtual address
                                                   of a given symbols (function, variable) */
private:
	class private_data;
	class private_data * d;
public:
	class symbol
	{
	public:
		const char * name;
		void ** ptr;
	};
};

// Dll exceptions
class EW_CORE_EXPORT dll_loading_error : public ew::core::exception
{
public:
	dll_loading_error() : exception("Dll loading error") { }
};

class EW_CORE_EXPORT dll_unloading_error : public ew::core::exception
{
public:
	dll_unloading_error() : exception("Dll unloading error") { }
};

class EW_CORE_EXPORT dll_symbol_not_found : public ew::core::exception
{
public:
	dll_symbol_not_found() : exception("Dll symbol not found") { }
};

}
} // ! namespace ew::core::DLL
