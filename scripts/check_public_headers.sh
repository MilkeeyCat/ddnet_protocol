#!/bin/bash

errors=0

for header in ./include/ddnet_protocol/*.h
do
	if ! grep -q '^extern "C" {' "$header"
	then
		printf 'Error: header is missing extern C annotation %s\n' "$header" 1>&2
		errors="$((errors+1))"
	fi
done

if [ "$errors" != 0 ]
then
	printf 'Error: public header check failed with %d errors\n' "$errors" 1>&2
	exit 1
fi
