#!/bin/bash

set -e
set -u

gcc graphics.c -o graphics.o -c -Werror
gcc laser.c -o laser.o -c -Werror
gcc laser_test.c -o laser_test.o -c -Werror
gcc -lm -o laser_test laser_test.o graphics.o laser.o -Werror

rm *.o

./laser_test
