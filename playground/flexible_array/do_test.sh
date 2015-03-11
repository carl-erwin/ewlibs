#!/bin/sh

cd $(dirname $0)

# test_boostArray
# test_qtVector
# test_stdvector
# test_simpleArray
# test_flexArray

BIN_LIST="
test_all
"

for i in $(echo ${BIN_LIST})
do
 echo  running ./$i $1 $2 $3 $4
 taskset 1 ${TOOL_PREFIX} ./$i $1 $2 $3 $4
 [ -e perf.data ] && mv perf.data $i.perf.data

 echo --------------------------
done
