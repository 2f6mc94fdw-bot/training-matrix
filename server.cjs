// Express API Server for Training Matrix
// Bridges React frontend with SQL Server database

const express = require('express');
const cors = require('cors');
const bcrypt = require('bcryptjs');
const db = require('./database/queries');

const app = express();
const PORT = process.env.PORT || 3001;

// Middleware
app.use(cors());
app.use(express.json());

// Request logging
app.use((req, res, next) => {
  console.log(`${new Date().toISOString()} - ${req.method} ${req.path}`);
  next();
});

// ============================================================================
// AUTHENTICATION
// ============================================================================

// Login
app.post('/api/auth/login', async (req, res) => {
  try {
    const { username, password } = req.body;

    const user = await db.getUserByUsername(username);
    if (!user) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    const isValid = await db.verifyUserPassword(username, password);
    if (!isValid) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    // Remove password from response
    const { password: _, ...userWithoutPassword } = user;
    res.json(userWithoutPassword);
  } catch (error) {
    console.error('Login error:', error);
    res.status(500).json({ error: 'Login failed' });
  }
});

// ============================================================================
// DATA - Get All
// ============================================================================

// Get all data (initial load)
app.get('/api/data', async (req, res) => {
  try {
    const data = await db.getAllData();
    res.json(data);
  } catch (error) {
    console.error('Get all data error:', error);
    res.status(500).json({ error: 'Failed to load data' });
  }
});

// ============================================================================
// USERS
// ============================================================================

app.get('/api/users', async (req, res) => {
  try {
    const users = await db.getAllUsers();
    res.json(users);
  } catch (error) {
    console.error('Get users error:', error);
    res.status(500).json({ error: 'Failed to get users' });
  }
});

app.post('/api/users', async (req, res) => {
  try {
    const { username, password, role, engineerId } = req.body;
    const user = await db.createUser({ username, password, role, engineerId });
    res.status(201).json(user);
  } catch (error) {
    console.error('Create user error:', error);
    res.status(500).json({ error: 'Failed to create user' });
  }
});

app.delete('/api/users/:id', async (req, res) => {
  try {
    await db.deleteUser(req.params.id);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete user error:', error);
    res.status(500).json({ error: 'Failed to delete user' });
  }
});

app.put('/api/users/:id/password', async (req, res) => {
  try {
    const { password } = req.body;
    await db.updateUserPassword(req.params.id, password);
    res.json({ success: true });
  } catch (error) {
    console.error('Reset password error:', error);
    res.status(500).json({ error: 'Failed to reset password' });
  }
});

// ============================================================================
// ENGINEERS
// ============================================================================

app.get('/api/engineers', async (req, res) => {
  try {
    const engineers = await db.getAllEngineers();
    res.json(engineers);
  } catch (error) {
    console.error('Get engineers error:', error);
    res.status(500).json({ error: 'Failed to get engineers' });
  }
});

app.post('/api/engineers', async (req, res) => {
  try {
    const { name, shift } = req.body;
    const engineer = await db.createEngineer({ name, shift });

    // Auto-create user account
    const username = name.toLowerCase().replace(/\s+/g, '.');
    let user = null;
    let userCreated = false;

    try {
      const existingUser = await db.getUserByUsername(username);
      if (!existingUser) {
        user = await db.createUser({
          username,
          password: 'password',
          role: 'engineer',
          engineerId: engineer.id
        });
        userCreated = true;
      }
    } catch (userError) {
      console.warn('Failed to create user account:', userError);
    }

    res.status(201).json({ engineer, username, userCreated });
  } catch (error) {
    console.error('Create engineer error:', error);
    res.status(500).json({ error: 'Failed to create engineer' });
  }
});

app.put('/api/engineers/:id', async (req, res) => {
  try {
    const { name, shift } = req.body;
    await db.updateEngineer(req.params.id, { name, shift });
    res.json({ success: true });
  } catch (error) {
    console.error('Update engineer error:', error);
    res.status(500).json({ error: 'Failed to update engineer' });
  }
});

app.delete('/api/engineers/:id', async (req, res) => {
  try {
    await db.deleteEngineer(req.params.id);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete engineer error:', error);
    res.status(500).json({ error: 'Failed to delete engineer' });
  }
});

// ============================================================================
// PRODUCTION AREAS
// ============================================================================

app.get('/api/production-areas', async (req, res) => {
  try {
    const areas = await db.getAllProductionAreas();
    res.json(areas);
  } catch (error) {
    console.error('Get production areas error:', error);
    res.status(500).json({ error: 'Failed to get production areas' });
  }
});

app.post('/api/production-areas', async (req, res) => {
  try {
    const { name } = req.body;
    const area = await db.createProductionArea({ name });
    res.status(201).json(area);
  } catch (error) {
    console.error('Create production area error:', error);
    res.status(500).json({ error: 'Failed to create production area' });
  }
});

