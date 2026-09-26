#!/usr/bin/env bash
#
# Vendor the Particle MS/TP port and its BACnet Stack dependencies into a
# Particle extended-layout application.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STACK_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
PROJECT_DIR="${1:-}"
LIB_DIR="${PROJECT_DIR}/lib/bacnet-stack"
DEST="${LIB_DIR}/src"

if [[ -z "${PROJECT_DIR}" || ! -f "${PROJECT_DIR}/project.properties" ]]; then
    echo "usage: $0 <particle-project-directory>" >&2
    exit 2
fi

rm -rf "${LIB_DIR}"
mkdir -p "${DEST}"

# Copy every public/internal header and generated data include, then remove
# implementation files so only the curated source manifest is compiled.
cp -R "${STACK_ROOT}/src/bacnet" "${DEST}/bacnet"
while IFS= read -r implementation; do
    rm -f "${implementation}"
done < <(find "${DEST}/bacnet" -type f \( -name '*.c' -o -name '*.cpp' \))

# Preserve the upstream defaults before replacing bacnet/config.h with the
# Particle configuration wrapper.
cp "${STACK_ROOT}/src/bacnet/config.h" \
    "${DEST}/bacnet/config-upstream.h"
cp "${SCRIPT_DIR}/config/bacnet-config.h" "${DEST}/bacnet-config.h"
cp "${SCRIPT_DIR}/config/bacnet/config.h" "${DEST}/bacnet/config.h"

while IFS= read -r source; do
    [[ -z "${source}" || "${source}" == \#* ]] && continue
    mkdir -p "${DEST}/$(dirname "${source}")"
    cp "${STACK_ROOT}/src/${source}" "${DEST}/${source}"
done < "${SCRIPT_DIR}/sources.txt"

cp "${SCRIPT_DIR}/bacnet.c" "${SCRIPT_DIR}/bacnet.h" \
    "${SCRIPT_DIR}/dlenv.c" "${SCRIPT_DIR}/dlenv.h" \
    "${SCRIPT_DIR}/hardware.h" \
    "${SCRIPT_DIR}/mstimer-init.cpp" "${SCRIPT_DIR}/mstimer-init.h" \
    "${SCRIPT_DIR}/rs485.cpp" "${SCRIPT_DIR}/rs485.h" "${DEST}/"
cp "${STACK_ROOT}/src/bacnet-mstp-config.h" "${DEST}/bacnet-mstp-config.h"

cat > "${LIB_DIR}/library.properties" <<'EOF'
name=bacnet-stack
version=1.0.0
author=Steve Karg and contributors
license=MIT
sentence=BACnet MS/TP client port for Particle P2 and B5 SoM
url=https://github.com/bacnet-stack/bacnet-stack
repository=https://github.com/bacnet-stack/bacnet-stack.git
architectures=*
EOF

echo "Particle BACnet library synchronized to ${LIB_DIR}"
