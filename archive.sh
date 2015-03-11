#!/bin/sh

cd `dirname $0`

DATE=`date +%F-%H%M`

git archive  -v --format tar --prefix 'ew/' HEAD > ../ew-${DATE}.tar
lzma -z ../ew-${DATE}.tar

tar cvf ../ew-git-${DATE}.tar .git
lzma -z ../ew-git-${DATE}.tar