app.put('/api/production-areas/:id', async (req, res) => {
  try {
    const { name } = req.body;
    await db.updateProductionArea(req.params.id, { name });
    res.json({ success: true });
  } catch (error) {
    console.error('Update production area error:', error);
    res.status(500).json({ error: 'Failed to update production area' });
  }
});

app.delete('/api/production-areas/:id', async (req, res) => {
  try {
    await db.deleteProductionArea(req.params.id);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete production area error:', error);
    res.status(500).json({ error: 'Failed to delete production area' });
  }
});

// ============================================================================
// MACHINES
// ============================================================================

app.post('/api/production-areas/:areaId/machines', async (req, res) => {
  try {
    const { name, importance } = req.body;
    const machine = await db.createMachine(req.params.areaId, { name, importance });
    res.status(201).json(machine);
  } catch (error) {
    console.error('Create machine error:', error);
    res.status(500).json({ error: 'Failed to create machine' });
  }
});

app.put('/api/production-areas/:areaId/machines/:machineId', async (req, res) => {
  try {
    const { name, importance } = req.body;
    await db.updateMachine(req.params.machineId, { name, importance });
    res.json({ success: true });
  } catch (error) {
    console.error('Update machine error:', error);
    res.status(500).json({ error: 'Failed to update machine' });
  }
});

app.delete('/api/production-areas/:areaId/machines/:machineId', async (req, res) => {
  try {
    await db.deleteMachine(req.params.machineId);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete machine error:', error);
    res.status(500).json({ error: 'Failed to delete machine' });
  }
});

// ============================================================================
// COMPETENCIES
// ============================================================================

app.post('/api/production-areas/:areaId/machines/:machineId/competencies', async (req, res) => {
  try {
    const { name, maxScore } = req.body;
    const competency = await db.createCompetency(req.params.machineId, { name, maxScore });
    res.status(201).json(competency);
  } catch (error) {
    console.error('Create competency error:', error);
    res.status(500).json({ error: 'Failed to create competency' });
  }
});

app.put('/api/production-areas/:areaId/machines/:machineId/competencies/:compId', async (req, res) => {
  try {
    const { name, maxScore } = req.body;
    await db.updateCompetency(req.params.compId, { name, maxScore });
    res.json({ success: true });
  } catch (error) {
    console.error('Update competency error:', error);
    res.status(500).json({ error: 'Failed to update competency' });
  }
});

app.delete('/api/production-areas/:areaId/machines/:machineId/competencies/:compId', async (req, res) => {
  try {
    await db.deleteCompetency(req.params.compId);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete competency error:', error);
    res.status(500).json({ error: 'Failed to delete competency' });
  }
});

// ============================================================================
// ASSESSMENTS
// ============================================================================

app.get('/api/assessments', async (req, res) => {
  try {
    const assessments = await db.getAllAssessments();
    res.json(assessments);
  } catch (error) {
    console.error('Get assessments error:', error);
    res.status(500).json({ error: 'Failed to get assessments' });
  }
});

app.post('/api/assessments', async (req, res) => {
  try {
    const { engineerId, areaId, machineId, competencyId, score } = req.body;
    await db.saveAssessment(engineerId, areaId, machineId, competencyId, score);
    res.json({ success: true });
  } catch (error) {
    console.error('Save assessment error:', error);
    res.status(500).json({ error: 'Failed to save assessment' });
  }
});

app.post('/api/assessments/bulk', async (req, res) => {
  try {
    const { updates } = req.body;
    await db.bulkUpdateAssessments(updates);
    res.json({ success: true });
  } catch (error) {
    console.error('Bulk update assessments error:', error);
    res.status(500).json({ error: 'Failed to bulk update assessments' });
  }
});

// ============================================================================
// CORE SKILLS
// ============================================================================

app.get('/api/core-skills', async (req, res) => {
  try {
    const coreSkills = await db.getCoreSkills();
    res.json(coreSkills);
  } catch (error) {
    console.error('Get core skills error:', error);
    res.status(500).json({ error: 'Failed to get core skills' });
  }
});

app.get('/api/core-skills/assessments', async (req, res) => {
  try {
    const assessments = await db.getCoreSkillAssessments();
    res.json(assessments);
  } catch (error) {
    console.error('Get core skill assessments error:', error);
    res.status(500).json({ error: 'Failed to get core skill assessments' });
  }
});

app.post('/api/core-skills/assessments', async (req, res) => {
  try {
    const { engineerId, categoryId, skillId, score } = req.body;
    await db.saveCoreSkillAssessment(engineerId, categoryId, skillId, score);
    res.json({ success: true });
  } catch (error) {
    console.error('Save core skill assessment error:', error);
    res.status(500).json({ error: 'Failed to save core skill assessment' });
  }
});

