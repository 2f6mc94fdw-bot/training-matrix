#!/bin/bash
# Training Matrix - Mac Setup Script
# This script sets up the SQL Server database for your Training Matrix application

set -e  # Exit on any error

echo "======================================"
echo "Training Matrix - Database Setup"
echo "======================================"
echo ""

# Step 1: Check if Docker is running
echo "Step 1: Checking Docker..."
if ! docker ps > /dev/null 2>&1; then
    echo "❌ Docker is not running. Please start Docker Desktop and try again."
    exit 1
fi
echo "✅ Docker is running"
echo ""

# Step 2: Check for existing SQL Server container
echo "Step 2: Checking for SQL Server container..."
CONTAINER_ID=$(docker ps -a --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}" | head -n 1)

if [ -z "$CONTAINER_ID" ]; then
    echo "No existing SQL Server container found."
    echo "Creating new SQL Server container..."
    docker run -d \
        --name training-matrix-sql \
        -e "ACCEPT_EULA=Y" \
        -e "SA_PASSWORD=YourStrong@Passw0rd" \
        -p 1433:1433 \
        mcr.microsoft.com/mssql/server:2022-latest
    echo "✅ SQL Server container created"
    sleep 10  # Wait for SQL Server to start
else
    CONTAINER_NAME=$(docker ps -a --filter "id=$CONTAINER_ID" --format "{{.Names}}")
    CONTAINER_STATUS=$(docker ps -a --filter "id=$CONTAINER_ID" --format "{{.Status}}")
    echo "Found existing container: $CONTAINER_NAME"
    echo "Status: $CONTAINER_STATUS"

    # Start container if not running
    if ! docker ps --filter "id=$CONTAINER_ID" --format "{{.ID}}" | grep -q .; then
        echo "Starting container..."
        docker start $CONTAINER_ID
        sleep 5
    fi
    echo "✅ SQL Server container is running"
fi
echo ""

# Step 3: Create .env file
echo "Step 3: Creating .env file..."
if [ -f .env ]; then
    echo "⚠️  .env file already exists. Backing up to .env.backup"
    cp .env .env.backup
fi

cat > .env << 'EOF'
# Backend API Server Configuration
PORT=3001
NODE_ENV=development

# Frontend URL (for CORS)
FRONTEND_URL=http://localhost:3000

# Database Configuration (SQL Server)
DB_SERVER=localhost
DB_NAME=training_matrix
DB_USER=sa
DB_PASSWORD=YourStrong@Passw0rd

# Database Connection Options
DB_ENCRYPT=true
DB_TRUST_SERVER_CERTIFICATE=true
DB_CONNECTION_TIMEOUT=30000
DB_REQUEST_TIMEOUT=30000

# Database Pool Settings
DB_POOL_MAX=10
DB_POOL_MIN=0
DB_POOL_IDLE_TIMEOUT=30000
EOF

echo "✅ .env file created"
echo ""

# Step 4: Create database
echo "Step 4: Creating database..."
docker exec -i $(docker ps --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}" | head -n 1) \
    /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P "YourStrong@Passw0rd" \
    -Q "IF NOT EXISTS (SELECT * FROM sys.databases WHERE name = 'training_matrix') CREATE DATABASE training_matrix;"

if [ $? -eq 0 ]; then
    echo "✅ Database created (or already exists)"
else
    echo "❌ Failed to create database. Please check SQL Server password."
    exit 1
fi
echo ""

# Step 5: Run schema
echo "Step 5: Running database schema..."
if [ -f database/schema.sql ]; then
    docker exec -i $(docker ps --filter "ancestor=mcr.microsoft.com/mssql/server" --format "{{.ID}}" | head -n 1) \
        /opt/mssql-tools/bin/sqlcmd -S localhost -U sa -P "YourStrong@Passw0rd" -d training_matrix \
        < database/schema.sql
    echo "✅ Database schema created"
else
    echo "⚠️  database/schema.sql not found. Skipping schema creation."
fi
echo ""

# Step 6: Install dependencies
echo "Step 6: Installing npm dependencies..."
if [ -f package.json ]; then
    npm install
    echo "✅ Dependencies installed"
else
    echo "⚠️  package.json not found. Are you in the correct directory?"
fi
echo ""

echo "======================================"
echo "✅ Setup Complete!"
echo "======================================"
echo ""
echo "Next steps:"
echo "1. Start the application:"
echo "   npm start"
echo ""
echo "2. Open your browser to:"
echo "   http://localhost:3000"
echo ""
echo "3. Login with:"
echo "   Username: admin"
echo "   Password: admin123"
echo ""
echo "Database Information:"
echo "   Server: localhost:1433"
echo "   Database: training_matrix"
echo "   Username: sa"
echo "   Password: YourStrong@Passw0rd"
echo ""
