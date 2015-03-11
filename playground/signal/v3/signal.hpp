#pragma once

#include <list>
#include <iostream>

namespace playground
{

//#define DBG_METHOD()    ::std::cerr << __FUNCTION__ << " : this = " << this << "\n";
#define DBG_METHOD()    ;


class Object {};     // ew::core::objects::IObject
class DummyType { }; // ew::core::types

// namespace ew {
// namespace Core {
namespace SIGNALS
{

#define __EW_SIGNALS__
# include "signals_forward_decl.hpp"
# include "signals.hpp"
# include "slots.hpp"
#undef __EW_SIGNALS__

} // namespace SIGNALS
// } // namespace Core
// } // namespace ew {


// helpers

#define EMIT(name__) (sig_##name__.emit)

#define DECL_SIGNAL(CLASS__, name__, ...) ::playground::SIGNALS::Signal<__VA_ARGS__> sig_##name__

#define SIGNAL(name__) sig_##name__

#define SLOT(name__) slot_##name__

#define DECL_SLOT(CLASS__, name__, ...) ::playground::SIGNALS::Slot<CLASS__, __VA_ARGS__> slot_##name__


#define DECL_VOID_SIGNAL_SLOT(RET_TYPE__, CLASS__, name__)  \
    ::playground::SIGNALS::Slot<CLASS__, RET_TYPE__> slot_##name__;     \
    ::playground::SIGNALS::Signal<RET_TYPE__> sig_##name__;             \
    virtual RET_TYPE__ name__ ( )                           \

#define DECL_SIGNAL_SLOT(RET_TYPE__, CLASS__, name__, ...)           \
    ::playground::SIGNALS::Slot<CLASS__, RET_TYPE__, __VA_ARGS__> slot_##name__; \
    ::playground::SIGNALS::Signal<RET_TYPE__, __VA_ARGS__> sig_##name__;         \
    virtual RET_TYPE__ name__ ( __VA_ARGS__ )                        \


#define  connect_signal(CLASS0__, SIG__,  a__, CLASS1__, SLT__, b__)  \
    do {                \
        (b__)->SLOT(SLT__).setSlot( &CLASS1__::SLT__ ); \
        (b__)->SLOT(SLT__).add_signal(&((a__)->SIGNAL(SIG__))); \
        (a__)->SIGNAL(SIG__).connect(static_cast<Object *>(b__), &(b__)->SLOT(SLT__)); \
    } while (0)

}

// ---------------------------
