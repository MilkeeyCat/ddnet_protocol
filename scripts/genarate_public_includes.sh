#!/bin/bash

set -euo pipefail

### parse args

arg_is_dry=0
for arg in "$@"
do
	if [ "$arg" = '--dry-run' ]
	then
		arg_is_dry=1
	else
		printf "[!] Error: unknown argument '%s'\n" "$arg" 1>&2
	fi
done

### copy files

public_headers=(common errors token chunk packet packer huffman)

for header in "${public_headers[@]}"
do
	cp src/"$header".h include/ddnet_protocol
done

### cmake

if ! TMP_FILE="$(mktemp ./scripts/edit_in_place.tmp.XXXXX)"
then
	printf '[!] Error: mktemp failed' 1>&2
	exit 1
fi

cleanup() {
	[[ -f "$TMP_FILE" ]] && rm "$TMP_FILE"
}

trap cleanup EXIT

build_cmake_string() {
	local last_header_index
	local header
	local i=0
	last_header_index="${#public_headers[@]}"
	last_header_index=$((last_header_index-1))
	for header in "${public_headers[@]}"
	do
		[[ "$i" = 0 ]] && printf '"'
		printf 'include/ddnet_protocol/%s.h' "$header"
		if [[ "$i" = "$last_header_index" ]]
		then
			printf '"'
		else
			printf ';\\\n'
		fi
		i=$((i+1))
	done
}

insert_at() {
	# insert_at [from_pattern] [to_pattern] [new content] [filename]
	local from_pattern="$1"
	local to_pattern="$2"
	local content="$3"
	local filename="$4"
	local from_ln
	local to_ln
	if ! grep -q "$from_pattern" "$filename"; then
		echo "Error: pattern '$from_pattern' not found in '$filename'"
		exit 1
	fi
	from_ln="$(grep -n "$from_pattern" "$filename" | cut -d':' -f1 | head -n1)"
	from_ln="$((from_ln + 1))"
	to_ln="$(tail -n +"$from_ln" "$filename" | grep -n "$to_pattern" | cut -d':' -f1 | head -n1)"
	to_ln="$((from_ln + to_ln - 1))"

	{
		head -n "$((from_ln - 1))" "$filename"
		printf '%b\n' "$content"
		tail -n +"$to_ln" "$filename"
	} > "$TMP_FILE"
	if [ "$arg_is_dry" == "1" ]; then
		if [ "$(cat "$TMP_FILE")" != "$(cat "$filename")" ]; then
			echo "Error: missing header generation for $filename"
			echo "       run ./scripts/generate_public_includes.sh"
			git diff --no-index --color "$TMP_FILE" "$filename"
			exit 1
		fi
	else
		mv "$TMP_FILE" "$filename"
	fi
}

# build_cmake_string

insert_at 'ddnet_protocol PROPERTIES PUBLIC_HEADER' ')' "$(build_cmake_string)" CMakeLists.txt

# grep -n 'set_target_properties(ddnet_protocol PROPERTIES PUBLIC_HEADER' CMakeLists.txt | cut -d':' -f1
