#!/bin/bash
# Zeta Tokio Build — requires zetac v0.11.2+ in PATH
set -e

ZETAC="${ZETAC:-zetac}"
DIR="$(cd "$(dirname "$0")" && pwd)"

echo "=== Zeta Tokio Build ==="
echo "Compiler: $(which $ZETAC 2>/dev/null || echo $ZETAC)"

# Build the runtime object for AOT linking
echo ""
echo "=== Extracting runtime symbols ==="
if [ -f "$DIR/../zeta/target/release/deps/libzetac-"*.rlib ]; then
    LIB=$(ls -t "$DIR/../zeta/target/release/deps/libzetac-"*.rlib 2>/dev/null | head -1)
    if [ -n "$LIB" ]; then
        TMP=$(mktemp -d)
        cd "$TMP"
        ar x "$LIB"
        for f in *.o; do
            if nm "$f" 2>/dev/null | grep -q "reactor_create"; then
                cp "$f" "$DIR/tokio_runtime.o"
                echo "  → tokio_runtime.o extracted"
                break
            fi
        done
        cd / && rm -rf "$TMP"
    fi
fi

# JIT testing
echo ""
echo "=== JIT Testing ==="
for f in examples/*.z; do
    name=$(basename "$f" .z)
    echo -n "  $name: "
    $ZETAC --jit "$f" 2>&1 || echo "(JIT error, expected for some examples)"
done

echo ""
echo "=== Build Complete ==="
echo "To run:  zetac --jit src/main.z"
echo "For AOT: gcc -no-pie program.o tokio_runtime.o -lc -o program"
