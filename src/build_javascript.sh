#!/bin/sh
emcc -o test.html -s EXPORTED_FUNCTIONS="['_main']" *.c
