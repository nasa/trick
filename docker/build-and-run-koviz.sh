#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR=${SCRIPT_DIR}/..
cd ${ROOT_DIR}

if [ ! -f "${ROOT_DIR}/bin/koviz" ]; then
    echo "koviz is not built, building now."
    qmake
    make
fi

./bin/koviz $@
