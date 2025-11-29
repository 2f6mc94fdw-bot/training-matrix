# useData Hook - Function Implementations

This file contains the fixed implementations for stub functions in `src/hooks/useData.js`.

Replace the console.warn() implementations with these full implementations.

---

## Fix 1: updateProductionArea (Line ~125-129)

**Replace:**
```javascript
const updateProductionArea = async (areaId, updates) => {
  // Note: API doesn't have update endpoint yet, would need to add
  console.warn('Update production area not yet implemented in API');
  await logAction('update_production_area', `Updated production area: ${areaId}`);
};
```

**With:**
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
```

---

## Fix 2: addMachine (Line ~143-146)

**Replace:**
```javascript
const addMachine = async (areaId, machine) => {
  console.warn('Add machine not yet fully implemented - needs API endpoint');
  await refreshData();
};
```

**With:**
```javascript
const addMachine = async (areaId, machine) => {
  try {
    await api.createMachine({
      productionAreaId: areaId,
      name: machine.name,
      importance: machine.importance || 1
    });
    await logAction('add_machine', `Added machine: ${machine.name} to area ${areaId}`);
    await refreshData();
  } catch (error) {
    console.error('Error adding machine:', error);
    throw error;
  }
};
```

---

## Fix 3: updateMachine (Line ~148-151)

**Replace:**
```javascript
const updateMachine = async (areaId, machineId, updates) => {
  console.warn('Update machine not yet implemented');
};
```

**With:**
```javascript
const updateMachine = async (areaId, machineId, updates) => {
  try {
    await api.updateMachine(machineId, {
      name: updates.name,
      importance: updates.importance
    });
    await logAction('update_machine', `Updated machine: ${machineId}`);
    await refreshData();
  } catch (error) {
    console.error('Error updating machine:', error);
    throw error;
  }
};
```

---

## Fix 4: deleteMachine (Line ~153-155)

**Replace:**
```javascript
const deleteMachine = async (areaId, machineId) => {
  console.warn('Delete machine not yet implemented');
};
```

**With:**
```javascript
const deleteMachine = async (areaId, machineId) => {
  try {
    await api.deleteMachine(machineId);
    await logAction('delete_machine', `Deleted machine: ${machineId} from area ${areaId}`);
    await refreshData();
  } catch (error) {
    console.error('Error deleting machine:', error);
    throw error;
  }
};
```

---

## Fix 5: addCompetency (Line ~157-159)

**Replace:**
```javascript
const addCompetency = async (areaId, machineId, competency) => {
  console.warn('Add competency not yet implemented');
};
```

**With:**
```javascript
const addCompetency = async (areaId, machineId, competency) => {
  try {
    await api.createCompetency({
      machineId: machineId,
      name: competency.name,
      maxScore: competency.maxScore || 3
    });
    await logAction('add_competency', `Added competency: ${competency.name} to machine ${machineId}`);
    await refreshData();
  } catch (error) {
    console.error('Error adding competency:', error);
    throw error;
  }
};
```

---

## Fix 6: updateCompetency (Line ~161-163)

**Replace:**
```javascript
const updateCompetency = async (areaId, machineId, compId, updates) => {
  console.warn('Update competency not yet implemented');
};
```

**With:**
```javascript
const updateCompetency = async (areaId, machineId, compId, updates) => {
  try {
    await api.updateCompetency(compId, {
      name: updates.name,
      maxScore: updates.maxScore
    });
    await logAction('update_competency', `Updated competency: ${compId}`);
    await refreshData();
  } catch (error) {
    console.error('Error updating competency:', error);
    throw error;
  }
};
```

---

## Fix 7: deleteCompetency (Line ~165-167)

**Replace:**
```javascript
const deleteCompetency = async (areaId, machineId, compId) => {
  console.warn('Delete competency not yet implemented');
};
```

**With:**
```javascript
const deleteCompetency = async (areaId, machineId, compId) => {
  try {
    await api.deleteCompetency(compId);
    await logAction('delete_competency', `Deleted competency: ${compId} from machine ${machineId}`);
    await refreshData();
  } catch (error) {
    console.error('Error deleting competency:', error);
    throw error;
  }
};
```

---

## Optional Enhancements (Future)

### Fix 8: updateCertification (Line ~309)

**Replace:**
```javascript
const updateCertification = async (certId, updates) => {
  console.warn('Update certification not yet implemented');
};
```

**With:**
```javascript
const updateCertification = async (certId, updates) => {
  // Note: Backend doesn't have PUT /certifications/:id endpoint yet
  // Would need to add to server/index.cjs first
  console.warn('Update certification endpoint not available in backend');
  // Workaround: Delete and recreate
  // await deleteCertification(certId);
  // await addCertification(updates);
};
```

### Fix 9: addCoreSkillCategory (Line ~366)

**Replace:**
```javascript
const addCoreSkillCategory = async (name) => {
  console.warn('Add core skill category not yet implemented');
};
```

**With:**
```javascript
const addCoreSkillCategory = async (name) => {
  // Note: Backend doesn't have POST /core-skills/categories endpoint
  // This would require backend implementation first
  console.warn('Add core skill category endpoint not available in backend');
  // Would need to add:
  // await api.createCoreSkillCategory({ id: generateId(), name });
};
```

### Fix 10: addCoreSkill (Line ~369)

**Replace:**
```javascript
const addCoreSkill = async (categoryId, skillName) => {
  console.warn('Add core skill not yet implemented');
};
```

**With:**
```javascript
const addCoreSkill = async (categoryId, skillName) => {
  // Note: Backend doesn't have POST /core-skills endpoint
  // This would require backend implementation first
  console.warn('Add core skill endpoint not available in backend');
  // Would need to add:
  // await api.createCoreSkill({ id: generateId(), categoryId, name: skillName, maxScore: 3 });
};
```

### Fix 11: deleteCoreSkill (Line ~373)

**Replace:**
```javascript
const deleteCoreSkill = async (categoryId, skillId) => {
  console.warn('Delete core skill not yet implemented');
};
```

**With:**
```javascript
const deleteCoreSkill = async (categoryId, skillId) => {
  // Note: Backend doesn't have DELETE /core-skills/:id endpoint
  // This would require backend implementation first
  console.warn('Delete core skill endpoint not available in backend');
  // Would need to add:
  // await api.deleteCoreSkill(skillId);
};
```

---

## Quick Apply Script

To apply all fixes automatically, you can use this sed script:

```bash
# Backup original
cp src/hooks/useData.js src/hooks/useData.js.backup

# Apply fixes (manual editing recommended for safety)
# Or use the provided api-FIXED.js and manually update useData.js
```

---

## Testing After Fixes

After applying all fixes, test:

1. **Add Machine:**
   - Go to Admin → Production Areas
   - Select an area
   - Click "Add Machine"
   - Fill form and save
   - Should work without console warnings

2. **Update Machine:**
   - Edit a machine
   - Change name or importance
   - Save
   - Should update successfully

3. **Delete Machine:**
   - Click delete on a machine
   - Confirm
   - Should remove from list

4. **Competencies:**
   - Same tests as machines

5. **Production Area Update:**
   - Edit production area name
   - Save
   - Should update successfully

---

## Notes

- Fixes 1-7 are **required** for full production area management
- Fixes 8-11 are **optional** - these features can be added later
- All fixed functions follow the same pattern:
  1. Call API method
  2. Log action to audit trail
  3. Refresh data
  4. Handle errors

---

**Integration Status After Fixes:**
- ✅ Production Areas: CRUD complete
- ✅ Machines: CRUD complete
- ✅ Competencies: CRUD complete
- ⚠️ Core Skills: Read-only (backend endpoints needed)
- ⚠️ Certifications: Create and Delete only (Update needs backend)
