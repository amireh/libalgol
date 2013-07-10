#!/usr/bin/env bash

ROOT="."
if [ $# -gt 0 ]; then
  ROOT=$1
fi

OLDDIR=`pwd`
OUTDIR=../wrappers/

cd $ROOT/packages

function generate() {
  swig -Wall -python -c++ -threads -I$ROOT/packages -o $OUTDIR/${2}_wrap.cxx $3 ${1}.i
}

echo "@ Generating SWIG Python wrappers"
generate libalgol algol_py
echo "@ Done!"

cd $OLDDIR