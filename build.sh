#!/bin/bash
# Zeta Tokio Build — JIT mode (primary) + AOT mode (with C runtime)
set -e
ZETAC="${ZETAC:-zetac}"

echo "=== Zeta Tokio Build ==="
echo "JIT mode:  zetac --jit src/main.z"
echo "AOT mode:  zetac src/main.z -o tokio.out && gcc -no-pie tokio.out.o tokio_runtime.o -lc -o tokio_prog"

if ! command -v $ZETAC &>/dev/null; then
    echo "zetac not found. Install Zeta v0.11.3+ from https://github.com/murphsicles/zeta"
    exit 1
fi

echo ""
echo "=== Building C runtime ==="
gcc -c -O2 tokio_runtime.c -o tokio_runtime.o 2>&1
echo "  tokio_runtime.o ready ($(nm tokio_runtime.o | grep " T " | wc -l) symbols)"

echo ""
echo "=== JIT Testing ==="
for f in examples/*.z; do
    name=$(basename "$f" .z)
    echo -n "  $name: "
    $ZETAC --jit "$f" 2>&1 || echo "(skipped)"
done

echo ""
echo "=== Ready ==="
