#ifndef __EW__IMPLEMENTATION__PLATFORM__Win32_FILE_HH__
#define __EW__IMPLEMENTATION__PLATFORM__Win32_FILE_HH__

// on va utiliser les fstream ?

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/file/ifile.hpp>

namespace ew
{
namespace core
{
namespace platform
{
namespace win32
{
namespace file
{

using namespace ew::core::types;
using namespace ew::core::filesystem;

class EW_PLATFORM_EXPORT Win32_File : public ew::core::File::IFile
{
public:

	// implementation must copy somewere fileName in order to use in const char * getName(void)
	Win32_File(const char * fileName);
	virtual ~Win32_File();


	virtual bool open(u32 mode);
	virtual bool reOpen(u32 mode);

	virtual bool close(void);

	virtual bool is_opened();
	virtual bool is_closed();
	virtual const char * getFileName();
	virtual const u32 get_open_mode();

	virtual bool write(u8 * buffer, u64 bufferSize);
	virtual bool read(u8 * buffer, u64 bufferSize);
	virtual bool setPos(u64 ,
			    ew::core::objects::STREAM::Location from,
			    ew::core::objects::STREAM::Direction dir);
	virtual u64  getPos(void);
};

}
}
}
}
} // ! namespace ew::core::Platform::Win32::File

#endif
