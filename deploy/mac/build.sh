#!/bin/sh
# SPDX-License-Identifier: MIT
set -eu

if [ "$(uname -s)" != Darwin ] || [ "$(uname -m)" != arm64 ]; then
  echo "Run this script natively on an Apple Silicon Mac" >&2
  exit 1
fi

source_dir=$(CDPATH= cd "$(dirname "$0")/../.." && pwd)
build_dir="$source_dir/build/macos"
qt_prefix=$(brew --prefix qt)
openssl_prefix=$(brew --prefix openssl@3)
export PATH="$qt_prefix/bin:$PATH"

cmake -S "$source_dir" -B "$build_dir" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_PREFIX_PATH="$qt_prefix;$openssl_prefix" \
  -DOPENSSL_ROOT_DIR="$openssl_prefix" \
  -DBUILD_TESTS=OFF \
  -DBUILD_INSTALLER=ON \
  -DBUILD_OSX_BUNDLE=ON \
  "$@"
cmake --build "$build_dir" --parallel "$(sysctl -n hw.ncpu)"
cpack --config "$build_dir/CPackConfig.cmake" -B "$build_dir" -G DragNDrop

echo "DMG: $build_dir/deskflow-*-macos-arm64.dmg"
