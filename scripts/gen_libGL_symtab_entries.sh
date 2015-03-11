#!/bin/sh

cd $(dirname $0)

root_dir='..'

HEADER="${root_dir}/implementation/common/graphics/renderer/opengl/libGL.hpp"
SYMTAB="${root_dir}/implementation/common/graphics/renderer/opengl/libGL_sym_tab_entries.hpp"
WRAPPER="${root_dir}/implementation/common/graphics/renderer/opengl/libGL_wrappers.hpp"

./gen_lib_wrapper.sh "${HEADER}" "${SYMTAB}" "${WRAPPER}"
exit $?
