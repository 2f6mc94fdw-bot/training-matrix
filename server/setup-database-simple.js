// Simplified Database Setup Script for SQL Server
require('dotenv').config({ path: require('path').join(__dirname, '../.env') });
const bcrypt = require('bcryptjs');
const sql = require('mssql');
const config = require('../database/config.cjs');

async function setupDatabase() {
  console.log('\n╔══════════════════════════════════════════════════╗');
  console.log('║   Training Matrix - Database Setup              ║');
  console.log('╚══════════════════════════════════════════════════╝\n');

  let pool;

  try {
    // Connect
    console.log('🔌 Connecting to SQL Server...');
    pool = await sql.connect(config);
    console.log('✅ Connected!\n');

    // Drop existing tables
    console.log('🗑️  Dropping existing tables...');
    const dropTables = [
      'DROP TABLE IF EXISTS audit_logs',
      'DROP TABLE IF EXISTS snapshots',
      'DROP TABLE IF EXISTS certifications',
      'DROP TABLE IF EXISTS core_skill_assessments',
      'DROP TABLE IF EXISTS core_skills',
      'DROP TABLE IF EXISTS core_skill_categories',
      'DROP TABLE IF EXISTS assessments',
      'DROP TABLE IF EXISTS competencies',
      'DROP TABLE IF EXISTS machines',
      'DROP TABLE IF EXISTS production_areas',
      'DROP TABLE IF EXISTS engineers',
      'DROP TABLE IF EXISTS users'
    ];

    for (const drop of dropTables) {
      try {
        await pool.request().query(drop);
      } catch (e) {
        // Ignore errors for non-existent tables
      }
    }
    console.log('✅ Tables dropped\n');

    // Create tables
    console.log('📝 Creating tables...');

    // Users
    await pool.request().query(`
      CREATE TABLE users (
        id NVARCHAR(50) PRIMARY KEY,
        username NVARCHAR(100) UNIQUE NOT NULL,
        password NVARCHAR(255) NOT NULL,
        role NVARCHAR(20) NOT NULL CHECK (role IN ('admin', 'engineer')),
        engineer_id NVARCHAR(50),
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE()
      )
    `);
    console.log('   ✓ users');

    // Engineers
    await pool.request().query(`
      CREATE TABLE engineers (
        id NVARCHAR(50) PRIMARY KEY,
        name NVARCHAR(200) NOT NULL,
        shift NVARCHAR(50) NOT NULL,
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE()
      )
    `);
    console.log('   ✓ engineers');

    // Production areas
    await pool.request().query(`
      CREATE TABLE production_areas (
        id INT IDENTITY(1,1) PRIMARY KEY,
        name NVARCHAR(200) NOT NULL,
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE()
      )
    `);
    console.log('   ✓ production_areas');

    // Machines
    await pool.request().query(`
      CREATE TABLE machines (
        id INT IDENTITY(1,1) PRIMARY KEY,
        production_area_id INT NOT NULL,
        name NVARCHAR(200) NOT NULL,
        importance INT DEFAULT 1,
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE(),
        CONSTRAINT FK_machines_production_areas FOREIGN KEY (production_area_id)
          REFERENCES production_areas(id) ON DELETE CASCADE
      )
    `);
    console.log('   ✓ machines');

    // Competencies
    await pool.request().query(`
      CREATE TABLE competencies (
        id INT IDENTITY(1,1) PRIMARY KEY,
        machine_id INT NOT NULL,
        name NVARCHAR(200) NOT NULL,
        max_score INT NOT NULL DEFAULT 3,
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE(),
        CONSTRAINT FK_competencies_machines FOREIGN KEY (machine_id)
          REFERENCES machines(id) ON DELETE CASCADE
      )
    `);
    console.log('   ✓ competencies');

    // Assessments
    await pool.request().query(`
      CREATE TABLE assessments (
        id INT IDENTITY(1,1) PRIMARY KEY,
        engineer_id NVARCHAR(50) NOT NULL,
        production_area_id INT NOT NULL,
        machine_id INT NOT NULL,
        competency_id INT NOT NULL,
        score INT NOT NULL DEFAULT 0,
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE(),
        CONSTRAINT FK_assessments_engineers FOREIGN KEY (engineer_id)
          REFERENCES engineers(id) ON DELETE CASCADE,
        CONSTRAINT FK_assessments_production_areas FOREIGN KEY (production_area_id)
          REFERENCES production_areas(id) ON DELETE CASCADE,
        CONSTRAINT FK_assessments_machines FOREIGN KEY (machine_id)
          REFERENCES machines(id),
        CONSTRAINT FK_assessments_competencies FOREIGN KEY (competency_id)
          REFERENCES competencies(id)
      )
    `);
    console.log('   ✓ assessments');

    // Core skill categories
    await pool.request().query(`
      CREATE TABLE core_skill_categories (
        id NVARCHAR(50) PRIMARY KEY,
        name NVARCHAR(200) NOT NULL,
        created_at DATETIME DEFAULT GETDATE()
      )
    `);
    console.log('   ✓ core_skill_categories');

    // Core skills
    await pool.request().query(`
      CREATE TABLE core_skills (
        id NVARCHAR(50) PRIMARY KEY,
        category_id NVARCHAR(50) NOT NULL,
        name NVARCHAR(200) NOT NULL,
        max_score INT NOT NULL DEFAULT 3,
        created_at DATETIME DEFAULT GETDATE(),
        CONSTRAINT FK_core_skills_categories FOREIGN KEY (category_id)
          REFERENCES core_skill_categories(id) ON DELETE CASCADE
      )
    `);
    console.log('   ✓ core_skills');

    // Core skill assessments
    await pool.request().query(`
      CREATE TABLE core_skill_assessments (
        id INT IDENTITY(1,1) PRIMARY KEY,
        engineer_id NVARCHAR(50) NOT NULL,
        category_id NVARCHAR(50) NOT NULL,
        skill_id NVARCHAR(50) NOT NULL,
        score INT NOT NULL DEFAULT 0,
        created_at DATETIME DEFAULT GETDATE(),
        updated_at DATETIME DEFAULT GETDATE(),
        CONSTRAINT FK_core_assessments_engineers FOREIGN KEY (engineer_id)
          REFERENCES engineers(id) ON DELETE CASCADE,
        CONSTRAINT FK_core_assessments_categories FOREIGN KEY (category_id)
          REFERENCES core_skill_categories(id),
        CONSTRAINT FK_core_assessments_skills FOREIGN KEY (skill_id)
          REFERENCES core_skills(id),
        CONSTRAINT UQ_core_skill_assessments UNIQUE (engineer_id, category_id, skill_id)
      )
    `);
    console.log('   ✓ core_skill_assessments');

    // Certifications
    await pool.request().query(`
      CREATE TABLE certifications (
        id INT IDENTITY(1,1) PRIMARY KEY,
        engineer_id NVARCHAR(50) NOT NULL,
        name NVARCHAR(200) NOT NULL,
        issue_date DATE,
        expiry_date DATE,
        created_at DATETIME DEFAULT GETDATE(),
        CONSTRAINT FK_certifications_engineers FOREIGN KEY (engineer_id)
          REFERENCES engineers(id) ON DELETE CASCADE
      )
    `);
    console.log('   ✓ certifications');

    // Snapshots
    await pool.request().query(`
      CREATE TABLE snapshots (
        id INT IDENTITY(1,1) PRIMARY KEY,
        name NVARCHAR(200) NOT NULL,
        data NVARCHAR(MAX) NOT NULL,
        created_at DATETIME DEFAULT GETDATE(),
        created_by NVARCHAR(50)
      )
    `);
    console.log('   ✓ snapshots');

    // Audit logs
    await pool.request().query(`
      CREATE TABLE audit_logs (
        id INT IDENTITY(1,1) PRIMARY KEY,
        user_id NVARCHAR(50),
        action NVARCHAR(100) NOT NULL,
        details NVARCHAR(MAX),
        created_at DATETIME DEFAULT GETDATE()
      )
    `);
    console.log('   ✓ audit_logs');

    console.log('\n✅ All tables created!\n');

    // Create admin user
    console.log('👤 Creating admin user...');
    const hashedPassword = await bcrypt.hash('admin123', 10);

    await pool.request()
      .input('id', sql.VarChar, 'admin')
      .input('username', sql.VarChar, 'admin')
      .input('password', sql.VarChar, hashedPassword)
      .input('role', sql.VarChar, 'admin')
      .query(`
        INSERT INTO users (id, username, password, role, engineer_id)
        VALUES (@id, @username, @password, @role, NULL)
      `);

    console.log('✅ Admin user created');
    console.log('   Username: admin');
    console.log('   Password: admin123\n');

    console.log('╔══════════════════════════════════════════════════╗');
    console.log('║   ✅ SETUP COMPLETE!                             ║');
    console.log('╚══════════════════════════════════════════════════╝\n');
    console.log('You can now login with: admin / admin123\n');

  } catch (error) {
    console.error('\n❌ Setup failed:', error.message);
    console.error(error.stack);
    process.exit(1);
  } finally {
    if (pool) {
      await pool.close();
    }
  }
}

setupDatabase().catch(console.error);
