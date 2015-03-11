#ifndef __EW__IMPLEMENTATION__PLATFORM__WIN32_DLL_HH__
#define __EW__IMPLEMENTATION__PLATFORM__WIN32_DLL_HH__

#include "ew_config.hpp"

#include "core/types/types.hpp"
#include "core/file/ifile.hpp"
#include "core/file/dll/idll.hpp"


#include <windows.h>

namespace ew
{
namespace implementation
{
namespace Platform
{
namespace Win32
{
namespace File
{

typedef HINSTANCE dllHandlerType;

using namespace ew::core::types;
using namespace ew::core::File;

class EW_DECLSPEC Win32_Dll : public ew::core::File::IDll
{
public:
	Win32_Dll(const char * fileName);
	virtual      ~Win32_Dll();

	const char * class_name(void) const;
	const char * getName() const;

	virtual bool load();
	virtual bool unload();

	virtual const char * getFileName();

	virtual bool   isLoaded(void);
	virtual void  * getSymbol(const char * symbol);

private:
	char    *   _fileName;
	dllHandlerType  _handler;
};

// -----------------------------------------------------------

class EW_DECLSPEC DllInterface : public ew::core::File::IDllInterface
{
public:
	const char * class_name(void) const
	{
		return 0;
	};
	const char * getName() const
	{
		return 0;
	};


	IDll  * loadDll(const char * name);
	bool  releaseDll(IDll * idll);
};


} // ! namespace ew::implementation::Platform::Win32::File
} // ! namespace ew::implementation::Platform::Win32
} // ! namespace ew::implementation::Platform
} // ! namespace ew::implementation
} // ! namespace ew

#endif // ! __EW__IMPLEMENTATION__PLATFORM__WIN32_DLL_HH__