app.post('/api/core-skills/categories', async (req, res) => {
  try {
    const { name } = req.body;
    const category = await db.createCoreSkillCategory({ name });
    res.status(201).json(category);
  } catch (error) {
    console.error('Create core skill category error:', error);
    res.status(500).json({ error: 'Failed to create category' });
  }
});

app.post('/api/core-skills/categories/:categoryId/skills', async (req, res) => {
  try {
    const { name, maxScore } = req.body;
    const skill = await db.createCoreSkill(req.params.categoryId, { name, maxScore });
    res.status(201).json(skill);
  } catch (error) {
    console.error('Create core skill error:', error);
    res.status(500).json({ error: 'Failed to create skill' });
  }
});

app.delete('/api/core-skills/:skillId', async (req, res) => {
  try {
    await db.deleteCoreSkill(req.params.skillId);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete core skill error:', error);
    res.status(500).json({ error: 'Failed to delete skill' });
  }
});

// ============================================================================
// CERTIFICATIONS
// ============================================================================

app.get('/api/certifications', async (req, res) => {
  try {
    const certifications = await db.getAllCertifications();
    res.json(certifications);
  } catch (error) {
    console.error('Get certifications error:', error);
    res.status(500).json({ error: 'Failed to get certifications' });
  }
});

app.post('/api/certifications', async (req, res) => {
  try {
    const { name, validityDays } = req.body;
    const cert = await db.createCertification({ name, validityDays });
    res.status(201).json(cert);
  } catch (error) {
    console.error('Create certification error:', error);
    res.status(500).json({ error: 'Failed to create certification' });
  }
});

app.delete('/api/certifications/:id', async (req, res) => {
  try {
    await db.deleteCertification(req.params.id);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete certification error:', error);
    res.status(500).json({ error: 'Failed to delete certification' });
  }
});

// ============================================================================
// SNAPSHOTS
// ============================================================================

app.get('/api/snapshots', async (req, res) => {
  try {
    const snapshots = await db.getAllSnapshots();
    res.json(snapshots);
  } catch (error) {
    console.error('Get snapshots error:', error);
    res.status(500).json({ error: 'Failed to get snapshots' });
  }
});

app.post('/api/snapshots', async (req, res) => {
  try {
    const { description } = req.body;
    const snapshot = await db.createSnapshot(description);
    res.status(201).json(snapshot);
  } catch (error) {
    console.error('Create snapshot error:', error);
    res.status(500).json({ error: 'Failed to create snapshot' });
  }
});

// ============================================================================
// AUDIT LOGS
// ============================================================================

app.get('/api/audit-logs', async (req, res) => {
  try {
    const logs = await db.getAllAuditLogs();
    res.json(logs);
  } catch (error) {
    console.error('Get audit logs error:', error);
    res.status(500).json({ error: 'Failed to get audit logs' });
  }
});

app.post('/api/audit-logs', async (req, res) => {
  try {
    const { user, action, details } = req.body;
    await db.createAuditLog({ user, action, details });
    res.json({ success: true });
  } catch (error) {
    console.error('Create audit log error:', error);
    res.status(500).json({ error: 'Failed to create audit log' });
  }
});

// ============================================================================
// HEALTH CHECK
// ============================================================================

app.get('/api/health', async (req, res) => {
  try {
    const connected = await db.testConnection();
    res.json({
      status: connected ? 'healthy' : 'unhealthy',
      database: connected ? 'connected' : 'disconnected',
      timestamp: new Date().toISOString()
    });
  } catch (error) {
    res.status(500).json({
      status: 'unhealthy',
      database: 'error',
      error: error.message
    });
  }
});

// ============================================================================
// ERROR HANDLING
// ============================================================================

// 404 handler
app.use((req, res) => {
  res.status(404).json({ error: 'Endpoint not found' });
});

// Global error handler
app.use((err, req, res, next) => {
  console.error('Server error:', err);
  res.status(500).json({ error: 'Internal server error' });
});

// ============================================================================
// START SERVER
// ============================================================================

app.listen(PORT, () => {
  console.log('╔══════════════════════════════════════════════════╗');
  console.log('║   Training Matrix API Server                     ║');
  console.log('╚══════════════════════════════════════════════════╝');
  console.log(`\n🚀 Server running on http://localhost:${PORT}`);
  console.log(`📊 API endpoints: http://localhost:${PORT}/api`);
  console.log(`❤️  Health check: http://localhost:${PORT}/api/health\n`);
});

// Graceful shutdown
process.on('SIGTERM', async () => {
  console.log('\n🛑 Shutting down gracefully...');
  await db.closePool();
  process.exit(0);
});

process.on('SIGINT', async () => {
  console.log('\n🛑 Shutting down gracefully...');
  await db.closePool();
  process.exit(0);
});
