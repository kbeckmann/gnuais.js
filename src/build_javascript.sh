#!/bin/sh
emcc -o ais.html -s EXPORTED_FUNCTIONS="['_main']" *.c
