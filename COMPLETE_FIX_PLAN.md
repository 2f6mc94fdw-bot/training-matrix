# TRAINING MATRIX - COMPLETE FIX IMPLEMENTATION PLAN

This document provides step-by-step instructions to fix ALL issues in the Training Matrix application.

---

## QUICK FIX OVERVIEW

1. Fix CORS configuration
2. Fix Core Skills database schema
3. Remove service worker interference
4. Add missing Production Areas API endpoints
5. Update frontend to use new endpoints
6. Create comprehensive setup script with sample data

---

## FIX #1: CORS CONFIGURATION (2 minutes)

### Problem
Backend expects frontend on port 5173, but it's actually on port 3000.

### Solution
Edit `/home/user/training-matrix/.env` line 9:

```bash
# BEFORE:
FRONTEND_URL=http://localhost:5173

# AFTER:
FRONTEND_URL=http://localhost:3000
```

### Verification
```bash
# Restart backend server
cd /home/user/training-matrix/server
node index.cjs

# In another terminal, test CORS:
curl -v -H "Origin: http://localhost:3000" \
  -H "Content-Type: application/json" \
  -X OPTIONS http://localhost:3001/api/health

# Should see: Access-Control-Allow-Origin: http://localhost:3000
```

---

## FIX #2: CORE SKILLS SCHEMA (5 minutes)

### Problem
Database schema has wrong foreign keys for core_skill_assessments.

### Solution
Create new setup script that fixes the schema.

**File: `/home/user/training-matrix/server/setup-database-fixed.js`**

See attached file for complete implementation.

Key changes:
- Add `category_id` column
- Rename `core_skill_id` to `skill_id`
- Add proper foreign keys
- Add unique constraint

### Run the fix:
```bash
cd /home/user/training-matrix
node server/setup-database-fixed.js
```

---

## FIX #3: SERVICE WORKER (2 minutes)

### Problem
Service worker file exists and may be cached in browsers.

### Solution A: Delete service worker
```bash
cd /home/user/training-matrix
rm public/sw.js
rm public/manifest.json
```

### Solution B: Add unregister code (BETTER)
Keep files but add unregister code to ensure cached workers are removed.

**File: `/home/user/training-matrix/src/main.jsx`**
Add after line 22:

```javascript
// Unregister any existing service workers
if ('serviceWorker' in navigator) {
  navigator.serviceWorker.getRegistrations().then(function(registrations) {
    for(let registration of registrations) {
      registration.unregister();
      console.log('Service Worker unregistered:', registration);
    }
  });
}
```

---

## FIX #4: PRODUCTION AREAS API (20 minutes)

### Problem
Missing 7 API endpoints for managing machines and competencies.

### Solution
Add endpoints to `/home/user/training-matrix/server/index.cjs`

Insert after line 352 (after DELETE production area endpoint):

```javascript
// ==================== PRODUCTION AREA UPDATE ====================
app.put('/api/production-areas/:id', async (req, res) => {
  try {
    const { id } = req.params;
    const { name } = req.body;

    if (!name || !validation.validateStringLength(name, 1, 200)) {
      return res.status(400).json({ error: 'Invalid name' });
    }

    await db.query(
      'UPDATE production_areas SET name = $1, updated_at = CURRENT_TIMESTAMP WHERE id = $2',
      [name, id]
    );

    res.json({ success: true });
  } catch (error) {
    console.error('Update production area error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== MACHINES ====================
app.post('/api/production-areas/:areaId/machines', async (req, res) => {
  try {
    const { areaId } = req.params;
    const { name, importance } = req.body;

    if (!name || !validation.validateStringLength(name, 1, 200)) {
      return res.status(400).json({ error: 'Invalid machine name' });
    }

    const result = await db.query(
      'INSERT INTO machines (production_area_id, name, importance) VALUES ($1, $2, $3) RETURNING *',
      [areaId, name, importance || 1]
    );

    res.json(result.rows[0]);
  } catch (error) {
    console.error('Create machine error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.put('/api/machines/:id', async (req, res) => {
  try {
    const { id } = req.params;
    const { name, importance } = req.body;

    await db.query(
      'UPDATE machines SET name = $1, importance = $2, updated_at = CURRENT_TIMESTAMP WHERE id = $3',
      [name, importance, id]
    );

    res.json({ success: true });
  } catch (error) {
    console.error('Update machine error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.delete('/api/machines/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await db.query('DELETE FROM machines WHERE id = $1', [id]);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete machine error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== COMPETENCIES ====================
app.post('/api/machines/:machineId/competencies', async (req, res) => {
  try {
    const { machineId } = req.params;
    const { name, maxScore } = req.body;

    if (!name || !validation.validateStringLength(name, 1, 200)) {
      return res.status(400).json({ error: 'Invalid competency name' });
    }

    const result = await db.query(
      'INSERT INTO competencies (machine_id, name, max_score) VALUES ($1, $2, $3) RETURNING *',
      [machineId, name, maxScore || 3]
    );

    res.json(result.rows[0]);
  } catch (error) {
    console.error('Create competency error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.put('/api/competencies/:id', async (req, res) => {
  try {
    const { id } = req.params;
    const { name, maxScore } = req.body;

    await db.query(
      'UPDATE competencies SET name = $1, max_score = $2, updated_at = CURRENT_TIMESTAMP WHERE id = $3',
      [name, maxScore, id]
    );

    res.json({ success: true });
  } catch (error) {
    console.error('Update competency error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.delete('/api/competencies/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await db.query('DELETE FROM competencies WHERE id = $1', [id]);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete competency error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});
```

