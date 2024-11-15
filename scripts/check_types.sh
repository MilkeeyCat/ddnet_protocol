#!/bin/bash

error=0

for banned_type in int unsigned signed short long float double;
do
	if grep -rE --exclude='common.h' "([^a-z0-9_]|^)${banned_type}[^a-z0-9(]" src/;
	then
		echo "Error: illegal raw type found ^"
		error=1
	fi
done

exit "$error"

