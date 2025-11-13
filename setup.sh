#!/bin/bash
# Complete setup script for Training Matrix
# This script does everything in one go

set -e

echo "╔══════════════════════════════════════════════════╗"
echo "║   Training Matrix - Complete Setup              ║"
echo "╚══════════════════════════════════════════════════╝"
echo ""

# 1. Create .env file if it doesn't exist
if [ ! -f .env ]; then
  echo "📝 Creating .env file..."
  cp .env.example .env
  echo "✅ .env file created"
else
  echo "✅ .env file already exists"
fi

# 2. Install root dependencies
echo ""
echo "📦 Installing root dependencies..."
npm install --silent

# 3. Install server dependencies
echo ""
echo "📦 Installing server dependencies..."
cd server
npm install --silent
cd ..

# 4. Run database setup
echo ""
echo "🗄️  Setting up database..."
cd server
node setup-database.js

echo ""
echo "╔══════════════════════════════════════════════════╗"
echo "║   ✅ COMPLETE SETUP FINISHED!                    ║"
echo "╚══════════════════════════════════════════════════╝"
echo ""
echo "To start the application:"
echo ""
echo "Terminal 1 (Backend):"
echo "  cd server"
echo "  node index.cjs"
echo ""
echo "Terminal 2 (Frontend):"
echo "  npm run dev"
echo ""
echo "Then open: http://localhost:5173"
echo "Login with: admin / admin123"
echo ""
