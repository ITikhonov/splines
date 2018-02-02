#!/bin/bash

set -ex

gcc -std=gnu99 -g -Wall -Werror -o splines splines.c -lm

