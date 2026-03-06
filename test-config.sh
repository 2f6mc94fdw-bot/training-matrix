#!/bin/bash

# Check where Aptitude stores its config file on macOS
echo "Checking for Aptitude config file..."
echo ""

CONFIG_PATH="$HOME/Library/Application Support/Aptitude/config.json"

if [ -f "$CONFIG_PATH" ]; then
    echo "✅ Config file found at: $CONFIG_PATH"
    echo ""
    echo "Contents:"
    cat "$CONFIG_PATH" | python3 -m json.tool 2>/dev/null || cat "$CONFIG_PATH"
else
    echo "❌ Config file not found at: $CONFIG_PATH"
    echo ""
    echo "Searching for any Aptitude config files..."
    find ~/Library/Application\ Support -name "config.json" -o -name "*aptitude*" -o -name "*Aptitude*" 2>/dev/null
fi
