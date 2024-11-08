#!/bin/sh

find . \
	-path './build' \
	-prune \
	-o \( -name '*.h' -o -name '*.c' -o -name '*.cc' \) \
	-exec clang-format -style=file -i {} \;
