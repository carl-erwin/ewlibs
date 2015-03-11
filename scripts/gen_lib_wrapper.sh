#!/bin/sh

cd $(dirname $0)


HEADER=$1
SYMTAB=$2
WRAPPER=$3

echo "HEADER  = ${HEADER}"
echo "SYMTAB  = ${SYMTAB}"
echo "WRAPPER = ${WRAPPER}"

(
 > ${WRAPPER}

# echo "#ifdef USE_EW_LIB_WRAPPER" > ${WRAPPER}
 cat ${HEADER} | grep DECLARE_wrapper | cut -d\, -f2 |
 while read line ;
  do
   echo "{ \"$line\", reinterpret_cast<void **>( &ew_${line} ) } ," ;
   echo "#define $line ew_$line" >>  ${WRAPPER}
  done
# echo "#endif /* ! USE_EW_LIB_WRAPPER */" >> ${WRAPPER}

) | tee ${SYMTAB}
