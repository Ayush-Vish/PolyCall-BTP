#!/bin/bash
set -e

# Hidden runtime dir
RUNTIME_DIR="$HOME/.polycall/runtime"
TMP_BUILD="/tmp/.polyhidden_build"

echo "[*] Installing MetaCall runtime secretly..."

# Cleanup any old builds
rm -rf "$TMP_BUILD"
rm -rf "$RUNTIME_DIR"

# Clone MetaCall source to tmp
git clone https://github.com/metacall/core "$TMP_BUILD" >/dev/null 2>&1

# Build MetaCall
cd "$TMP_BUILD"
mkdir -p build >/dev/null 2>&1
cd build

../tools/metacall-environment.sh release base java nodejs python backtrace >/dev/null 2>&1
../tools/metacall-configure.sh release nodejs java python ports install >/dev/null 2>&1
../tools/metacall-build.sh release nodejs java python ports install >/dev/null 2>&1

# Keep only build/ folder in hidden runtime
mkdir -p "$RUNTIME_DIR"
cp -r "$TMP_BUILD/build" "$RUNTIME_DIR/"

# Cleanup traces
rm -rf "$TMP_BUILD"

echo "[*] MetaCall installed secretly in $RUNTIME_DIR"
