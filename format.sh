#!/bin/bash

IFS=$'\n'
for f in `git ls-files | grep -E '*.cpp|*.hpp'`; do
	echo Running clang-format on "$f"...
	clang-format -i -style=file "$f"
done
