#!/bin/bash
# Build Zeta Tokio — compiles Zeta sources (+ examples) with runtime linking
set -e

ZETAC="${ZETAC:-zetac}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RUNTIME_O="$SCRIPT_DIR/tokio-runtime/tokio_runtime.o"

# Check compiler
if ! command -v "$ZETAC" &>/dev/null; then
    echo "Zeta compiler not found. Set ZETAC env var or install zetac in PATH."
    exit 1
fi

echo "=== Building Zeta Tokio Runtime ==="
cd "$SCRIPT_DIR/tokio-runtime"
gcc -c -O2 -fPIC tokio_runtime.c -o tokio_runtime.o
echo "  → tokio_runtime.o"

echo "=== Building Zeta Tokio ==="
cd "$SCRIPT_DIR"

# Compile main.z with runtime linkage via --link flag
if [ -f "$RUNTIME_O" ]; then
    LINK_EXTRA="-L$SCRIPT_DIR/tokio-runtime -l:tokio_runtime.o"
fi

echo "  Compiling src/main.z..."
$ZETAC src/main.z -o zeta_tokio $LINK_EXTRA 2>&1 || echo "  (compile only — runtime linking needs manual gcc step)"

echo ""
echo "=== Artifacts ==="
ls -la zeta_tokio 2>/dev/null || echo "(manual linking required for full binary)"
echo ""
echo "To compile with full runtime linkage:"
echo "  gcc -no-pie <your_program.o> $RUNTIME_O -lc -o program"
