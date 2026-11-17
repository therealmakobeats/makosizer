#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${1:-${ROOT_DIR}/build}"
VERSION="${2:-1.0.0}"

PKG_ROOT="${ROOT_DIR}/dist/pkgroot"
PKG_OUT="${ROOT_DIR}/dist/Makosizer.pkg"

VST3="${BUILD_DIR}/Makosizer_artefacts/Release/VST3/Makosizer.vst3"
AU="${BUILD_DIR}/Makosizer_artefacts/Release/AU/Makosizer.component"

[[ -d "${VST3}" ]] || {
    echo "Missing VST3 bundle: ${VST3}" >&2
    exit 1
}

[[ -d "${AU}" ]] || {
    echo "Missing AU bundle: ${AU}" >&2
    exit 1
}

rm -rf "${PKG_ROOT}"

mkdir -p \
    "${PKG_ROOT}/Library/Audio/Plug-Ins/VST3" \
    "${PKG_ROOT}/Library/Audio/Plug-Ins/Components" \
    "${ROOT_DIR}/dist"

cp -R "${VST3}"
