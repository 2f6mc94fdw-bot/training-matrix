#!/bin/bash
# Training Matrix - Easy Startup Script for Mac
# Double-click this file to start the application

# Get the directory where this script is located
cd "$(dirname "$0")"

echo "======================================"
echo "  Training Matrix - Starting..."
echo "======================================"
echo ""

# Check if SQL Server container is running
echo "Checking SQL Server..."
CONTAINER_RUNNING=$(docker ps --filter "name=sql-server-training" --format "{{.Names}}" 2>/dev/null)

if [ -z "$CONTAINER_RUNNING" ]; then
    echo "Starting SQL Server container..."
    docker start sql-server-training
    echo "Waiting for SQL Server to be ready..."
    sleep 5
else
    echo "✅ SQL Server is already running"
fi

echo ""
echo "======================================"
echo "  Starting Training Matrix Application"
echo "======================================"
echo ""
echo "Frontend will be available at:"
echo "  http://localhost:3000"
echo ""
echo "Login credentials:"
echo "  Username: admin"
echo "  Password: admin123"
echo ""
echo "Press Ctrl+C to stop the servers"
echo ""
echo "======================================"
echo ""

# Start the application
npm start
