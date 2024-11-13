#!/bin/bash

clang_arg="${1:--i}"

while read -r src_file
do
	clang-format -Werror "$clang_arg" "$src_file" || exit 1
done < <(find ./src \( -name '*.h' -o -name '*.c' -o -name '*.cc' \))
