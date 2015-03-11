#pragma once

#include <assert.h>
#include <vector>

#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/container/flexible_array.hpp>

namespace ew
{
namespace core
{
namespace objects
{

using namespace ew::core::types;

/*
   TODO: remove this
     - update text codecs : the core codec mechanism should be byte based
        and a template could be used to expressed input iterators

        codec = codec_adapter<codec, flexible_array<byte>>
        codec = codec_adapter<codec, std::vector<byte>>
        etc ...

        use specialization for read ???

 */

typedef typename ew::core::container::flexible_array
<
ew::core::types::u8,
ew::core::container::page<ew::core::types::u8>
> buffer;

}
}
}

