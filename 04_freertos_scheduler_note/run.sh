#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IDF_EXPORT="${IDF_PATH:-$HOME/esp/esp-idf}/export.sh"
PORT="${PORT:-/dev/ttyUSB0}"

cd "$SCRIPT_DIR"
. "$IDF_EXPORT"

idf.py build
idf.py -p "$PORT" flash monitor