---

## FIX #5: FRONTEND DATA HOOKS (10 minutes)

### Problem
Frontend hooks have placeholder implementations that don't call real APIs.

### Solution
Update `/home/user/training-matrix/src/hooks/useData.js`

Replace lines 125-167 with:

```javascript
const updateProductionArea = async (areaId, updates) => {
  try {
    await api.updateProductionArea(areaId, updates);
    await logAction('update_production_area', `Updated production area: ${areaId}`);
    await refreshData();
  } catch (error) {
    console.error('Error updating production area:', error);
    throw error;
  }
};

const addMachine = async (areaId, machine) => {
  try {
    await api.addMachine(areaId, machine);
    await logAction('add_machine', `Added machine to area ${areaId}`);
    await refreshData();
  } catch (error) {
    console.error('Error adding machine:', error);
    throw error;
  }
};

const updateMachine = async (areaId, machineId, updates) => {
  try {
    await api.updateMachine(machineId, updates);
    await logAction('update_machine', `Updated machine: ${machineId}`);
    await refreshData();
  } catch (error) {
    console.error('Error updating machine:', error);
    throw error;
  }
};

const deleteMachine = async (areaId, machineId) => {
  try {
    await api.deleteMachine(machineId);
    await logAction('delete_machine', `Deleted machine: ${machineId}`);
    await refreshData();
  } catch (error) {
    console.error('Error deleting machine:', error);
    throw error;
  }
};

const addCompetency = async (areaId, machineId, competency) => {
  try {
    await api.addCompetency(machineId, competency);
    await logAction('add_competency', `Added competency to machine ${machineId}`);
    await refreshData();
  } catch (error) {
    console.error('Error adding competency:', error);
    throw error;
  }
};

const updateCompetency = async (areaId, machineId, compId, updates) => {
  try {
    await api.updateCompetency(compId, updates);
    await logAction('update_competency', `Updated competency: ${compId}`);
    await refreshData();
  } catch (error) {
    console.error('Error updating competency:', error);
    throw error;
  }
};

const deleteCompetency = async (areaId, machineId, compId) => {
  try {
    await api.deleteCompetency(compId);
    await logAction('delete_competency', `Deleted competency: ${compId}`);
    await refreshData();
  } catch (error) {
    console.error('Error deleting competency:', error);
    throw error;
  }
};
```

---

## FIX #6: FRONTEND API CLIENT (10 minutes)

### Problem
API client doesn't have methods for new endpoints.

### Solution
Update `/home/user/training-matrix/src/utils/api.js`

Add after line 115 (after deleteProductionArea):

```javascript
async updateProductionArea(areaId, areaData) {
  return this.request(`/production-areas/${areaId}`, {
    method: 'PUT',
    body: JSON.stringify(areaData),
  });
}

async addMachine(areaId, machineData) {
  return this.request(`/production-areas/${areaId}/machines`, {
    method: 'POST',
    body: JSON.stringify(machineData),
  });
}

async updateMachine(machineId, machineData) {
  return this.request(`/machines/${machineId}`, {
    method: 'PUT',
    body: JSON.stringify(machineData),
  });
}

async deleteMachine(machineId) {
  return this.request(`/machines/${machineId}`, {
    method: 'DELETE',
  });
}

async addCompetency(machineId, compData) {
  return this.request(`/machines/${machineId}/competencies`, {
    method: 'POST',
    body: JSON.stringify(compData),
  });
}

async updateCompetency(compId, compData) {
  return this.request(`/competencies/${compId}`, {
    method: 'PUT',
    body: JSON.stringify(compData),
  });
}

async deleteCompetency(compId) {
  return this.request(`/competencies/${compId}`, {
    method: 'DELETE',
  });
}
```

