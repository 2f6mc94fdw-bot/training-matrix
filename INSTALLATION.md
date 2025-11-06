# Training Matrix - Complete Installation Guide

Follow these steps to get your Training Matrix system up and running with SQL Server.

## Prerequisites

✅ **Node.js** (v14 or higher) - [Download](https://nodejs.org/)
✅ **Microsoft SQL Server** (2016+, Express, or Azure SQL)
✅ **SQL Server Management Studio (SSMS)** - [Download](https://docs.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms)

---

## Step 1: Install SQL Server (if not already installed)

### Windows:
Download [SQL Server Express](https://www.microsoft.com/en-us/sql-server/sql-server-downloads) (free) and install

### Linux/macOS (Docker):
```bash
docker run -e "ACCEPT_EULA=Y" -e "SA_PASSWORD=YourPassword123!" \
   -p 1433:1433 --name sql_server \
   -d mcr.microsoft.com/mssql/server:2022-latest
```

---

## Step 2: Create Database

Open **SQL Server Management Studio (SSMS)** or use `sqlcmd`:

```sql
CREATE DATABASE training_matrix;
GO
```

Or via command line:
```bash
sqlcmd -S localhost -U sa -P YourPassword -Q "CREATE DATABASE training_matrix"
```

---

## Step 3: Run Database Schema

### Option A: Using SSMS
1. Open SSMS and connect to your server
2. Open `database/schema.sql`
3. Make sure `training_matrix` database is selected
4. Execute (F5)

### Option B: Using sqlcmd
```bash
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/schema.sql
```

---

## Step 4: Configure Connection

```bash
# Copy the example config
cp database/config.example.js database/config.js

# Edit database/config.js with your text editor
```

Update with your server details:

```javascript
const config = {
  server: 'localhost',            // Your server name or IP
  database: 'training_matrix',
  user: 'sa',                     // Your username
  password: 'YourPassword123',    // Your password

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};
```

### For Work Server:
```javascript
const config = {
  server: '192.168.1.100',        // Your work server
  database: 'training_matrix',
  user: 'your_username',
  password: 'your_password',

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};
```

---

## Step 5: Install Dependencies

```bash
# Install Node.js dependencies
npm install
```

---

## Step 6: Test Connection

```bash
node database/test-connection.js
```

You should see:
```
✅ Connection successful!
✅ Found X tables
```

If connection fails, check:
- SQL Server is running
- Server name/IP is correct
- Username/password are correct
- Database exists
- Firewall allows port 1433

---

## Step 7: Install Production Areas & Core Skills

### Option A: Install Everything at Once (Recommended)

```bash
node database/install-all.js
```

This installs:
- **3 Production Areas** (Line 7 Filling, Packing, Tank Room)
- **3 Core Skill Categories** (Mechanical, Software, Electrical)

Wait 5 seconds or press Ctrl+C to cancel.

### Option B: Install Individually

```bash
# Production areas
node database/install-production-areas.js --all

# Core skills
node database/install-core-skills.js --all
```

---

## Step 8: Verify Installation

Run verification queries in SSMS:

```sql
-- Check production areas
SELECT
    pa.name AS Area,
    COUNT(DISTINCT m.id) AS Machines,
    COUNT(c.id) AS Competencies
FROM production_areas pa
LEFT JOIN machines m ON m.production_area_id = pa.id
LEFT JOIN competencies c ON c.machine_id = m.id
GROUP BY pa.name;

-- Check core skills
SELECT
    csc.name AS Category,
    COUNT(cs.id) AS Skills
FROM core_skill_categories csc
LEFT JOIN core_skills cs ON cs.category_id = csc.id
GROUP BY csc.name;
```

Expected results:
- **Production Areas:** 3 areas, 29 machines, 93 competencies
- **Core Skills:** 5 categories, 109 skills

---

## Step 9: Start the Application

```bash
npm run dev
```

Open your browser to: **http://localhost:5173**

---

## Step 10: Login & Setup

### Default Login:
- **Username:** `admin`
- **Password:** `admin123`

⚠️ **IMPORTANT:** Change this password immediately!

1. Login with default credentials
2. Go to **Admin** tab → **Users**
3. Click **Reset Password** for admin user
4. Set a strong password

---

## Quick Start Commands

```bash
# Test database connection
node database/test-connection.js

# Install everything
node database/install-all.js

# Start application
npm run dev

# Open in browser
http://localhost:5173
```

---

## What's Installed

### Production Areas (Line 7)
- **Filling** - 13 machines, 33 competencies
- **Packing** - 9 machines, 26 competencies
- **Tank Room** - 7 systems, 34 competencies

### Core Skills
- **Mechanical** - 7 categories, 28 skills (Welding, Lathe, Mill, Gearbox, Pumps, Pneumatics, Hydraulics)
- **Software** - 12 systems, 53 skills (RSLinx, Rockwell, Staubli, ABB, Cognex, Siemens, etc.)
- **Electrical** - 5 categories, 20 skills (Theory, Hand Skills, Motors, Inverters, Control Panels)
- **Safety** - 4 skills (default - can be updated)
- **Leadership** - 4 skills (default - can be updated)

**Total:** 194 competencies across all areas

---

## Troubleshooting

### Cannot connect to database
```bash
# Check SQL Server is running
# Windows: Services → SQL Server (MSSQLSERVER)
# Check your config.js has correct server/user/password
```

### Tables not found
```bash
# Run schema first
sqlcmd -S localhost -U sa -P pass -d training_matrix -i database/schema.sql
```

### Port 1433 blocked
```bash
# Windows Firewall
netsh advfirewall firewall add rule name="SQL Server" dir=in action=allow protocol=TCP localport=1433

# Linux (Ubuntu)
sudo ufw allow 1433/tcp
```

### Named instance connection
```javascript
// Use double backslash
server: 'SERVERNAME\\SQLEXPRESS'

// Or use comma for port
server: 'SERVERNAME,1433'
```

---

## Security Checklist

✅ Change default admin password
✅ Create dedicated database user (don't use 'sa')
✅ Enable encryption (`encrypt: true`)
✅ Use strong passwords (12+ characters)
✅ Limit database access by IP address
✅ Regular backups
✅ Keep SQL Server updated

---

## Backup & Restore

### Backup
```sql
BACKUP DATABASE training_matrix
TO DISK = 'C:\\Backups\\training_matrix.bak'
WITH COMPRESSION;
```

### Restore
```sql
RESTORE DATABASE training_matrix
FROM DISK = 'C:\\Backups\\training_matrix.bak'
WITH REPLACE;
```

---

## Next Steps

After installation:

1. ✅ Add Engineers (Admin → Engineers)
2. ✅ Assign shifts
3. ✅ Start assessments (Assessment tab)
4. ✅ Track progress (Reports tab)
5. ✅ Use production area filter for focused analysis

---

## Support

For issues:
- Check [SQL_SERVER_SETUP.md](./SQL_SERVER_SETUP.md) for detailed troubleshooting
- Check [database/production-areas/README.md](./database/production-areas/README.md) for production area management
- Check [database/core-skills/README.md](./database/core-skills/README.md) for core skills management

---

## Quick Reference

| Task | Command |
|------|---------|
| Test connection | `node database/test-connection.js` |
| Install all | `node database/install-all.js` |
| Install production areas | `node database/install-production-areas.js --all` |
| Install core skills | `node database/install-core-skills.js --all` |
| Start app | `npm run dev` |
| Login | admin / admin123 |

---

**🎉 You're all set! Enjoy using your Training Matrix system.**
