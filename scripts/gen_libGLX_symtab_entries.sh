#!/bin/sh

cd $(dirname $0)

root_dir='..'

HEADER="${root_dir}/implementation/platform/unix/graphics/gui/x11_libGLX.hpp"
SYMTAB="${root_dir}/implementation/platform/unix/graphics/gui/x11_libGLX_sym_tab_entries.hpp"
WRAPPER="${root_dir}/implementation/platform/unix/graphics/gui/x11_libGLX_wrappers.hpp"

./gen_lib_wrapper.sh "${HEADER}" "${SYMTAB}" "${WRAPPER}"
exit $?
