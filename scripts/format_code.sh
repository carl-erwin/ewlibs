#!/bin/sh

cd $(dirname $0)

root_dir='..'

ASTYLE_OPTS="--style=linux -T8"

EXT="
cc
cpp
hh
hpp
h
"

for ext in ${EXT}
do
    find ${root_dir} -type f -iname "*.${ext}" -exec astyle ${ASTYLE_OPTS} {} \;
    find ${root_dir} -type f -iname "*.${ext}.orig" | xargs rm -f
done

# (re)generate wrappers
./gen_libGL_symtab_entries.sh
./gen_libGLU_symtab_entries.sh
./gen_libX11_symtab_entries.sh
