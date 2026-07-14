#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="${1:-v1.0.0}"
ARTIFACT_DIR="${ARTIFACT_DIR:-${ROOT_DIR}/build/artifacts}"
DIST_DIR="${DIST_DIR:-${ROOT_DIR}/dist/${VERSION}}"

EXPECTED=(
  glitchy.ntkdigunit
  glitchy.mnlgxdunit
  glitchy.prlgunit
  glitchy.nts1mkiiunit
  glitchy.mk2unit
)

mkdir -p "${DIST_DIR}"
for artifact in "${EXPECTED[@]}"; do
  if [[ ! -f "${ARTIFACT_DIR}/${artifact}" ]]; then
    echo "Missing ${ARTIFACT_DIR}/${artifact}" >&2
    exit 1
  fi
  cp "${ARTIFACT_DIR}/${artifact}" "${DIST_DIR}/${artifact}"
done

(
  cd "${DIST_DIR}"
  shasum -a 256 "${EXPECTED[@]}" > SHA256SUMS.txt
  zip -q -j "glitchy-${VERSION}-all-logue-synths.zip" "${EXPECTED[@]}" \
    SHA256SUMS.txt
)

echo "Packaged release files in ${DIST_DIR}"
