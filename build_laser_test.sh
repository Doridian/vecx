#!/usr/bin/env bash
set -xeuo pipefail

set -e
set -u

SDK_DIR=helios_dac/sdk/cpp

buildc() {
    g++ $1.c -o "$(basename "$1").o" -c -Werror
}
buildcpp() {
    g++ $1.cpp -o "$(basename "$1").o" -c -Werror
}

buildc graphics
buildc laser
buildc laser_test
buildcpp $SDK_DIR/HeliosDac
buildcpp $SDK_DIR/shared_library/HeliosDacAPI
buildcpp $SDK_DIR/idn/idn
buildcpp $SDK_DIR/idn/idnServerList
buildcpp $SDK_DIR/idn/plt-posix
g++ -lusb-1.0 -lm -o laser_test idn.o idnServerList.o plt-posix.o HeliosDacAPI.o HeliosDac.o laser_test.o graphics.o laser.o -Werror

rm *.o

exec ./laser_test
