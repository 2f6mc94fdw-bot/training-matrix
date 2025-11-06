# Quick Start: SQL Database Setup

Get the Training Matrix running with SQL database in 5 minutes!

## For Local Testing (PostgreSQL)

### 1. Install PostgreSQL
```bash
# macOS
brew install postgresql && brew services start postgresql

# Ubuntu
sudo apt install postgresql && sudo service postgresql start

# Windows - Download from: https.postgresql.org/download/windows/
```

### 2. Create Database
```bash
# Connect and create database
psql -U postgres
CREATE DATABASE training_matrix;
\q

# Import schema
psql -U postgres -d training_matrix -f server/db/schema.sql
```

### 3. Configure Connection
```bash
# Copy environment template
cp .env.example .env

# Edit .env with your text editor
# Update this line:
DB_CONNECTION_STRING=postgresql://postgres:YOUR_PASSWORD@localhost:5432/training_matrix
```

### 4. Install Dependencies & Start
```bash
# Install packages
npm install
cd server && npm install && cd ..

# Start backend server (terminal 1)
npm run server

# Start frontend (terminal 2)
npm run dev
```

✅ Done! Open http://localhost:5173

---

## For Work Server (Remote Database)

### 1. Get Credentials from IT Department

You need:
- **Server address**: `db.company.com` or `192.168.1.100`
- **Port**: Usually `5432`
- **Database name**: `training_matrix`
- **Username**: Your database username
- **Password**: Your database password

### 2. Create .env File

```bash
cp .env.example .env
```

Edit `.env`:
```env
DB_CONNECTION_STRING=postgresql://YOUR_USERNAME:YOUR_PASSWORD@YOUR_SERVER:5432/training_matrix
DB_SSL=true
PORT=3001
NODE_ENV=production
```

### 3. Setup Database Schema on Server

```bash
# Connect to work server
psql "postgresql://YOUR_USERNAME:YOUR_PASSWORD@YOUR_SERVER:5432/training_matrix"

# Inside psql, run:
\i server/db/schema.sql
\q
```

Or use this one-liner:
```bash
psql "postgresql://YOUR_USERNAME:YOUR_PASSWORD@YOUR_SERVER:5432/training_matrix" -f server/db/schema.sql
```

### 4. Start Application

```bash
# Install dependencies
npm install
cd server && npm install && cd ..

# Start server
npm run server

# In another terminal, start frontend
npm run dev
```

### 5. Verify

Visit: `http://localhost:3001/api/health`

Should see:
```json
{
  "status": "ok",
  "database": "connected"
}
```

---

## Connection String Examples

### Local Development
```
postgresql://postgres:admin123@localhost:5432/training_matrix
```

### Work Server (with domain)
```
postgresql://john.smith:SecurePass123@db-server.company.com:5432/training_matrix
```

### Work Server (with IP)
```
postgresql://username:password@192.168.1.100:5432/training_matrix
```

### With SSL (production)
```
postgresql://user:pass@server:5432/database?sslmode=require
```

---

## Troubleshooting

### ❌ "Connection refused"
- Check PostgreSQL is running: `sudo service postgresql status`
- Verify server address and port
- Check firewall allows connection

### ❌ "Authentication failed"
- Double-check username and password
- No spaces in connection string
- Password might need URL encoding if it has special characters

### ❌ "Database does not exist"
- Create it first: `createdb training_matrix`
- Or: `CREATE DATABASE training_matrix;` in psql

### ❌ "Port 3001 already in use"
- Change PORT in `.env` to `3002` or another number

---

## Default Login

After setup, login with:
- **Username**: `admin`
- **Password**: `admin123`

**⚠️ Change this password immediately after first login!**

---

## Need Help?

See [DATABASE_SETUP.md](./DATABASE_SETUP.md) for detailed documentation.

Contact your IT department for work server access.
