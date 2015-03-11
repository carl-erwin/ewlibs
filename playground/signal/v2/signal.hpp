#include <list>
#include <iostream>

#define DBG_METHOD()    std::cerr << __FUNCTION__ << " : this = " << this << "\n";

namespace playground
{

class Object {};     // ew::core::objects::IObject
class DummyType { }; // ew::core::types

namespace SIGNALS
{

#define __EW_SIGNALS__
# include "signals_forward_decl.hpp"
# include "slots.hpp"
# include "signals.hpp"
#undef __EW_SIGNALS__

} // namespace SIGNALS

// helpers

#define EMIT(name) (sig_##name.emit)

#define DECL_SIGNAL(CLASS__, name__, ...) ::playground::SIGNALS::Signal<__VA_ARGS__> sig_##name__

#define SIGNAL(name) sig_##name

#define SLOT(name) slot_##name

#define DECL_SLOT(CLASS__, name, ...) ::playground::SIGNALS::Slot<CLASS__, __VA_ARGS__> slot_##name

#define  connect_signal(CLASS0__, SIG__,  a__, CLASS1__, SLT__, b__)  \
    (b__)->SLOT(SLT__).real_slot = &CLASS1__::SLT__;      \
    (a__)->SIGNAL(SIG__).connect(static_cast<Object *>(b__), &(b__)->SLOT(SLT__))

}

// ---------------------------