---

## COMPLETE SETUP SCRIPT WITH SAMPLE DATA

Create `/home/user/training-matrix/server/setup-complete.js` - see attached file.

This script:
- Creates all tables with correct schema
- Creates admin user
- Adds sample engineers
- Adds sample production areas with machines and competencies
- Adds sample core skill categories and skills
- One command to get fully working system

---

## DEPLOYMENT CHECKLIST

Run these steps in order:

### Step 1: Apply Configuration Fixes
```bash
cd /home/user/training-matrix

# Fix CORS
sed -i 's|FRONTEND_URL=http://localhost:5173|FRONTEND_URL=http://localhost:3000|g' .env

# Verify change
grep FRONTEND_URL .env
```

### Step 2: Update Code Files
```bash
# Create the complete setup script (provided separately)
# Create the fixed backend API endpoints (provided separately)
# Update frontend hooks (provided separately)
# Update frontend API client (provided separately)
```

### Step 3: Setup Database
```bash
cd /home/user/training-matrix
node server/setup-complete.js
```

### Step 4: Start Servers
```bash
# Terminal 1 - Backend
cd /home/user/training-matrix/server
node index.cjs

# Terminal 2 - Frontend
cd /home/user/training-matrix
npm run dev

# Or use the combined start script:
npm start
```

### Step 5: Test in Browser
1. Open http://localhost:3000
2. Login with admin / admin123
3. Check console for errors (should be none)
4. Try adding a production area
5. Try adding a machine to the area
6. Try adding a competency to the machine
7. Try editing all of the above
8. Try deleting (in reverse order)

### Step 6: Clear Browser Cache
If service worker was cached:
```
1. Open browser DevTools (F12)
2. Go to Application tab
3. Click "Service Workers"
4. Unregister all service workers
5. Click "Clear storage"
6. Hard refresh (Ctrl+Shift+R)
```

---

## VERIFICATION TESTS

### Test 1: Login
```bash
curl -X POST http://localhost:3001/api/auth/login \
  -H "Content-Type: application/json" \
  -H "Origin: http://localhost:3000" \
  -d '{"username":"admin","password":"admin123"}'

# Should return user object without password
```

### Test 2: Get Production Areas
```bash
curl http://localhost:3001/api/production-areas

# Should return array with sample data
```

### Test 3: Add Production Area
```bash
curl -X POST http://localhost:3001/api/production-areas \
  -H "Content-Type: application/json" \
  -d '{"name":"Test Area","machines":[]}'

# Should return success
```

### Test 4: Add Machine
```bash
curl -X POST http://localhost:3001/api/production-areas/1/machines \
  -H "Content-Type: application/json" \
  -d '{"name":"Test Machine","importance":1}'

# Should return machine object
```

### Test 5: Core Skills Assessment
```bash
curl -X POST http://localhost:3001/api/core-skills/assessments \
  -H "Content-Type: application/json" \
  -d '{"engineerId":"eng1","categoryId":1,"skillId":1,"score":2}'

# Should return success
```

---

## TROUBLESHOOTING

### Issue: CORS errors still occur
```bash
# Check .env file
cat .env | grep FRONTEND_URL

# Should show: FRONTEND_URL=http://localhost:3000

# Restart backend server
cd server && node index.cjs
```

### Issue: Login fails with 401
```bash
# Test database connection
node test-login.js

# Should show admin user exists and password matches
```

### Issue: Cannot add production areas
```bash
# Check backend logs for errors
# Verify database connection
# Check that setup script ran successfully
```

### Issue: Service worker still cached
```bash
# In browser console:
navigator.serviceWorker.getRegistrations().then(r => console.log(r))

# Should show empty array
# If not, manually unregister in Application tab
```

---

## SUCCESS METRICS

After applying all fixes, verify:
- [ ] No CORS errors in browser console
- [ ] Login works with admin/admin123
- [ ] Production areas page loads
- [ ] Can add new production area
- [ ] Can add machine to area
- [ ] Can add competency to machine
- [ ] Can edit area/machine/competency
- [ ] Can delete in proper order
- [ ] Core skills page works
- [ ] Can assess core skills
- [ ] All API calls return < 1s
- [ ] No 404 errors
- [ ] No service worker active

---

**END OF FIX PLAN**
