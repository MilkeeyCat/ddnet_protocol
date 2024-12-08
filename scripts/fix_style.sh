#!/bin/bash

clang_arg="${1:--i}"

while read -r src_file
do
	clang-format -Werror "$clang_arg" "$src_file" || exit 1
done < <(find ./src ./include ./test \( -name '*.h' -o -name '*.c' -o -name '*.cc' \))

if grep -rnEo '0x[A-Z0-9]+' src/ | grep -E '0x.*[A-Z]'
then
	printf 'Error: detected upper case hex values ^ please use lowercase\n'
	exit 1
fi
