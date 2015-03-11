#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{

using namespace ew::core::types;

/**
 * \class object
 * \brief this class will be the base of all classes
 * \todo  improve implementation children insert is slow
 */
class EW_CORE_OBJECT_EXPORT object
{
public:
	object();                                     /** do nothing  particularly */
	virtual ~object();                            /** destroy children recursively */

private:
	class private_data;
	private_data * d;

public:
	virtual const char * class_name() const;    /** returns an ascii string that describes the object's family  */

	virtual const char * name(void) const;          /** returns an ascii string that describes the entity name */
	virtual bool set_name(const char * name);   /** returns an ascii string that describes the entity name */

	// add object & name(const char *) // setter

private:
	/** \brief we do not allow object copy */
	object(const object & obj);                   /* not allowed */  // use deleted ?
	object & operator=(const object & obj);       /* not allowed */

public:
	virtual object * get_parent()          const;
	virtual bool     set_parent(object *);

	virtual bool     has_children()       const;
	virtual size_t   number_of_children() const;
	virtual object * get_child(u64 nr)    const;

	virtual bool add_child(object *);
	virtual bool remove_child(object *);

	// TODO: do not allow  overrive
	virtual bool is_parent_of(object *);
	virtual bool is_child_of(object *);
	virtual bool is_sibling_of(object *);
};

}
}
