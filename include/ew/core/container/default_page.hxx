#pragma once

#include <assert.h>

#include "ew/core/container/simple_array.hpp"
#include "ew/filesystem/file.hpp"

#include <iostream>

#define USE_MMAP_SYSCALL 1  /* else  read */

namespace ew
{
    namespace core
    {
        namespace container
        {


            /*
             * the ew::core::container::page class
             * augment the simple_array class with a virtual offset field
            */
            template <class T>
            class page : public simple_array<T>
            {
            protected:
                u64 _offset;
            private:
                // disabled for now
                page<T> & operator = (const page<T> &);
                page(const page<T> &);

            public:
                inline page()
                    : simple_array<T >::simple_array(0)
                    , _offset(0) {

                }

                inline page(u32 nr_items)
                    : simple_array<T>::simple_array(nr_items)
                    , _offset(0) {

                }

                virtual inline ~page() {
                }

                virtual inline   u64  offset() const {
                    return _offset;
                }

                virtual inline void set_offset(u64 offset) {
                    _offset = offset;
                }

                /*
                  -1 left
                  +1 right
                   0 found
                 */
                virtual inline int search_offset(const u64 off, u32 * pg_offset) const {

                    if (off < _offset)
                        return -1;

                    if (off >= (_offset + simple_array<T>::size())) {
                        return 1;
                    }

                    if (pg_offset) {
                        *pg_offset = off - this->_offset;
                    }

                    return 0;
                }

                virtual inline bool have_offset(const u64 off, u32 * pg_offset) const {
                    return search_offset(off, pg_offset) == 0;
                }

                inline u32  read(u32 pg_offset, T * items, u32 nr_items) {

                    if (pg_offset + nr_items > simple_array<T>::size())
                        nr_items = simple_array<T>::size() - pg_offset;

                    T * p = simple_array<T>::begin() + pg_offset;
                    if (! std::is_pod<T>()) {
                        for (u32 i = 0; i < nr_items; i++) {
                            new(&items[i]) T(p[ i ]);
                        }
                    } else {
                        memcpy(items, p, nr_items * sizeof(T));
                    }

                    return nr_items;
                }

                virtual inline bool copyOnWrite() { return true; }

                virtual inline bool map() {
                    assert(simple_array<T>::_ptr != 0);
                    return true;
                }

                virtual inline bool unmap() { return true; }

                virtual inline bool is_mapped() const {
                    return simple_array<T>::_ptr != 0;
                }

                virtual inline bool has_ref() const {
                    return simple_array<T>::size() != 0;
                }

                virtual inline u32 nr_ref() const {
                    return 0xFFFFFFFF;
                }

                virtual inline void  dump_page(const char * pgname = 0) const {
                    //  return;
#ifndef NDEBUG
                    if (pgname) {
                        std::cerr << "page[ " << pgname << "] : ";
                    }
                    std::cerr << "_ptr       = " << (void *)simple_array<T>::_ptr << " , ";
                    std::cerr << "skip       = " << simple_array<T>::skipped() << " , ";
                    std::cerr << "used       = " << simple_array<T>::size() << " , ";
                    std::cerr << "alloc      = " << simple_array<T>::allocated() << ", ";
                    std::cerr << "_is_mapped = " << is_mapped() << ", ";
                    std::cerr << "nr_ref     = " << nr_ref();
                    std::cerr << "\n";
#endif
                }                

            };

//

// TODO: rename in FileMappedpage ?
            template <class T>
            class memory_mapped_page : public page <T>
            {
            private:
                // methods
                // for now
                memory_mapped_page<T> & operator = (const memory_mapped_page<T> &);
                memory_mapped_page(const memory_mapped_page<T> &);
            protected:
                u64 _on_disk_offset;
                ew::filesystem::file * _file;
                u32 _nr_ref;

                typedef page<T> __page;

            public:
                inline memory_mapped_page(ew::filesystem::file * file, size_t size, u64 offset, u32 flags) {
                    _file             = file;
                    _on_disk_offset = offset;
                    _nr_ref = 0;

                    __page::_is_mapped = true;
                    __page::set_offset(offset);
                    __page::set_data(0, size, size, true); // remember projection params using ptr diff
                }

                inline memory_mapped_page(u32 nr = 0)
                    :
                    __page::page(nr) {
                    _file           = 0;
                    _on_disk_offset = 0;
                    _nr_ref = 0;
                    __page::_is_mapped = false;
                }

                inline u32 nr_ref() const {
                    return _nr_ref;
                }

