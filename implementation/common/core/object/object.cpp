#include <string>
#include <mutex>

#include <ew/core/object/object.hpp>



#include <vector>
#include <ew/core/container/simple_array.hpp>


#include <cstring>

#include <ew/Utils.hpp>

namespace ew
{
namespace core
{

using namespace ew::core::types;

class object::private_data
{
public:
    std::mutex  _mtx;
    std::string _name;
    object  * _parent;
    size_t    _parent_index;
    size_t    _nrChildren;

    //typedef typename ew::core::container::simple_array<object *> container;
#ifdef EW_TARGET_SYSTEM_WINDOWS
    typedef std::vector<object *> container;
#else
    typedef typename std::vector<object *> container;
#endif
    container * _children_vec;

    private_data(const private_data&) = delete;
    private_data & operator = (const private_data&) = delete;

    private_data()
        :
        _mtx(),
        _name("unnamed object"),
        _parent(nullptr),
        _parent_index(0),
        _nrChildren(0),
        _children_vec(nullptr)
    {

    }

    ~private_data()
    {
        _parent = nullptr;
        delete _children_vec;
        _children_vec = nullptr;
    }

    container & children_array()
    {
        return *(_children_vec);
    }

    void lock()
    {
        return _mtx.lock();
    }
    bool try_lock()
    {
        return _mtx.try_lock();
    }
    void unlock()
    {
        return _mtx.unlock();
    }

};


object::object()
    : d(new private_data())
{
}

// -------------------------------------------------

object::~object()
{
    {
        std::lock_guard<std::mutex> locker(d->_mtx);

        // TODO: add to prevent usage
        // this->d->valid = true;

        // if automatic_direction destroy flag is set
        // Destroy children

        if (has_children()) {

            size_t nrChildren = d->children_array().size();

            for (u32 i = 0; i < nrChildren; ++i) {
                if (d->children_array()[i] != nullptr) {
                    d->unlock();
                    {
                        delete d->children_array()[i];
                    }
                    d->lock();
                    d->children_array()[i] = nullptr;
                }
                --d->_nrChildren;
            }
        }
    }

    // remove parent link
    set_parent(nullptr);

    delete d;
}

void object::lock()
{
    return d->_mtx.lock();
}
bool object::try_lock()
{
    return d->_mtx.try_lock();
}
void object::unlock()
{
    return d->_mtx.unlock();
}

// ------------------------------
// name

const char * object::class_name() const
{
    return "ew::core::::object";
}

const char * object::name() const
{
    return d->_name.c_str();
}

bool object::set_name(const char * name)
{
    if (name == nullptr)
        return false;

    d->_name = name;
    return true;
}

// ------------------------------
object * object::get_parent() const
{
    return  d->_parent;
}

bool object::set_parent(object * obj)
{
    // assert(this != obj);
    if (this == obj)
        return false;

    std::lock_guard<std::mutex> child_locker(d->_mtx);

    // remove previous parent
    object * parent = d->_parent;
    if (parent) {
        std::lock_guard<std::mutex> parent_locker(parent->d->_mtx);

        parent->d->children_array()[ d->_parent_index ] = nullptr;
        --parent->d->_nrChildren;
    }

    d->_parent = nullptr;
    if (!obj)
        return true;

    // select new parent
    parent = obj;
    std::lock_guard<std::mutex> parent_locker(parent->d->_mtx);

    if (parent->d->_children_vec == nullptr) {
        // parent->d->_children_vec = new ew::core::container::simple_array<object *>();
        parent->d->_children_vec = new std::vector<object *>();

        //    parent->d->_children_vec->reserve(2000000);
    }

    size_t sz = parent->d->children_array().size();

    // cerr << " BEGIN this = " << this << "{ \n";
    // cerr << " parent  = " << parent << "\n";
    // cerr << " sz = " << sz << "\n";
    // cerr << " parent->d->_nrChildren "  << parent->d->_nrChildren << "\n" ;

    // insert child O(n) , scan empty slots
    if (parent->d->_nrChildren != sz) {

        for (size_t i = 0; i < sz; i++) {

            if (parent->d->children_array()[i])
                continue;

            parent->d->children_array()[i] = this;
            d->_parent_index = i;
            d->_parent = parent;
            ++parent->d->_nrChildren;


            sz = parent->d->children_array().size();

            // cerr << " insert child in slot " << i << "\n";
            // cerr << " new sz = " << sz << "\n";
            // cerr << " new parent->d->_nrChildren "  << parent->d->_nrChildren << "\n" ;
            // cerr << " END } \n";

            return true ;
        }

        // cerr << "no slot found\n";
    }

    parent->d->children_array().push_back(this);
    d->_parent_index = sz;
    d->_parent = parent;
    ++parent->d->_nrChildren;

    sz = parent->d->children_array().size();

    //  cerr << "d->_parent_index == sz == " << sz << "\n";

    // cerr << " new sz = " << sz << "\n";
    // cerr << " new parent->d->_nrChildren "  << parent->d->_nrChildren << "\n" ;
    // cerr << " END } \n";

    return true;
}

bool object::has_children() const
{
    return (d->_nrChildren != 0);
}

size_t  object::number_of_children() const
{
    return d->_nrChildren;
}

object * object::get_child(u64 nr) const
{
    std::lock_guard<std::mutex> me(d->_mtx);          // needed ?

    if (has_children() == false) {
        return nullptr;
    }

    size_t sz = d->children_array().size();
    if (nr >= sz)
        return nullptr;

    return d->children_array()[(size_t)nr];
}

bool object::add_child(object * child)
{
    return child->set_parent(this);
}

bool object::remove_child(object * child)
{
    if (!child || (child->d->_parent != this))
        return false;

    return child->set_parent(nullptr);
}

bool object::is_parent_of(object * obj)
{
    std::lock_guard<std::mutex> me(d->_mtx);         // needed ?
    std::lock_guard<std::mutex> child(obj->d->_mtx); // needed ?

    return (this == obj->d->_parent);
}

bool object::is_child_of(object * obj)
{
    std::lock_guard<std::mutex> me(d->_mtx);          // needed ?
    std::lock_guard<std::mutex> parent(obj->d->_mtx); // needed ?

    return (d->_parent == obj);
}

bool object::is_sibling_of(object * obj)
{
    std::lock_guard<std::mutex> me(d->_mtx);            // needed ?
    std::lock_guard<std::mutex> sibling(obj->d->_mtx);  // needed ?

    return (this->d->_parent == obj->d->_parent);
}

}
} // ! ew::core
