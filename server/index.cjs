// Training Matrix API Server
require('dotenv').config();
const express = require('express');
const cors = require('cors');
const bcrypt = require('bcryptjs');
const db = require('./db/connection');

const app = express();
const PORT = process.env.PORT || 3001;

// Middleware
app.use(cors({
  origin: process.env.FRONTEND_URL || 'http://localhost:5173',
  credentials: true
}));
app.use(express.json({ limit: '50mb' }));
app.use(express.urlencoded({ extended: true, limit: '50mb' }));

// Logging middleware
app.use((req, res, next) => {
  console.log(`${new Date().toISOString()} - ${req.method} ${req.path}`);
  next();
});

// ==================== HEALTH CHECK ====================
app.get('/api/health', async (req, res) => {
  const dbStatus = await db.testConnection();
  res.json({
    status: 'ok',
    database: dbStatus ? 'connected' : 'disconnected',
    timestamp: new Date().toISOString()
  });
});

// ==================== AUTH ====================
app.post('/api/auth/login', async (req, res) => {
  try {
    const { username, password } = req.body;

    const result = await db.query(
      'SELECT * FROM users WHERE username = $1',
      [username]
    );

    if (result.rows.length === 0) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    const user = result.rows[0];
    const validPassword = bcrypt.compareSync(password, user.password);

    if (!validPassword) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    // Don't send password back
    delete user.password;

    res.json({ user });
  } catch (error) {
    console.error('Login error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== USERS ====================
app.get('/api/users', async (req, res) => {
  try {
    const result = await db.query('SELECT id, username, role, engineer_id FROM users ORDER BY username');
    res.json(result.rows);
  } catch (error) {
    console.error('Get users error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/users', async (req, res) => {
  try {
    const { id, username, password, role, engineerId } = req.body;
    const hashedPassword = bcrypt.hashSync(password, 10);

    const result = await db.query(
      'INSERT INTO users (id, username, password, role, engineer_id) VALUES ($1, $2, $3, $4, $5) RETURNING id, username, role, engineer_id',
      [id, username, hashedPassword, role, engineerId]
    );

    res.json(result.rows[0]);
  } catch (error) {
    console.error('Create user error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.put('/api/users/:id/password', async (req, res) => {
  try {
    const { id } = req.params;
    const { password } = req.body;
    const hashedPassword = bcrypt.hashSync(password, 10);

    await db.query(
      'UPDATE users SET password = $1, updated_at = CURRENT_TIMESTAMP WHERE id = $2',
      [hashedPassword, id]
    );

    res.json({ success: true });
  } catch (error) {
    console.error('Reset password error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.delete('/api/users/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await db.query('DELETE FROM users WHERE id = $1', [id]);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete user error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== ENGINEERS ====================
app.get('/api/engineers', async (req, res) => {
  try {
    const result = await db.query('SELECT * FROM engineers ORDER BY name');
    res.json(result.rows);
  } catch (error) {
    console.error('Get engineers error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/engineers', async (req, res) => {
  try {
    const { id, name, shift } = req.body;
    const result = await db.query(
      'INSERT INTO engineers (id, name, shift) VALUES ($1, $2, $3) RETURNING *',
      [id, name, shift]
    );
    res.json(result.rows[0]);
  } catch (error) {
    console.error('Create engineer error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.put('/api/engineers/:id', async (req, res) => {
  try {
    const { id } = req.params;
    const { name, shift } = req.body;
    const result = await db.query(
      'UPDATE engineers SET name = $1, shift = $2, updated_at = CURRENT_TIMESTAMP WHERE id = $3 RETURNING *',
      [name, shift, id]
    );
    res.json(result.rows[0]);
  } catch (error) {
    console.error('Update engineer error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.delete('/api/engineers/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await db.query('DELETE FROM engineers WHERE id = $1', [id]);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete engineer error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== PRODUCTION AREAS ====================
app.get('/api/production-areas', async (req, res) => {
  try {
    // Get all production areas with their machines and competencies
    const areas = await db.query('SELECT * FROM production_areas ORDER BY id');

    for (const area of areas.rows) {
      const machines = await db.query(
        'SELECT * FROM machines WHERE production_area_id = $1 ORDER BY id',
        [area.id]
      );

      for (const machine of machines.rows) {
        const competencies = await db.query(
          'SELECT * FROM competencies WHERE machine_id = $1 ORDER BY id',
          [machine.id]
        );
        machine.competencies = competencies.rows;
      }

      area.machines = machines.rows;
    }

    res.json(areas.rows);
  } catch (error) {
    console.error('Get production areas error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/production-areas', async (req, res) => {
  try {
    const { name, machines } = req.body;

    await db.transaction(async (client) => {
      // Insert production area
      const areaResult = await client.query(
        'INSERT INTO production_areas (name) VALUES ($1) RETURNING *',
        [name]
      );
      const area = areaResult.rows[0];

      // Insert machines and competencies
      if (machines && machines.length > 0) {
        for (const machine of machines) {
          const machineResult = await client.query(
            'INSERT INTO machines (production_area_id, name, importance) VALUES ($1, $2, $3) RETURNING *',
            [area.id, machine.name, machine.importance || 1]
          );
          const machineId = machineResult.rows[0].id;

          if (machine.competencies && machine.competencies.length > 0) {
            for (const comp of machine.competencies) {
              await client.query(
                'INSERT INTO competencies (machine_id, name, max_score) VALUES ($1, $2, $3)',
                [machineId, comp.name, comp.maxScore || 3]
              );
            }
          }
        }
      }

      return area;
    });

    res.json({ success: true });
  } catch (error) {
    console.error('Create production area error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.delete('/api/production-areas/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await db.query('DELETE FROM production_areas WHERE id = $1', [id]);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete production area error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== ASSESSMENTS ====================
app.get('/api/assessments', async (req, res) => {
  try {
    const result = await db.query('SELECT * FROM assessments');

    // Convert to the format expected by frontend: { engineerId-areaId-machineId-compId: score }
    const assessments = {};
    result.rows.forEach(row => {
      const key = `${row.engineer_id}-${row.production_area_id}-${row.machine_id}-${row.competency_id}`;
      assessments[key] = row.score;
    });

    res.json(assessments);
  } catch (error) {
    console.error('Get assessments error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/assessments', async (req, res) => {
  try {
    const { engineerId, areaId, machineId, competencyId, score } = req.body;

    await db.query(
      `INSERT INTO assessments (engineer_id, production_area_id, machine_id, competency_id, score)
       VALUES ($1, $2, $3, $4, $5)
       ON CONFLICT (engineer_id, production_area_id, machine_id, competency_id)
       DO UPDATE SET score = $5, updated_at = CURRENT_TIMESTAMP`,
      [engineerId, areaId, machineId, competencyId, score]
    );

    res.json({ success: true });
  } catch (error) {
    console.error('Save assessment error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== CORE SKILLS ====================
app.get('/api/core-skills', async (req, res) => {
  try {
    const categories = await db.query('SELECT * FROM core_skill_categories ORDER BY id');

    for (const category of categories.rows) {
      const skills = await db.query(
        'SELECT * FROM core_skills WHERE category_id = $1 ORDER BY id',
        [category.id]
      );
      category.skills = skills.rows;
    }

    res.json({ categories: categories.rows });
  } catch (error) {
    console.error('Get core skills error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.get('/api/core-skills/assessments', async (req, res) => {
  try {
    const result = await db.query('SELECT * FROM core_skill_assessments');

    // Convert to format: { engineerId-categoryId-skillId: score }
    const assessments = {};
    result.rows.forEach(row => {
      const key = `${row.engineer_id}-${row.category_id}-${row.skill_id}`;
      assessments[key] = row.score;
    });

    res.json(assessments);
  } catch (error) {
    console.error('Get core skill assessments error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/core-skills/assessments', async (req, res) => {
  try {
    const { engineerId, categoryId, skillId, score } = req.body;

    await db.query(
      `INSERT INTO core_skill_assessments (engineer_id, category_id, skill_id, score)
       VALUES ($1, $2, $3, $4)
       ON CONFLICT (engineer_id, category_id, skill_id)
       DO UPDATE SET score = $4, updated_at = CURRENT_TIMESTAMP`,
      [engineerId, categoryId, skillId, score]
    );

    res.json({ success: true });
  } catch (error) {
    console.error('Save core skill assessment error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== CERTIFICATIONS ====================
app.get('/api/certifications', async (req, res) => {
  try {
    const result = await db.query('SELECT * FROM certifications ORDER BY date_earned DESC');
    res.json(result.rows);
  } catch (error) {
    console.error('Get certifications error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/certifications', async (req, res) => {
  try {
    const { engineerId, name, dateEarned, expiryDate } = req.body;
    const result = await db.query(
      'INSERT INTO certifications (engineer_id, name, date_earned, expiry_date) VALUES ($1, $2, $3, $4) RETURNING *',
      [engineerId, name, dateEarned, expiryDate || null]
    );
    res.json(result.rows[0]);
  } catch (error) {
    console.error('Create certification error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.delete('/api/certifications/:id', async (req, res) => {
  try {
    const { id } = req.params;
    await db.query('DELETE FROM certifications WHERE id = $1', [id]);
    res.json({ success: true });
  } catch (error) {
    console.error('Delete certification error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== SNAPSHOTS ====================
app.get('/api/snapshots', async (req, res) => {
  try {
    const result = await db.query('SELECT * FROM snapshots ORDER BY timestamp DESC LIMIT 50');
    res.json(result.rows);
  } catch (error) {
    console.error('Get snapshots error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/snapshots', async (req, res) => {
  try {
    const { id, description, timestamp, data } = req.body;
    await db.query(
      'INSERT INTO snapshots (id, description, timestamp, data) VALUES ($1, $2, $3, $4)',
      [id, description, timestamp, JSON.stringify(data)]
    );
    res.json({ success: true });
  } catch (error) {
    console.error('Create snapshot error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== AUDIT LOGS ====================
app.get('/api/audit-logs', async (req, res) => {
  try {
    const result = await db.query('SELECT * FROM audit_logs ORDER BY timestamp DESC LIMIT 1000');
    res.json(result.rows);
  } catch (error) {
    console.error('Get audit logs error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

app.post('/api/audit-logs', async (req, res) => {
  try {
    const { id, timestamp, user, action, details } = req.body;
    await db.query(
      'INSERT INTO audit_logs (id, timestamp, user_id, action, details) VALUES ($1, $2, $3, $4, $5)',
      [id, timestamp, user, action, details]
    );
    res.json({ success: true });
  } catch (error) {
    console.error('Create audit log error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// ==================== BULK DATA EXPORT/IMPORT ====================
app.get('/api/data/export', async (req, res) => {
  try {
    const [
      engineers,
      users,
      productionAreas,
      assessments,
      coreSkills,
      coreSkillAssessments,
      certifications,
      snapshots
    ] = await Promise.all([
      db.query('SELECT * FROM engineers'),
      db.query('SELECT id, username, role, engineer_id FROM users'),
      getAllProductionAreas(),
      db.query('SELECT * FROM assessments'),
      getCoreSkillsData(),
      db.query('SELECT * FROM core_skill_assessments'),
      db.query('SELECT * FROM certifications'),
      db.query('SELECT * FROM snapshots')
    ]);

    const data = {
      engineers: engineers.rows,
      users: users.rows,
      productionAreas: productionAreas,
      assessments: convertAssessmentsToObject(assessments.rows),
      coreSkills: coreSkills,
      coreSkillAssessments: convertCoreSkillAssessmentsToObject(coreSkillAssessments.rows),
      certifications: certifications.rows,
      snapshots: snapshots.rows,
      exportDate: new Date().toISOString(),
      version: '2.0.0'
    };

    res.json(data);
  } catch (error) {
    console.error('Export error:', error);
    res.status(500).json({ error: 'Server error' });
  }
});

// Helper functions
async function getAllProductionAreas() {
  const areas = await db.query('SELECT * FROM production_areas');

  for (const area of areas.rows) {
    const machines = await db.query(
      'SELECT * FROM machines WHERE production_area_id = $1',
      [area.id]
    );

    for (const machine of machines.rows) {
      const competencies = await db.query(
        'SELECT * FROM competencies WHERE machine_id = $1',
        [machine.id]
      );
      machine.competencies = competencies.rows;
    }

    area.machines = machines.rows;
  }

  return areas.rows;
}

async function getCoreSkillsData() {
  const categories = await db.query('SELECT * FROM core_skill_categories');

  for (const category of categories.rows) {
    const skills = await db.query(
      'SELECT * FROM core_skills WHERE category_id = $1',
      [category.id]
    );
    category.skills = skills.rows;
  }

  return { categories: categories.rows };
}

function convertAssessmentsToObject(rows) {
  const assessments = {};
  rows.forEach(row => {
    const key = `${row.engineer_id}-${row.production_area_id}-${row.machine_id}-${row.competency_id}`;
    assessments[key] = row.score;
  });
  return assessments;
}

function convertCoreSkillAssessmentsToObject(rows) {
  const assessments = {};
  rows.forEach(row => {
    const key = `${row.engineer_id}-${row.category_id}-${row.skill_id}`;
    assessments[key] = row.score;
  });
  return assessments;
}

// ==================== START SERVER ====================
const startServer = async () => {
  try {
    // Test database connection
    const connected = await db.testConnection();
    if (!connected) {
      console.error('❌ Failed to connect to database. Check your configuration.');
      console.error('   Connection string:', process.env.DB_CONNECTION_STRING || 'Not set');
      process.exit(1);
    }

    app.listen(PORT, () => {
      console.log('');
      console.log('╔═══════════════════════════════════════════════════╗');
      console.log('║   Training Matrix API Server                      ║');
      console.log('╚═══════════════════════════════════════════════════╝');
      console.log('');
      console.log(`🚀 Server running on: http://localhost:${PORT}`);
      console.log(`📊 Health check: http://localhost:${PORT}/api/health`);
      console.log(`🗄️  Database: Connected`);
      console.log('');
      console.log('Available endpoints:');
      console.log('  - POST /api/auth/login');
      console.log('  - GET  /api/engineers');
      console.log('  - GET  /api/production-areas');
      console.log('  - GET  /api/assessments');
      console.log('  - GET  /api/core-skills');
      console.log('  - GET  /api/data/export');
      console.log('');
      console.log('Press Ctrl+C to stop');
      console.log('');
    });
  } catch (error) {
    console.error('❌ Failed to start server:', error);
    process.exit(1);
  }
};

startServer();
