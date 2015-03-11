#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/types/types.hpp>


namespace ew
{
namespace core
{

using namespace ew::core::types;

//TODO: ??? class device_capabilities;


/**
   @namespace ew::core
   @class device
*/

class EW_CORE_DEVICE_EXPORT device : public ew::core::object
{
public:
	//  device(u32 index = 0);
	//  virtual ~device();

	virtual u32 getSystemIndex(void) = 0;
	virtual bool is_opened(void) = 0;
	virtual bool is_closed(void) = 0;

	virtual bool open(void) = 0;
	virtual bool close(void) = 0;

	/*
	add Here Common Device operation unix style ?
	we must find an elegant way for getCapabilities()
	*/
	//   virtual bool lock(void) = 0;
	//   virtual bool unlock(void) = 0;
	//   virtual bool isLocked(void) = 0;
	//   virtual bool isUnlocked(void) = 0;
	//  etc ...
};

}
} // ! ew::core
