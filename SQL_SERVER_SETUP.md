# SQL Server Setup Guide

This guide will help you connect the Training Matrix application directly to your SQL Server database.

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Quick Start](#quick-start)
4. [Detailed Setup](#detailed-setup)
5. [Connecting to Work Server](#connecting-to-work-server)
6. [Troubleshooting](#troubleshooting)

---

## Overview

The Training Matrix application now stores data directly in **Microsoft SQL Server** instead of browser localStorage. This enables:

✅ **Centralized data storage** - All users access the same data
✅ **Multi-user access** - Multiple people can use the system simultaneously
✅ **Remote connectivity** - Connect to your work server from anywhere
✅ **Data persistence** - No more lost data when clearing browser cache
✅ **Better security** - Database-level authentication and encryption

---

## Prerequisites

1. **Microsoft SQL Server** (2016 or later)
   - SQL Server Express (free) for local/small teams
   - SQL Server Standard/Enterprise for production
   - Azure SQL Database also supported

2. **Node.js** (v14 or higher)

3. **Database permissions**
   - Ability to create databases
   - Read/write access to the database

---

## Quick Start

### 1. Install Dependencies

```bash
npm install
```

### 2. Configure Connection

```bash
# Copy the example config
cp database/config.example.js database/config.js

# Edit database/config.js with your connection details
```

Example `database/config.js`:

```javascript
const config = {
  server: 'localhost',             // Or your server name/IP
  database: 'training_matrix',
  user: 'sa',                      // Your SQL Server username
  password: 'YourPassword123',     // Your SQL Server password

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};

module.exports = config;
```

### 3. Create Database

Open **SQL Server Management Studio (SSMS)** or use `sqlcmd`:

```sql
CREATE DATABASE training_matrix;
GO
```

### 4. Run Schema Script

In SSMS:
1. Open `database/schema.sql`
2. Make sure `training_matrix` database is selected
3. Click **Execute** (or press F5)

Or use command line:
```bash
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/schema.sql
```

### 5. Test Connection

```bash
node database/test-connection.js
```

You should see:
```
✅ Connection successful!
✅ Found 11 tables
✅ All tests passed!
```

### 6. Start Application

```bash
npm run dev
```

Open http://localhost:5173 and login with:
- **Username**: `admin`
- **Password**: `admin123`

⚠️ **Change the default password immediately after first login!**

---

## Detailed Setup

### Step 1: SQL Server Installation

If you don't have SQL Server installed:

#### Windows
1. Download [SQL Server Express](https://www.microsoft.com/en-us/sql-server/sql-server-downloads) (free)
2. Run the installer
3. Choose "Basic" installation
4. Note the instance name (usually `SQLEXPRESS`)
5. Download [SQL Server Management Studio (SSMS)](https://docs.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms)

#### Linux/macOS
Use Docker:
```bash
docker run -e "ACCEPT_EULA=Y" -e "SA_PASSWORD=YourPassword123" \
   -p 1433:1433 --name sql_server \
   -d mcr.microsoft.com/mssql/server:2022-latest
```

### Step 2: Enable SQL Server for Remote Connections

1. Open **SQL Server Configuration Manager**
2. Expand **SQL Server Network Configuration**
3. Click **Protocols for [YOUR_INSTANCE]**
4. Enable **TCP/IP** protocol
5. Right-click TCP/IP → Properties → IP Addresses tab
6. Scroll to **IPAll** section
7. Set **TCP Port** to `1433`
8. Restart SQL Server service

### Step 3: Configure Firewall

#### Windows Firewall:
```powershell
# Allow SQL Server port
netsh advfirewall firewall add rule name="SQL Server" dir=in action=allow protocol=TCP localport=1433
```

#### Linux (Ubuntu):
```bash
sudo ufw allow 1433/tcp
```

### Step 4: Create Database User (Recommended)

Instead of using `sa` account, create a dedicated user:

```sql
-- Create login
CREATE LOGIN training_user WITH PASSWORD = 'SecurePassword123!';
GO

-- Create database user
USE training_matrix;
GO

CREATE USER training_user FOR LOGIN training_user;
GO

-- Grant permissions
ALTER ROLE db_owner ADD MEMBER training_user;
GO
```

Then update `database/config.js`:
```javascript
user: 'training_user',
password: 'SecurePassword123!'
```

---

## Connecting to Work Server

### Getting Server Information

Ask your IT department for:

- **Server name or IP**: e.g., `SERVERNAME`, `192.168.1.100`, or `server.company.com`
- **Instance name** (if applicable): e.g., `SQLEXPRESS`, `INSTANCE01`
- **Port**: Usually `1433` (default)
- **Database name**: `training_matrix`
- **Username** and **Password**
- **Authentication type**: SQL Server or Windows Authentication

### Configuration Examples

#### SQL Server Authentication (most common):

```javascript
// database/config.js
const config = {
  server: 'SERVERNAME',          // or '192.168.1.100'
  database: 'training_matrix',
  user: 'your_username',
  password: 'your_password',

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};
```

#### Named Instance:

```javascript
const config = {
  server: 'SERVERNAME\\SQLEXPRESS',  // Notice the double backslash
  database: 'training_matrix',
  user: 'your_username',
  password: 'your_password',

  options: {
    encrypt: true,
    trustServerCertificate: true,
    instanceName: 'SQLEXPRESS'      // Add instance name
  }
};
```

#### Windows Authentication (if on same domain):

```javascript
const config = {
  server: 'SERVERNAME',
  database: 'training_matrix',

  options: {
    encrypt: true,
    trustedConnection: true   // Use Windows credentials
  }
};
```

#### Specific Port:

```javascript
const config = {
  server: '192.168.1.100',
  database: 'training_matrix',
  user: 'your_username',
  password: 'your_password',
  port: 1433,                 // Explicit port

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};
```

### Setting Up Database on Work Server

1. **Connect via SSMS** to your work server

2. **Create the database:**
   ```sql
   CREATE DATABASE training_matrix;
   GO
   ```

3. **Run the schema script:**
   - In SSMS, open `database/schema.sql`
   - Select the `training_matrix` database
   - Execute the script (F5)

4. **Verify tables were created:**
   ```sql
   USE training_matrix;
   GO

   SELECT name FROM sys.tables ORDER BY name;
   GO
   ```

5. **Test from your machine:**
   ```bash
   node database/test-connection.js
   ```

---

## Troubleshooting

### ❌ "Connection Refused" or "Cannot Connect"

**Possible causes:**

1. **SQL Server not running**
   - Windows: Check Services → SQL Server (MSSQLSERVER)
   - Restart if needed

2. **TCP/IP not enabled**
   - Open SQL Server Configuration Manager
   - Enable TCP/IP protocol
   - Restart SQL Server

3. **Firewall blocking**
   - Add firewall rule for port 1433
   - Windows Firewall → Allow an app → Add SQL Server

4. **Wrong server name/IP**
   - Use `ipconfig` to find your IP address
   - Try `localhost`, `127.0.0.1`, or `(local)`
   - For named instances, use `SERVERNAME\\INSTANCE`

### ❌ "Login Failed for User"

**Possible causes:**

1. **Wrong username/password**
   - Double-check credentials
   - No spaces before/after

2. **SQL Server Authentication not enabled**
   - SSMS → Right-click server → Properties → Security
   - Enable "SQL Server and Windows Authentication mode"
   - Restart SQL Server

3. **User doesn't have permissions**
   ```sql
   USE training_matrix;
   ALTER ROLE db_owner ADD MEMBER your_username;
   ```

### ❌ "Database 'training_matrix' does not exist"

Create the database first:

```sql
CREATE DATABASE training_matrix;
GO
```

### ❌ "Cannot open database requested in login"

The database exists but user doesn't have access:

```sql
USE training_matrix;
CREATE USER your_username FOR LOGIN your_username;
ALTER ROLE db_datareader ADD MEMBER your_username;
ALTER ROLE db_datawriter ADD MEMBER your_username;
```

### ❌ Tables Not Found

Run the schema script:

```bash
# Via SSMS - Open and execute database/schema.sql

# Or via command line:
sqlcmd -S SERVER -U username -P password -d training_matrix -i database/schema.sql
```

### ❌ Named Instance Connection Issues

For named instances (e.g., `SERVERNAME\\SQLEXPRESS`):

1. **SQL Server Browser must be running**
   - Check Services → SQL Server Browser
   - Set to Automatic and Start

2. **UDP Port 1434 must be open** (used by Browser service)

3. **Try explicit port instead:**
   ```javascript
   server: 'SERVERNAME,1433'  // Note: comma, not backslash
   ```

### ❌ "Self-signed certificate" Errors

In `database/config.js`:
```javascript
options: {
  encrypt: true,
  trustServerCertificate: true  // Allow self-signed certificates
}
```

For production with proper SSL:
```javascript
options: {
  encrypt: true,
  trustServerCertificate: false
}
```

---

## Testing Connection String

Use this script to quickly test different connection strings:

```javascript
// test-quick.js
const sql = require('mssql');

const config = {
  server: 'YOUR_SERVER',
  database: 'training_matrix',
  user: 'YOUR_USER',
  password: 'YOUR_PASSWORD',
  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};

sql.connect(config).then(pool => {
  console.log('✅ Connected!');
  return pool.request().query('SELECT @@VERSION as version');
}).then(result => {
  console.log('Version:', result.recordset[0].version);
}).catch(err => {
  console.error('❌ Error:', err.message);
}).finally(() => {
  sql.close();
});
```

Run: `node test-quick.js`

---

## Common Connection Strings

```javascript
// Local SQL Server Express
server: 'localhost\\SQLEXPRESS'

// Local SQL Server default instance
server: 'localhost'

// By computer name
server: 'MYCOMPUTER\\SQLEXPRESS'

// By IP address
server: '192.168.1.100'

// With port
server: '192.168.1.100,1433'

// Domain server
server: 'SERVER.domain.local'

// Azure SQL
server: 'yourserver.database.windows.net'
```

---

## Security Best Practices

1. **Never commit config.js**
   - It's already in `.gitignore`
   - Use `config.example.js` as template

2. **Use strong passwords**
   - Minimum 12 characters
   - Mix of letters, numbers, symbols

3. **Create dedicated user**
   - Don't use `sa` account
   - Grant only needed permissions

4. **Enable encryption**
   - `encrypt: true` in options
   - Use proper SSL certificates in production

5. **Firewall rules**
   - Only allow specific IP addresses if possible
   - Close port 1433 when not needed

6. **Regular backups**
   ```sql
   BACKUP DATABASE training_matrix
   TO DISK = 'C:\\Backups\\training_matrix.bak'
   WITH FORMAT, COMPRESSION;
   ```

7. **Change default admin password**
   - Login to app immediately
   - Change admin password from admin123

---

## Database Maintenance

### Backup

```sql
-- Full backup
BACKUP DATABASE training_matrix
TO DISK = 'C:\\Backups\\training_matrix_full.bak'
WITH COMPRESSION;

-- Differential backup
BACKUP DATABASE training_matrix
TO DISK = 'C:\\Backups\\training_matrix_diff.bak'
WITH DIFFERENTIAL, COMPRESSION;
```

### Restore

```sql
RESTORE DATABASE training_matrix
FROM DISK = 'C:\\Backups\\training_matrix_full.bak'
WITH REPLACE;
```

### Check Database Size

```sql
USE training_matrix;
GO

EXEC sp_spaceused;
GO
```

---

## Next Steps

- [ ] Install SQL Server
- [ ] Create database: `CREATE DATABASE training_matrix`
- [ ] Copy `config.example.js` to `config.js`
- [ ] Update connection details in `config.js`
- [ ] Run schema script: `database/schema.sql`
- [ ] Test connection: `node database/test-connection.js`
- [ ] Start app: `npm run dev`
- [ ] Login and change default password!

---

## Support

### Useful Commands

```bash
# Test connection
node database/test-connection.js

# Start application
npm run dev

# Install dependencies
npm install
```

### Useful SQL Queries

```sql
-- List all tables
SELECT name FROM sys.tables;

-- Count records
SELECT
  (SELECT COUNT(*) FROM users) as Users,
  (SELECT COUNT(*) FROM engineers) as Engineers,
  (SELECT COUNT(*) FROM production_areas) as Areas;

-- Reset admin password to 'admin123'
UPDATE users
SET password = '$2a$10$XQK9X.xjKZv4PqGqxdpN0OYjQz5Z8rCqH9VB0KpXKNH0qUqKjKqKq'
WHERE username = 'admin';
```

---

**Need more help?** Contact your IT department or database administrator.
