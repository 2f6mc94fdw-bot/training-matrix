// Database Setup Script for SQL Server
// Run this after configuring your .env file
require('dotenv').config({ path: require('path').join(__dirname, '../.env') });
const bcrypt = require('bcryptjs');
const db = require('../database/connection.cjs');
const fs = require('fs');
const path = require('path');

async function setupDatabase() {
  console.log('\n╔══════════════════════════════════════════════════╗');
  console.log('║   Training Matrix - Database Setup              ║');
  console.log('╚══════════════════════════════════════════════════╝\n');

  try {
    // Test connection
    console.log('🔌 Testing database connection...');
    const testResult = await db.testConnection();

    if (!testResult) {
      console.error('❌ Database connection failed!');
      console.log('Please check your .env file configuration.\n');
      process.exit(1);
    }

    // Check if tables exist
    console.log('\n🔍 Checking database state...');
    const pool = await db.getPool();
    const tableCheck = await pool.request().query(`
      SELECT TABLE_NAME
      FROM INFORMATION_SCHEMA.TABLES
      WHERE TABLE_TYPE = 'BASE TABLE'
      AND TABLE_NAME IN ('users', 'engineers', 'production_areas', 'machines')
    `);

    if (tableCheck.recordset.length > 0) {
      console.log(`⚠️  Found ${tableCheck.recordset.length} existing tables`);
      console.log('   Tables:', tableCheck.recordset.map(r => r.TABLE_NAME).join(', '));

      // Ask if user wants to proceed
      const readline = require('readline').createInterface({
        input: process.stdin,
        output: process.stdout
      });

      const answer = await new Promise(resolve => {
        readline.question('\n❓ Do you want to DROP all tables and recreate? (yes/no): ', resolve);
      });
      readline.close();

      if (answer.toLowerCase() !== 'yes') {
        console.log('\n✋ Setup cancelled. Existing tables preserved.');
        process.exit(0);
      }

      console.log('\n🗑️  Dropping existing tables...');
      // Drop in correct order (respecting foreign keys)
      const dropStatements = [
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

      for (const stmt of dropStatements) {
        try {
          await pool.request().query(stmt);
        } catch (err) {
          // Ignore errors for non-existent tables
        }
      }
      console.log('✅ Old tables dropped');
    } else {
      console.log('✅ Database is empty, ready for setup');
    }

    // Run schema
    console.log('\n📝 Creating database schema...');
    const schemaPath = path.join(__dirname, '../database/schema.sql');

    if (!fs.existsSync(schemaPath)) {
      console.error(`❌ Schema file not found: ${schemaPath}`);
      process.exit(1);
    }

    const schema = fs.readFileSync(schemaPath, 'utf8');

    // Split by GO or semicolon and execute each statement
    const statements = schema
      .split(/\bGO\b/i)
      .map(s => s.trim())
      .filter(s => s.length > 0 && !s.startsWith('--'));

    for (let i = 0; i < statements.length; i++) {
      try {
        await pool.request().query(statements[i]);
      } catch (error) {
        // Log error but continue
        if (!error.message.includes('already exists')) {
          console.error(`   Warning in statement ${i + 1}:`, error.message.split('\n')[0]);
        }
      }
    }

    console.log('✅ Schema created successfully');

    // Verify tables
    console.log('\n🔍 Verifying installation...');
    const tables = await pool.request().query(`
      SELECT TABLE_NAME
      FROM INFORMATION_SCHEMA.TABLES
      WHERE TABLE_TYPE = 'BASE TABLE'
      ORDER BY TABLE_NAME
    `);

    console.log(`✅ Found ${tables.recordset.length} tables:`);
    tables.recordset.forEach(row => {
      console.log(`   ✓ ${row.TABLE_NAME}`);
    });

    // Check default data
    console.log('\n📊 Checking default data...');
    const userCount = await pool.request().query('SELECT COUNT(*) as count FROM users');
    const coreSkillCount = await pool.request().query('SELECT COUNT(*) as count FROM core_skills');

    console.log(`   Users: ${userCount.recordset[0].count}`);
    console.log(`   Core Skills: ${coreSkillCount.recordset[0].count}`);

    if (userCount.recordset[0].count === 0) {
      console.log('\n👤 Creating default admin user...');
      const hashedPassword = await bcrypt.hash('admin123', 10);

      await pool.request()
        .input('id', db.sql.VarChar, 'admin')
        .input('username', db.sql.VarChar, 'admin')
        .input('password', db.sql.VarChar, hashedPassword)
        .input('role', db.sql.VarChar, 'admin')
        .query(`
          INSERT INTO users (id, username, password, role, engineer_id)
          VALUES (@id, @username, @password, @role, NULL)
        `);

      console.log('✅ Admin user created');
      console.log('   Username: admin');
      console.log('   Password: admin123');
    }

    console.log('\n╔══════════════════════════════════════════════════╗');
    console.log('║   ✅ Database Setup Complete!                    ║');
    console.log('╚══════════════════════════════════════════════════╝\n');

    console.log('Next steps:');
    console.log('  1. Start the server (if not running):');
    console.log('     cd server && node index.cjs');
    console.log('  2. Start the frontend (in new terminal):');
    console.log('     npm run dev');
    console.log('  3. Open http://localhost:5173');
    console.log('  4. Login with: admin / admin123');
    console.log('\n⚠️  Remember to change the default password!\n');

  } catch (error) {
    console.error('\n❌ Setup failed:', error.message);
    console.error('Stack:', error.stack);
    process.exit(1);
  } finally {
    await db.closePool();
  }
}

// Run setup
setupDatabase().catch(console.error);
