#!/usr/bin/env bash

# List of C headers
c_headers=(
	assert.h complex.h ctype.h errno.h fenv.h float.h inttypes.h iso646.h limits.h locale.h math.h setjmp.h signal.h stdarg.h stdbool.h stddef.h stdint.h stdio.h stdlib.h string.h tgmath.h time.h wchar.h wctype.h
)

# Create regex dynamically from the array to match any C header
c_headers_regex=$(printf "%s" "${c_headers[*]}" | tr ' ' '|')

# Find all C source and header files
files=$(find ./src -type f \( -name '*.c' -o -name '*.h' \) ! -path "./include/ddnet_protocol/common.h")

error_found=0

# Check each source file for stdlib headers
for file in $files; do
	if grep -E "#include\s+<($c_headers_regex)>" "$file" > /dev/null; then
		for ((i = 0; i < ${#c_headers[@]}; i++)); do
			if grep -E "^\s*#include\s+<${c_headers[i]}>" "$file" > /dev/null; then
				echo "Error: '$file' includes stdlib header '${c_headers[i]}'. Include 'ddnet_protocol/common.h' instead."
			fi
		done
		error_found=1
	fi
done

if [ $error_found -eq 1 ]; then
	exit 1
fi

echo "Success: No standard C headers are used."
