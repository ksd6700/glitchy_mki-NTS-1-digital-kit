#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDK_DIR="${LOGUE_SDK_DIR:-${ROOT_DIR}/sdk/logue-sdk}"
BUILD_MODE="${BUILD_MODE:-native}"
ARTIFACT_DIR="${ARTIFACT_DIR:-${ROOT_DIR}/build/artifacts}"

ALL_TARGETS=(nutekt-digital minilogue-xd prologue nts-1_mkii microkorg2)
TARGETS=("${@:-all}")
if [[ "${TARGETS[0]}" == "all" ]]; then
  TARGETS=("${ALL_TARGETS[@]}")
fi

mkdir -p "${ARTIFACT_DIR}"
LOGUE_SDK_DIR="${SDK_DIR}" "${ROOT_DIR}/scripts/prepare-targets.sh" "${TARGETS[@]}"

if [[ "${BUILD_MODE}" == "docker" ]] && \
   ! docker image inspect xiashj/logue-sdk:latest >/dev/null 2>&1; then
  docker pull xiashj/logue-sdk:latest
fi

extension_for() {
  case "$1" in
    nutekt-digital) echo ntkdigunit ;;
    minilogue-xd) echo mnlgxdunit ;;
    prologue) echo prlgunit ;;
    nts-1_mkii) echo nts1mkiiunit ;;
    microkorg2) echo mk2unit ;;
  esac
}

for target in "${TARGETS[@]}"; do
  project_dir="${SDK_DIR}/platform/${target}/glitchy"
  if [[ "${BUILD_MODE}" == "docker" ]]; then
    "${SDK_DIR}/docker/run_cmd.sh" build "${target}/glitchy"
  else
    if [[ "${target}" == "microkorg2" ]]; then
      echo "microKORG2 requires the official Docker build environment." >&2
      echo "Run: BUILD_MODE=docker ./scripts/build.sh microkorg2" >&2
      exit 3
    fi
    make -C "${project_dir}" clean
    make -C "${project_dir}"
    make -C "${project_dir}" install
  fi

  extension="$(extension_for "${target}")"
  artifact="${project_dir}/glitchy.${extension}"
  if [[ ! -f "${artifact}" ]]; then
    echo "Expected artifact was not generated: ${artifact}" >&2
    exit 4
  fi
  cp "${artifact}" "${ARTIFACT_DIR}/"
  echo "Built ${ARTIFACT_DIR}/glitchy.${extension}"
done
