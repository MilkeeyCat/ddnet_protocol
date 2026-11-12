#!/bin/bash

# https://github.com/MilkeeyCat/ddnet_protocol/pull/151#issuecomment-3694073249
#
# - if it's docs, write full sentences starting with uppercase first letter
# - if it's a code comment, start with lowercase and don't put . at the end of the comment

errors=0

is_valid_inline_comment() {
	local line="$1"
	local comment
	comment="$(printf '%s' "$line" | grep -o '// .*')"
	comment="${comment:3}"

	[[ "$comment" = TODO* ]] && return
	[[ "$comment" = NOLINT* ]] && return

	if [[ "$comment" =~ ^[A-Z] ]]
	then
		printf 'Error: code comments should start lowercase\n'
		return 1
	fi
	return 0
}

comments_lowercase() {
	local match
	local line
	while read -r match
	do
		line="$(printf '%s' "$match" | cut -d':' -f3-)"
		if ! is_valid_inline_comment "$line"
		then
			printf '%s\n' "$match"
			errors=$((errors+1))
		fi
	done < <(grep -nr --include='*.c' '// ' src)
}

comments_lowercase

if [ "$errors" -ne 0 ]
then
	echo "Error: found invalid comment style ^"
	exit 1
fi
