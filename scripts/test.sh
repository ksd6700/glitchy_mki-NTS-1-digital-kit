#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="${ROOT_DIR}/build/tests"
CXX="${CXX:-c++}"

mkdir -p "${TEST_DIR}"
"${CXX}" -std=c++11 -O2 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/dsp" \
  "${ROOT_DIR}/tests/glitch_core_test.cpp" \
  -o "${TEST_DIR}/glitch_core_test"
"${TEST_DIR}/glitch_core_test"
