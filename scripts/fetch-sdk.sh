#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDK_DIR="${LOGUE_SDK_DIR:-${ROOT_DIR}/sdk/logue-sdk}"
SDK_REF="${LOGUE_SDK_REF:-c224d07adeb2a7ea04bbb9bc330d55f08305aca7}"

if [[ ! -d "${SDK_DIR}/.git" ]]; then
  mkdir -p "$(dirname "${SDK_DIR}")"
  git clone https://github.com/korginc/logue-sdk.git "${SDK_DIR}"
fi

if ! git -C "${SDK_DIR}" diff --quiet || \
   ! git -C "${SDK_DIR}" diff --cached --quiet; then
  echo "SDK checkout has local changes: ${SDK_DIR}" >&2
  echo "Use a clean checkout or set LOGUE_SDK_DIR to another directory." >&2
  exit 1
fi

if [[ "$(git -C "${SDK_DIR}" rev-parse HEAD)" != "${SDK_REF}" ]]; then
  git -C "${SDK_DIR}" fetch --depth 1 origin "${SDK_REF}"
  git -C "${SDK_DIR}" checkout --detach "${SDK_REF}"
fi

git -C "${SDK_DIR}" submodule update --init --recursive
echo "KORG logue SDK ready at ${SDK_DIR} ($(git -C "${SDK_DIR}" rev-parse --short HEAD))"
