# Testing SQL Server Integration

## ✅ What's Complete

1. **Backend API** - Fully configured for SQL Server
   - `server/index.cjs` - API server with all endpoints
   - `database/connection.js` - SQL Server connection with PostgreSQL compatibility
   - Auto-converts PostgreSQL syntax to T-SQL

2. **Frontend API Client** - `src/utils/api.js`
   - Complete API client for all endpoints
   - Health check, auth, engineers, production areas, assessments, etc.

3. **Authentication Updated** - `src/hooks/useAuth.js`
   - Uses API for login
   - Saves session in sessionStorage
   - Logs actions to audit trail

4. **Package Scripts** - Ready to run full stack
   - `npm run server` - Start backend only
   - `npm run dev` - Start frontend only
   - `npm start` - Start both (full stack)

## 🧪 Testing Instructions (On Your Mac)

### Step 1: Pull Latest Code

```bash
cd /Users/danielabley/Desktop/training-matrix-main-2
git pull
```

### Step 2: Install Dependencies

```bash
npm install
```

This will install `concurrently` for running both servers.

### Step 3: Make Sure Docker SQL Server is Running

```bash
docker ps
```

You should see `sql-server-training`. If not:

```bash
docker start sql-server-training
```

### Step 4: Test Backend Server Only

```bash
npm run server
```

You should see:
```
🔌 Connecting to SQL Server...
✅ Connected to SQL Server
   Server: localhost
   Database: training_matrix
Server listening on port 3001
```

**Test the health endpoint:**

Open a new terminal and run:
```bash
curl http://localhost:3001/api/health
```

You should see:
```json
{
  "status": "ok",
  "database": "connected",
  "timestamp": "..."
}
```

**Stop the server** (Ctrl+C) before proceeding.

### Step 5: Run Full Stack

```bash
npm start
```

This runs BOTH the backend API (port 3001) AND the frontend (port 5173).

You should see output from both servers.

### Step 6: Test Login

1. **Open browser:** `http://localhost:5173`

2. **Try to login:**
   - Username: `admin`
   - Password: `admin123`

3. **Check browser console** (F12 → Console tab)
   - Look for any API errors
   - Should see successful API calls

### Step 7: Check Production Areas

After logging in:
- Go to the Matrix view
- Check if you see your 6 production areas from SQL Server

**If you don't see them:**
- Open browser console (F12)
- Look for errors
- The app still uses localStorage for data, not the API

## 🔍 Debugging

### Backend Not Starting

**Error: "Cannot find module 'concurrently'"**
```bash
npm install
```

**Error: "SQL Server connection failed"**
```bash
# Check Docker is running
docker ps

# Check config file exists
ls database/config.cjs

# Test connection
node database/check-data.cjs
```

### Frontend Can't Connect to Backend

**Error: "Failed to fetch" or "Network Error"**

Check:
1. Backend is running (`npm run server` in separate terminal)
2. Backend is on port 3001
3. Frontend is on port 5173
4. No firewall blocking localhost connections

### Login Not Working

**Check backend logs** - You should see:
```
2025-11-07T... - POST /api/auth/login
```

**Check browser console** - Should see:
```
POST http://localhost:3001/api/auth/login
```

If you see CORS errors, make sure backend is running.

## ⚠️ Current Limitation

**The app still loads production areas from localStorage**, not from the SQL Server API.

To see your SQL Server data in the app, we need to:

1. Update `src/hooks/useData.js` to use the API (534 lines to rewrite)
2. Transform API responses to match the current data structure
3. Test all CRUD operations

This is the remaining work for full SQL Server integration.

## 📊 What's in SQL Server Right Now

Check your database:
```bash
node database/check-data.cjs
```

You should see:
- 6 production areas
- 35 machines
- 8 engineers

## 🎯 Next Steps

### Option A: Test What We Have

Test that:
1. Backend starts successfully ✅
2. Frontend connects to backend ✅
3. Login works with SQL Server ✅
4. Health check works ✅

### Option B: Continue Integration

Update `src/hooks/useData.js` to:
- Load production areas from API
- Load engineers from API
- Load assessments from API
- Save changes via API

This requires rewriting the data layer (~4-6 hours).

---

**Which would you like to do?**
- Test current progress first, or
- Continue with full integration?
