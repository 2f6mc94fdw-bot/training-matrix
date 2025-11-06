# Quick Start Guide

## Getting Started in 5 Steps

### Step 1: Install Dependencies
```bash
npm install
```

### Step 2: Configure Your Database

Create your config file:
```bash
cp database/config.example.js database/config.js
```

Edit `database/config.js` and update with your SQL Server details:
```javascript
const config = {
  server: 'YOUR_SERVER_NAME',      // e.g., 'localhost' or '192.168.1.100'
  database: 'training_matrix',
  user: 'YOUR_USERNAME',           // e.g., 'sa'
  password: 'YOUR_PASSWORD',

  options: {
    encrypt: true,
    trustServerCertificate: true,
    enableArithAbort: true
  }
};

module.exports = config;
```

### Step 3: Create Database Tables

Open SQL Server Management Studio (SSMS):
1. Connect to your SQL Server
2. Create a new database called `training_matrix`
3. Open the file `database/schema.sql`
4. Select the `training_matrix` database
5. Click Execute (F5)

### Step 4: Install the Data
```bash
node database/install-all.js
```

You should see:
```
✅ Installed: Line 7 Filling (13 machines, 33 competencies)
✅ Installed: Line 7 Packing (9 machines, 26 competencies)
✅ Installed: Line 7 Tank Room (7 systems, 34 competencies)
✅ Installed: Mechanical Skills (7 categories, 28 skills)
✅ Installed: Software Skills (12 systems, 53 skills)
✅ Installed: Electrical Skills (5 categories, 20 skills)

Grand Total: 194 competencies installed ✅
```

### Step 5: Start the Application
```bash
npm run dev
```

This starts both the backend (port 3001) and frontend (port 5173).

Open your browser to: **http://localhost:5173**

**Login:**
- Username: `admin`
- Password: `admin123`

**⚠️ IMPORTANT:** Change this password immediately after logging in!

---

## That's It!

You're now running the Training Matrix with SQL Server. All your data is saved to the database.

## Having Problems?

**Can't connect to database?**
```bash
node database/test-connection.js
```
This will tell you if your database connection is working.

**Backend won't start?**
Make sure you ran `npm install` and your `database/config.js` file is set up correctly.

**Frontend can't reach backend?**
Make sure the backend is running (you should see "Server running on http://localhost:3001" in your terminal).

---

## Need More Help?

See detailed documentation:
- **INSTALLATION.md** - Complete setup guide
- **SQL_SERVER_INTEGRATION.md** - Full technical documentation
