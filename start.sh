#!/bin/bash
# Simple startup script for Training Matrix
# Just run: ./start.sh

echo "🚀 Starting Training Matrix..."
echo ""

# Check if .env exists
if [ ! -f .env ]; then
  echo "⚠️  .env file not found. Creating from .env.example..."
  cp .env.example .env
  echo "✅ .env created"
  echo "   Make sure DB_PASSWORD is set correctly!"
  echo ""
fi

# Check if node_modules exists in root
if [ ! -d node_modules ]; then
  echo "📦 Installing root dependencies..."
  npm install --silent
  echo ""
fi

# Check if node_modules exists in server
if [ ! -d server/node_modules ]; then
  echo "📦 Installing server dependencies..."
  cd server
  npm install --silent
  cd ..
  echo ""
fi

echo "✅ All dependencies ready"
echo ""
echo "🌐 Starting servers..."
echo "   Backend: http://localhost:3001"
echo "   Frontend: http://localhost:5173"
echo ""
echo "💡 The browser will open automatically when ready"
echo "   Login with: admin / admin123"
echo ""
echo "⏹️  Press Ctrl+C to stop all servers"
echo ""

# Wait a moment then open browser
(sleep 5 && open http://localhost:5173) &

# Start both servers using npm start
npm start
