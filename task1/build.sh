#!/bin/sh
set -e

project_dir="$(dirname -- "$0")"
build_dir="$project_dir/build"

conan install . -pr="$project_dir/.conan/profile" --output-folder "$build_dir" --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
