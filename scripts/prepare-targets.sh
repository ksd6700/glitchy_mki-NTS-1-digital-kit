#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDK_DIR="${LOGUE_SDK_DIR:-${ROOT_DIR}/sdk/logue-sdk}"

ALL_TARGETS=(nutekt-digital minilogue-xd prologue nts-1_mkii microkorg2)
TARGETS=("${@:-all}")
if [[ "${TARGETS[0]}" == "all" ]]; then
  TARGETS=("${ALL_TARGETS[@]}")
fi

if [[ ! -d "${SDK_DIR}/platform" ]]; then
  echo "logue SDK not found at ${SDK_DIR}; run scripts/fetch-sdk.sh first." >&2
  exit 1
fi

for target in "${TARGETS[@]}"; do
  case "${target}" in
    nutekt-digital|minilogue-xd|prologue|nts-1_mkii|microkorg2) ;;
    *)
      echo "Unknown target: ${target}" >&2
      exit 2
      ;;
  esac

  template="${SDK_DIR}/platform/${target}/dummy-modfx"
  destination="${SDK_DIR}/platform/${target}/glitchy"
  if [[ ! -d "${destination}" ]]; then
    cp -R "${template}" "${destination}"
  fi

  cp "${ROOT_DIR}/dsp/glitch_core.hpp" "${destination}/glitch_core.hpp"

  case "${target}" in
    nutekt-digital|minilogue-xd|prologue)
      cp "${ROOT_DIR}/overlays/v1/glitchy.cpp" "${destination}/glitchy.cpp"
      cp "${ROOT_DIR}/overlays/v1/project.mk" "${destination}/project.mk"
      cp "${ROOT_DIR}/overlays/${target}/manifest.json" \
         "${destination}/manifest.json"
      ;;
    nts-1_mkii|microkorg2)
      cp "${ROOT_DIR}/overlays/${target}/config.mk" "${destination}/config.mk"
      cp "${ROOT_DIR}/overlays/${target}/header.c" "${destination}/header.c"
      cp "${ROOT_DIR}/overlays/${target}/modfx.h" "${destination}/modfx.h"
      ;;
  esac

  echo "Prepared ${target}/glitchy"
done
