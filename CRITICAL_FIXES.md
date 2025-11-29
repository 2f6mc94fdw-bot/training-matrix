# Critical Fixes for Training Matrix

This document provides immediate fixes for the issues found in the system review.

---

## Fix 1: Update .env.example (PORT MISMATCH)

**File:** `.env.example`
**Line 9:** Change `FRONTEND_URL` to match Vite config

**Current:**
```env
FRONTEND_URL=http://localhost:5173
```

**Fixed:**
```env
FRONTEND_URL=http://localhost:3000
```

**Action:** This fix has been applied in the next section.

---

## Fix 2: Create .env File

**Priority:** CRITICAL - Required to start application

**Command:**
```bash
cp .env.example .env
```

**Then edit `.env` and update these values:**
```env
DB_SERVER=localhost          # Your SQL Server name or IP
DB_PASSWORD=YourPassword123  # Your actual SQL Server password
```

**Minimum required .env:**
```env
PORT=3001
NODE_ENV=development
FRONTEND_URL=http://localhost:3000
DB_SERVER=localhost
DB_NAME=training_matrix
DB_USER=sa
DB_PASSWORD=YourStrong@Passw0rd
DB_ENCRYPT=true
DB_TRUST_SERVER_CERTIFICATE=true
DB_CONNECTION_TIMEOUT=30000
DB_REQUEST_TIMEOUT=30000
DB_POOL_MAX=10
DB_POOL_MIN=0
DB_POOL_IDLE_TIMEOUT=30000
```

---

## Fix 3: Complete API Client Integration

These fixes have been prepared as a complete replacement file.

**See:** `src/utils/api-FIXED.js`

**Changes:**
1. Added `updateProductionArea` method
2. Added `createMachine` method
3. Added `updateMachine` method
4. Added `deleteMachine` method
5. Added `createCompetency` method
6. Added `updateCompetency` method
7. Added `deleteCompetency` method
8. Fixed `createSnapshot` to send required fields
9. Fixed `logAction` to send required fields

**To apply:**
```bash
mv src/utils/api.js src/utils/api-BACKUP.js
mv src/utils/api-FIXED.js src/utils/api.js
```

---

## Fix 4: Complete useData Hook

These fixes have been prepared as a patch file.

**See:** `src/hooks/useData-FIXED.js`

**Changes:**
1. Implemented `updateProductionArea` function
2. Implemented `addMachine` function
3. Implemented `updateMachine` function
4. Implemented `deleteMachine` function
5. Implemented `addCompetency` function
6. Implemented `updateCompetency` function
7. Implemented `deleteCompetency` function

**To apply:**
```bash
mv src/hooks/useData.js src/hooks/useData-BACKUP.js
mv src/hooks/useData-FIXED.js src/hooks/useData.js
```

---

## Quick Start After Fixes

### 1. Setup Database
```bash
# Make sure SQL Server is running
# Create database if not exists
# Run schema
sqlcmd -S localhost -U sa -P YourPassword -Q "CREATE DATABASE training_matrix"
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/schema.sql
```

### 2. Configure Environment
```bash
# Create .env file
cp .env.example .env

# Edit .env with your actual database credentials
nano .env  # or use your preferred editor
```

### 3. Install Dependencies
```bash
npm install
```

### 4. Start Application
```bash
# Option 1: Start both frontend and backend together
npm start

# Option 2: Start separately in different terminals
# Terminal 1:
npm run server

# Terminal 2:
npm run dev
```

### 5. Login
- Open browser to `http://localhost:3000`
- Username: `admin`
- Password: `admin123`

---

## Verification Steps

After applying all fixes, verify:

1. **Backend starts successfully:**
   ```bash
   npm run server
   # Should see: ✅ Connected to SQL Server
   # Should see: 🚀 Server running on: http://localhost:3001
   ```

2. **Frontend starts successfully:**
   ```bash
   npm run dev
   # Should see: ➜  Local: http://localhost:3000/
   ```

3. **Login works:**
   - Navigate to http://localhost:3000
   - Enter credentials
   - Should redirect to dashboard

4. **Production area management works:**
   - Go to Admin tab
   - Try adding a production area
   - Try adding a machine to it
   - Try adding a competency
   - Should all work without console warnings

---

## Troubleshooting

### Error: "DB_PASSWORD environment variable is required"
**Solution:** Create .env file with DB_PASSWORD set

### Error: "Failed to connect to database"
**Solution:** Check SQL Server is running and credentials in .env are correct

### Error: "CORS error" in browser console
**Solution:** Make sure FRONTEND_URL in .env matches vite config port (3000)

### Error: "Cannot find module"
**Solution:** Run `npm install` to install dependencies

---

## Files Modified Summary

1. `.env.example` - Fixed FRONTEND_URL port
2. `.env` - Created from example (user must do this)
3. `src/utils/api.js` - Added 9 missing methods
4. `src/hooks/useData.js` - Implemented 7 stub functions

---

## Time to Apply All Fixes

- Create .env: 2 minutes
- Apply API fixes: 1 minute (copy file)
- Apply useData fixes: 1 minute (copy file)
- Test application: 5 minutes

**Total: ~10 minutes**

---

## Next Steps After Fixes

Once all fixes are applied and verified:

1. Change default admin password
2. Add sample production areas using the provided SQL scripts
3. Add sample engineers
4. Start using the system!
5. Consider Phase 2 improvements from IMPROVEMENT_PLAN.md

---

**Need Help?** Check SYSTEM_REVIEW_REPORT.md for detailed analysis
