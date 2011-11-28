#!/bin/bash

FIREBIRD_ROOT=../firebird/src
FIREBIRD_SOURCE=${FIREBIRD_ROOT}/include
FIREBIRD_RELEASE=/opt/fb3cs
TEST=firetest

LC_ALL=C g++ -Wall -I${FIREBIRD_SOURCE} -c -fPIC ${TEST}.cpp
LC_ALL=C g++ -Wall -I${FIREBIRD_SOURCE} -o ${TEST} -fPIC ${TEST}.o ${FIREBIRD_RELEASE}/lib/libfbclient.so

if [[ $? -eq 0 ]]; then
  ./$TEST
fi