                inline bool copyOnWrite() {

                    if (__page::_is_mapped == false) {
                        return true;
                    }

                    u32 _save_ref = _nr_ref;

                    T * out = 0;
                    u32 newsize = __page::size();

                    bool ret = map();
                    if (ret == true) {
                        out = static_cast<T *>(::operator new(newsize * sizeof(T)));
                        if (! std::is_pod<T>()) {
                            abort(); // TODO: use std::copy
                        } else {
                            memcpy(out, __page::_ptr, newsize);
                        }

                        // force unmap
                        _nr_ref = 1;
                        ret = unmap();
                        if (ret == true) {
                            // std::cerr << " set new data pointer to " << std::hex << (void *)out << std::dec << "\n";
                            // std::cerr << " restore nr_ref to " << _nr_ref << "\n";
                            __page::set_data(out, newsize, newsize, false);
                            __page::_is_mapped = false;
                            _file = 0;
                        } else {
                            /// ???
                        }
                        // restore previous refcounter
                        _nr_ref = _save_ref;
                    }

                    if (ret == false) {
                        delete out;
                        assert(0);
                    }

                    // TODO: check_invariants()
                    return ret;
                }

                inline bool map() {

                    if (!__page::_is_mapped) {
                        assert(__page::_ptr != 0);
                        ++_nr_ref;
                        //std::cerr << " page map() : _nr_ref = " << _nr_ref << " page start offset = " << __page::offset() << "\n";
                        return true;
                    }

                    if (_nr_ref > 0) {
                        ++_nr_ref;
                        return true;
                    }

                    bool ret;

                    assert(__page::_ptr == 0);

                    u64 page_size =  __page::size();
                    assert(page_size);

                    // TODO: update file->map Api
                    // acces is : read | write | exec  ? mode
                    // visibility is : private | shared

                    T * ptr = 0;




                    assert(_file);

#ifdef USE_MMAP_SYSCALL
                   ret = _file->map(_on_disk_offset, page_size, 0, 0, reinterpret_cast<void **>(&ptr));
#else
                    ptr = static_cast<T *>(::operator new(page_size));
                    u64 nrRead;
                    u64 sz = page_size;
                    ret = _file->set_position(_on_disk_offset);
                    ret = _file->read(ptr, sz, &nrRead);
#endif
                    if (ret == true) {
                        // std::cerr << " REAL page map() !!! _nr_ref = " << _nr_ref << " page start offset = " << __page::offset() << "\n";

                        assert(ptr);
                        __page::set_data(ptr, page_size, page_size, true);
                        ++_nr_ref;

                        // std::cerr << "  ptr = " << (void *)ptr << ",  len = " << page_size << "\n";
                        // std::cerr << "  ptr + page_size = " << (void *)(ptr + page_size) << "\n";
                    } else {
                        // log
                        std::cerr << "cannot map offset = " << _on_disk_offset << " + size = " << page_size << "\n";
                        assert(0);
                    }

                    return ret;
                }

                inline bool is_mapped() const {
                    return __page::_is_mapped;
                }

                inline bool unmap() {

                    if (! __page::_is_mapped) {

                        assert(__page::_ptr != 0);

                         // FIXME: must be atomic
                        if (_nr_ref > 0) {
                            --_nr_ref;
                        }

                        // std::cerr << " page map() : _nr_ref = " << _nr_ref << " page start offset = " << __page::offset() << "\n";

                        return true;
                    }

                    if (!_nr_ref) {
                        assert(__page::_ptr == 0);
                        return true;
                    }

                    assert(__page::_ptr != 0);
                    if (_nr_ref > 1) { // FIXME: must be atomic
                        --_nr_ref;
                        return true;
                    }

                    // _nr_ref == 1

                    T * real_buffer = __page::data() - __page::skipped();
#ifdef USE_MMAP_SYSCALL
                    assert(_file);
                    u64 real_size = __page::size() + __page::skipped();
                    bool ret = _file->unmap(real_buffer, real_size);
#else
                    bool ret = true;
                    ::operator delete((T *)real_buffer); // calls madvise ?
#endif

                    assert(ret == true);
                    if (ret == true) {
                        u64 page_size =  __page::size();
                        assert(page_size);
                        __page::set_data(0, page_size, page_size, true);

                        --_nr_ref;
                    } else {
                        // log
                    }

                    return ret;
                }

                virtual inline ~memory_mapped_page() {

                    if (_nr_ref != 0) {
                        std::cerr << "warning : page is referenced !!! _nr_ref = " << _nr_ref << ", offset = " << __page::offset() << "\n";
                    }
                }

            };


        } // ! namespace container
    } // ! namespace core
} // ! namespace ew
