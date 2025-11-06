# Database Setup Guide

This guide will help you migrate from localStorage to SQL database and connect to your work server.

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Database Setup](#database-setup)
4. [Configuration](#configuration)
5. [Running the Application](#running-the-application)
6. [Connecting to Work Server](#connecting-to-work-server)
7. [Migrating Existing Data](#migrating-existing-data)
8. [Troubleshooting](#troubleshooting)

## Overview

The Training Matrix application now supports SQL databases (PostgreSQL, MySQL, SQL Server) instead of localStorage. This provides:
- ✅ Centralized data storage
- ✅ Multi-user access
- ✅ Better security
- ✅ Data backup and recovery
- ✅ Remote access capabilities

## Prerequisites

1. **Node.js** (v14 or higher)
2. **PostgreSQL** (v12 or higher) - Or MySQL/SQL Server
3. **Database server** at your work (if using remote database)

## Database Setup

### Option 1: Local PostgreSQL Setup

1. **Install PostgreSQL**
   ```bash
   # macOS (using Homebrew)
   brew install postgresql
   brew services start postgresql

   # Ubuntu/Debian
   sudo apt-get install postgresql postgresql-contrib
   sudo service postgresql start

   # Windows
   # Download installer from https://www.postgresql.org/download/windows/
   ```

2. **Create Database**
   ```bash
   # Connect to PostgreSQL
   psql -U postgres

   # Create database
   CREATE DATABASE training_matrix;

   # Create user (optional)
   CREATE USER training_user WITH PASSWORD 'your_secure_password';
   GRANT ALL PRIVILEGES ON DATABASE training_matrix TO training_user;

   # Exit
   \q
   ```

3. **Run Schema Setup**
   ```bash
   # Import the schema
   psql -U postgres -d training_matrix -f server/db/schema.sql
   ```

### Option 2: MySQL Setup

If you're using MySQL instead, modify the connection configuration:

1. Install MySQL
2. Create database:
   ```sql
   CREATE DATABASE training_matrix;
   ```
3. Update `server/config/database.js` to use `mysql2` instead of `pg`
4. Install MySQL driver: `npm install mysql2`

## Configuration

### 1. Create Environment File

Copy the example environment file:
```bash
cp .env.example .env
```

### 2. Configure Connection String

Edit `.env` file with your database credentials:

#### For Local Database:
```env
DB_CONNECTION_STRING=postgresql://postgres:password@localhost:5432/training_matrix
PORT=3001
NODE_ENV=development
FRONTEND_URL=http://localhost:5173
```

#### For Work Server Database:
```env
DB_CONNECTION_STRING=postgresql://username:password@work-server.company.com:5432/training_matrix
DB_SSL=true
PORT=3001
NODE_ENV=production
FRONTEND_URL=http://your-frontend-url.com
```

### Connection String Format

```
postgresql://[username]:[password]@[host]:[port]/[database]
```

**Example formats:**
- Local: `postgresql://postgres:admin123@localhost:5432/training_matrix`
- Remote: `postgresql://training_user:SecurePass123@192.168.1.100:5432/training_matrix`
- Cloud: `postgresql://user:pass@db.company.com:5432/training_matrix?sslmode=require`

### Alternative: Individual Parameters

Instead of connection string, you can use individual parameters in `.env`:

```env
DB_HOST=your-server.company.com
DB_PORT=5432
DB_NAME=training_matrix
DB_USER=your_username
DB_PASSWORD=your_password
DB_SSL=true
```

## Running the Application

### 1. Install Dependencies

```bash
# Install root dependencies
npm install

# Install server dependencies
cd server
npm install
cd ..
```

### 2. Start Backend Server

```bash
# Production mode
npm run server

# Development mode (with auto-restart)
cd server
npm run dev
```

The server will start on `http://localhost:3001`

### 3. Start Frontend

In a new terminal:
```bash
npm run dev
```

The frontend will start on `http://localhost:5173`

### 4. Verify Connection

Visit `http://localhost:3001/api/health` to check server status.

You should see:
```json
{
  "status": "ok",
  "database": "connected",
  "timestamp": "2024-11-06T..."
}
```

## Connecting to Work Server

### Prerequisites
1. Database server IP address or hostname
2. Database name
3. Username and password
4. Port number (usually 5432 for PostgreSQL)
5. Network access (firewall rules configured)

### Steps

1. **Get Database Credentials from IT**
   - Server address: `db-server.company.com` or `192.168.1.100`
   - Port: `5432`
   - Database name: `training_matrix`
   - Username: `your_username`
   - Password: `your_password`

2. **Test Connection**
   ```bash
   # Test with psql command line tool
   psql "postgresql://username:password@server:5432/database"
   ```

3. **Update .env File**
   ```env
   DB_CONNECTION_STRING=postgresql://username:password@db-server.company.com:5432/training_matrix
   DB_SSL=true
   NODE_ENV=production
   ```

4. **Configure Firewall** (if needed)
   - Ensure port 5432 is open on work server
   - Your IP address must be whitelisted
   - Contact IT department if connection fails

5. **Setup Database Schema**
   ```bash
   # Connect to work server and run schema
   psql "postgresql://username:password@work-server:5432/training_matrix" -f server/db/schema.sql
   ```

### SSL Configuration

For production servers, enable SSL:

**In .env:**
```env
DB_SSL=true
```

**For self-signed certificates:**
```env
DB_SSL=true
DB_SSL_REJECT_UNAUTHORIZED=false
```

## Migrating Existing Data

If you have data in localStorage that you want to migrate:

### Method 1: Export/Import via UI

1. Before switching to database, export your data:
   - Go to Data Management tab
   - Click "Export to JSON"
   - Save the file

2. After setting up database:
   - Start the new server
   - Use import API or manually insert data

### Method 2: Migration Script

Create a migration script (`server/migrate-localstorage.js`):

```javascript
const fs = require('fs');
const db = require('./db/connection');

// Load localStorage data
const localData = JSON.parse(fs.readFileSync('path/to/exported-data.json', 'utf8'));

// Migrate engineers
for (const engineer of localData.engineers) {
  await db.query(
    'INSERT INTO engineers (id, name, shift) VALUES ($1, $2, $3)',
    [engineer.id, engineer.name, engineer.shift]
  );
}

// ... migrate other data
```

Run: `node server/migrate-localstorage.js`

## Troubleshooting

### Connection Refused

**Error:** `ECONNREFUSED`

**Solutions:**
1. Check database is running: `sudo service postgresql status`
2. Verify host/port in connection string
3. Check firewall rules
4. Ensure PostgreSQL accepts remote connections

Edit `/etc/postgresql/*/main/postgresql.conf`:
```
listen_addresses = '*'
```

Edit `/etc/postgresql/*/main/pg_hba.conf`:
```
host    all             all             0.0.0.0/0            md5
```

### Authentication Failed

**Error:** `password authentication failed`

**Solutions:**
1. Verify username/password in `.env`
2. Check user has database permissions:
   ```sql
   GRANT ALL PRIVILEGES ON DATABASE training_matrix TO your_user;
   ```

### SSL Required

**Error:** `no pg_hba.conf entry for host, SSL off`

**Solution:**
Add to connection string:
```
?sslmode=require
```

Or in `.env`:
```
DB_SSL=true
```

### Port Already in Use

**Error:** `Port 3001 is already in use`

**Solution:**
Change port in `.env`:
```
PORT=3002
```

### Cannot Find Database

**Error:** `database "training_matrix" does not exist`

**Solution:**
Create database first:
```bash
createdb training_matrix
```

## Database Backup

### Automated Backup

```bash
# PostgreSQL backup
pg_dump -U username -h hostname training_matrix > backup_$(date +%Y%m%d).sql

# Restore
psql -U username -h hostname training_matrix < backup_20241106.sql
```

### Schedule Regular Backups

Add to crontab (`crontab -e`):
```
0 2 * * * pg_dump -U username training_matrix > /backups/training_matrix_$(date +\%Y\%m\%d).sql
```

## Security Best Practices

1. **Never commit `.env` file** - It contains sensitive credentials
2. **Use strong passwords** - Minimum 12 characters
3. **Enable SSL** for production servers
4. **Restrict database access** - Whitelist specific IP addresses
5. **Regular backups** - Automate daily backups
6. **Update dependencies** - Keep packages up to date

## Support

If you encounter issues:

1. Check server logs: `server/logs/` (if configured)
2. Test database connection: `http://localhost:3001/api/health`
3. Verify environment variables: `console.log(process.env.DB_CONNECTION_STRING)`
4. Contact your IT department for server access issues

## MySQL/SQL Server Variations

### For MySQL

1. Install MySQL driver:
   ```bash
   npm install mysql2
   ```

2. Update `server/db/connection.js` to use `mysql2`

3. Connection string format:
   ```
   mysql://username:password@host:3306/database
   ```

### For SQL Server

1. Install SQL Server driver:
   ```bash
   npm install mssql
   ```

2. Update connection code for `mssql`

3. Connection string format:
   ```
   mssql://username:password@host:1433/database
   ```

## Next Steps

- [ ] Set up database on work server
- [ ] Configure firewall rules
- [ ] Import schema (`schema.sql`)
- [ ] Update `.env` with work server credentials
- [ ] Test connection
- [ ] Migrate existing data
- [ ] Set up automated backups
- [ ] Configure SSL for production

---

**Need help?** Contact your system administrator or database team.
