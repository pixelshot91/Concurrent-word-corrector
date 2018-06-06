#!/bin/bash

IFS=$'\n'
for f in `git ls-files | grep -E '*.cpp|*.hpp'`; do
	echo Dry running clang-format on "$f"...
	diff "$f" <(clang-format -style=file "$f")
done
