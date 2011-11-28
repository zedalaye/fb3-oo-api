#!/bin/bash

FIREBIRD_SOURCE=../firebird/src/include
FIREBIRD_RELEASE=/opt/fb3cs
TEST=firetest

LC_ALL=C g++ -Wall -I${FIREBIRD_SOURCE} -o ${TEST} -fPIC ${TEST}.cpp ${FIREBIRD_RELEASE}/lib/libfbclient.so

if [[ $? -eq 0 ]]; then
  ./$TEST
fi
