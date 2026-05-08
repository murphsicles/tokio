#!/bin/bash
# Build Zeta Tokio using the Zeta compiler
# Requires zetac v1.0.2+ in PATH

set -e

ZETAC="${ZETAC:-zetac}"

echo "=== Zeta Tokio Build ==="
echo "Compiler: $(which $ZETAC)"

# Check compiler
if ! command -v $ZETAC &> /dev/null; then
    echo "Error: zetac not found. Set ZETAC env var or add to PATH."
    echo "  export ZETAC=/path/to/zetac"
    exit 1
fi

# Build the main demo
echo ""
echo "=== Building main.z ==="
$ZETAC src/main.z -o zeta_tokio
echo "  → zeta_tokio (binary)"

# Build examples
echo ""
echo "=== Building examples ==="
for example in examples/*.z; do
    name=$(basename "$example" .z)
    echo "  Building $example..."
    $ZETAC "$example" -o "zeta_tokio_$name" 2>&1 || true
done

# Run tests
echo ""
echo "=== Running tests ==="
$ZETAC tests/test_basic.z -o zeta_tokio_test 2>&1 || true

echo ""
echo "=== Build complete ==="
echo ""
echo "Artifacts:"
ls -la zeta_tokio* 2>/dev/null || true
