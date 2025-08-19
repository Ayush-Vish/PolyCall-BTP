#!/bin/bash
set -e

RUNTIME_DIR="$HOME/.polycall/runtime"
VISIBLE_DIR="$HOME/PolyCall-BTP"

./clean.sh
echo "[*] Setting up PolyCall repo..."
rm -rf "$VISIBLE_DIR"
git clone https://github.com/Ayush-Vish/PolyCall-BTP "$VISIBLE_DIR" >/dev/null 2>&1

echo "[*] Installing polycallcli wrapper..."
sudo tee /usr/local/bin/polycallcli >/dev/null <<EOF
#!/bin/bash

export PATH="$RUNTIME_DIR/build/cli/metacallcli:\$PATH"

exec metacallcli "\$@"
EOF

sudo chmod +x /usr/local/bin/polycallcli

echo "[*] PolyCall CLI installed successfully!"
echo "Use it with: polycallcli <file> [args...]"
