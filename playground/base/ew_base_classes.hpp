
#include "../../include/ew/core/types/types.hpp"


using namespace ew::core::types;

// --------------------------------------------------------------------------

namespace playground
{

class EwIObject
{
public:
	virtual ~EwIObject() {}
};


// template ?
class EwIBuffer : public EwIObject
{
public:
	EwIBuffer(u64 sz = 0);
	virtual ~EwIBuffer() { };

	virtual bool alloc(u64 sz) = 0;
	virtual bool release(void) = 0;

	virtual bool realloc(u64 sz) = 0;
	virtual bool resize(u64 sz, bool clear = false) = 0;

	///
	virtual const u8 * get
	(u64 pos)
	{
		return 0;
	}
	//  u8 * operator[] (u64 pos) = 0;
};

class Ewrunable_object : public EwIObject
{
public:
	virtual ~Ewrunable_object() {}
	virtual void run() = 0;
};

class EwINonRunableObject : private Ewrunable_object { }
;

// --------------------------------------------------------------------------

class EwIReadableObject : public EwIObject
{
public:
	virtual ~EwIReadableObject() {}
	;
	virtual bool read(u8 * buffer, u64 & size) = 0;
	virtual bool read(class EwIBuffer &) = 0;
};

class EwINonReadableObject : private EwIReadableObject { }
;


// --------------------------------------------------------------------------

class EwIWriteableObject : public EwIObject
{
public:
	virtual bool write(const u8 * buffer, u64 & size) = 0;
	virtual bool write(class EwIBuffer &) = 0;
};

// --------------------------------------------------------------------------

class EwISeekableObject : public EwIObject
{
public:
	virtual ~EwISeekableObject()  {}
	virtual bool seek(u64 pos)
	{
		return false;
	}
};

// --------------------------------------------------------------------------

class EwIExecutableObject : public EwIObject
{
public:
	virtual bool exec(void) = 0;
};

// --------------------------------------------------------------------------

class EwILoadableObject : public EwIObject
{
public:
	virtual bool load(void) = 0;
};

class EwINonLoadableObject : private EwILoadableObject {};




// --------------------------------------------------------------------------

enum open_mode {
	Read = (1 << 0),
	Write = (1 << 1),
	ReadOnly = (1 << 2),
	WriteOnly = (1 << 3),
	ReadWrite = (1 << 4),
	// Append    = (1 << 5) // ???
};

class EwIStream : public EwIObject
{
public:
	virtual ~EwIStream() {}
	virtual bool open(open_mode) = 0;
	virtual bool close() = 0;
};

// --------------------------------------------------------------------------

class EwIProgram :
	public EwIStream,
	public EwIExecutableObject {}
;


// --------------------------------------------------------------------------------

class EwISocket :
	public EwIStream,
	public EwIReadableObject,
	public EwIWriteableObject
{
public:
	virtual ~EwISocket() {}

	virtual bool open() = 0;
	virtual bool connect() = 0;
	virtual bool bind() = 0;
	virtual bool accept() = 0;
private:
};

// TCP
// --- interface ---
class EwITCP_Socket : public EwISocket
{
public:
	virtual ~EwITCP_Socket() {}
}
;

// warpper to current implementation
class TCP_Socket : public EwITCP_Socket
{
public:
	virtual ~TCP_Socket() {}
}
;


// UDP
// --- interface ---
class EwIUDP_Socket : public EwISocket
{
public:
	virtual ~EwIUDP_Socket() {}
}
;

// warpper to current implementation
class UDP_Socket : public EwIUDP_Socket
{
public:
	virtual ~UDP_Socket() {}
};

} // ! namespace playground

// --------------------------------------------------------------------------


