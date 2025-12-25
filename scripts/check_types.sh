#!/bin/bash

error=0

for banned_type in int unsigned signed short long double;
do
	if grep -rEn --exclude='common.h' "([^a-z0-9_/]|^)${banned_type}[^a-z0-9(]" src/ | grep -vE '^[a-z_/]+\.(h|c):[0-9]+:// '
	then
		echo "Error: illegal raw type found ^"
		error=1
	fi
done

exit "$error"

