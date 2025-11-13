#!/bin/bash
# Script to copy all Phase 1 fixes to your local project
# Usage: ./copy-fixes.sh /path/to/your/training-matrix-main-2

set -e

if [ -z "$1" ]; then
  echo "❌ Error: Please provide destination path"
  echo "Usage: ./copy-fixes.sh /path/to/your/training-matrix-main-2"
  exit 1
fi

DEST="$1"
SOURCE="/home/user/training-matrix"

echo "🔄 Copying Phase 1 fixes..."
echo "   Source: $SOURCE"
echo "   Destination: $DEST"
echo ""

# Check if destination exists
if [ ! -d "$DEST" ]; then
  echo "❌ Error: Destination directory does not exist: $DEST"
  exit 1
fi

# Create backup
BACKUP_DIR="$DEST/backup-$(date +%Y%m%d-%H%M%S)"
echo "📦 Creating backup at: $BACKUP_DIR"
mkdir -p "$BACKUP_DIR"

# Backup files that will be modified
echo "   Backing up existing files..."
[ -f "$DEST/.env.example" ] && cp "$DEST/.env.example" "$BACKUP_DIR/"
[ -f "$DEST/database/config.cjs" ] && cp "$DEST/database/config.cjs" "$BACKUP_DIR/"
[ -f "$DEST/database/connection.cjs" ] && cp "$DEST/database/connection.cjs" "$BACKUP_DIR/"
[ -f "$DEST/server/package.json" ] && cp "$DEST/server/package.json" "$BACKUP_DIR/"
[ -f "$DEST/server/index.cjs" ] && cp "$DEST/server/index.cjs" "$BACKUP_DIR/"
[ -f "$DEST/server/setup-database.js" ] && cp "$DEST/server/setup-database.js" "$BACKUP_DIR/"
[ -f "$DEST/src/App.jsx" ] && cp "$DEST/src/App.jsx" "$BACKUP_DIR/"
[ -f "$DEST/src/components/Login.jsx" ] && cp "$DEST/src/components/Login.jsx" "$BACKUP_DIR/"
[ -f "$DEST/src/hooks/useData.js" ] && cp "$DEST/src/hooks/useData.js" "$BACKUP_DIR/"

echo ""
echo "📝 Copying new files..."

# Copy new files
echo "   ✓ server/validation.cjs"
cp "$SOURCE/server/validation.cjs" "$DEST/server/"

echo ""
echo "📝 Copying modified files..."

# Copy configuration files
echo "   ✓ .env.example"
cp "$SOURCE/.env.example" "$DEST/"

echo "   ✓ database/config.cjs"
cp "$SOURCE/database/config.cjs" "$DEST/database/"

echo "   ✓ database/connection.cjs"
cp "$SOURCE/database/connection.cjs" "$DEST/database/"

# Copy server files
echo "   ✓ server/package.json"
cp "$SOURCE/server/package.json" "$DEST/server/"

echo "   ✓ server/index.cjs"
cp "$SOURCE/server/index.cjs" "$DEST/server/"

echo "   ✓ server/setup-database.js"
cp "$SOURCE/server/setup-database.js" "$DEST/server/"

# Copy frontend files
echo "   ✓ src/App.jsx"
cp "$SOURCE/src/App.jsx" "$DEST/src/"

echo "   ✓ src/components/Login.jsx"
cp "$SOURCE/src/components/Login.jsx" "$DEST/src/components/"

echo "   ✓ src/hooks/useData.js"
cp "$SOURCE/src/hooks/useData.js" "$DEST/src/hooks/"

echo ""
echo "🔐 Setting up environment file..."

# Create .env if it doesn't exist
if [ ! -f "$DEST/.env" ]; then
  echo "   Creating .env from .env.example"
  cp "$DEST/.env.example" "$DEST/.env"
  echo "   ⚠️  Remember to edit .env and set your DB_PASSWORD"
else
  echo "   ℹ️  .env already exists, not overwriting"
  echo "   ⚠️  Check .env.example for new settings you may need to add"
fi

echo ""
echo "✅ All files copied successfully!"
echo ""
echo "📋 Next steps:"
echo "   1. Edit $DEST/.env and set DB_PASSWORD"
echo "   2. cd $DEST/server && npm install"
echo "   3. cd $DEST && npm install"
echo "   4. Start your servers and test"
echo ""
echo "💾 Backup saved at: $BACKUP_DIR"
echo "   (You can delete this once you verify everything works)"
