#!/bin/bash

# Script to create macOS .icns icon from PNG
# Usage: ./create_icns.sh

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SOURCE_PNG="$SCRIPT_DIR/images/aptitude-icon.png"
ICONSET_DIR="$SCRIPT_DIR/aptitude.iconset"
OUTPUT_ICNS="$SCRIPT_DIR/aptitude.icns"

echo "Creating macOS .icns icon from PNG..."

# Check if source PNG exists
if [ ! -f "$SOURCE_PNG" ]; then
    echo "Error: $SOURCE_PNG not found!"
    echo "Please ensure aptitude-icon.png exists in resources/images/"
    exit 1
fi

# Create iconset directory
mkdir -p "$ICONSET_DIR"

# Generate all required icon sizes using sips (macOS built-in tool)
echo "Generating icon sizes..."

sips -z 16 16     "$SOURCE_PNG" --out "$ICONSET_DIR/icon_16x16.png" > /dev/null
sips -z 32 32     "$SOURCE_PNG" --out "$ICONSET_DIR/icon_16x16@2x.png" > /dev/null
sips -z 32 32     "$SOURCE_PNG" --out "$ICONSET_DIR/icon_32x32.png" > /dev/null
sips -z 64 64     "$SOURCE_PNG" --out "$ICONSET_DIR/icon_32x32@2x.png" > /dev/null
sips -z 128 128   "$SOURCE_PNG" --out "$ICONSET_DIR/icon_128x128.png" > /dev/null
sips -z 256 256   "$SOURCE_PNG" --out "$ICONSET_DIR/icon_128x128@2x.png" > /dev/null
sips -z 256 256   "$SOURCE_PNG" --out "$ICONSET_DIR/icon_256x256.png" > /dev/null
sips -z 512 512   "$SOURCE_PNG" --out "$ICONSET_DIR/icon_256x256@2x.png" > /dev/null
sips -z 512 512   "$SOURCE_PNG" --out "$ICONSET_DIR/icon_512x512.png" > /dev/null
sips -z 1024 1024 "$SOURCE_PNG" --out "$ICONSET_DIR/icon_512x512@2x.png" > /dev/null

# Convert iconset to icns using iconutil (macOS built-in tool)
echo "Creating .icns file..."
iconutil -c icns "$ICONSET_DIR" -o "$OUTPUT_ICNS"

# Clean up iconset directory
rm -rf "$ICONSET_DIR"

echo "✓ Success! Created $OUTPUT_ICNS"
echo ""
echo "The icon will be automatically included in the app bundle during the next build."
echo "Run: cmake --build build"
