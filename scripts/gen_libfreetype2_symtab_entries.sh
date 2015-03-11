#!/bin/sh

cd $(dirname $0)

root_dir='..'

HEADER="${root_dir}/implementation/platform/unix/graphics/font/libfreetype2.hpp"
SYMTAB="${root_dir}/implementation/platform/unix/graphics/font/libfreetype2_sym_tab_entries.hpp"
WRAPPER="${root_dir}/implementation/platform/unix/graphics/font/libfreetype2_wrappers.hpp"

./gen_lib_wrapper.sh "${HEADER}" "${SYMTAB}" "${WRAPPER}"
exit $?
