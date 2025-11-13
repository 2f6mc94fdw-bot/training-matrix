// Database Setup Helper Script
// Run this after configuring your .env file
require('dotenv').config({ path: '../.env' });
const { Pool } = require('pg');
const fs = require('fs');
const path = require('path');

const config = require('./config/database');

async function setupDatabase() {
  console.log('\n╔══════════════════════════════════════════════════╗');
  console.log('║   Training Matrix - Database Setup              ║');
  console.log('╚══════════════════════════════════════════════════╝\n');

  console.log('📋 Configuration:');
  console.log(`   Connection: ${config.connectionString.replace(/:[^:]*@/, ':****@')}`);
  console.log(`   SSL: ${config.ssl ? 'Enabled' : 'Disabled'}\n`);

  // Test connection first
  console.log('🔌 Testing database connection...');
  const pool = new Pool({
    connectionString: config.connectionString,
    ssl: config.ssl
  });

  try {
    const result = await pool.query('SELECT NOW() as now, version()');
    console.log('✅ Connection successful!');
    console.log(`   Time: ${result.rows[0].now}`);
    console.log(`   Version: ${result.rows[0].version.split(' ')[0]} ${result.rows[0].version.split(' ')[1]}\n`);
  } catch (error) {
    console.error('❌ Connection failed!');
    console.error(`   Error: ${error.message}\n`);
    console.log('Please check your .env file configuration.');
    console.log('Connection string format: postgresql://username:password@host:port/database\n');
    process.exit(1);
  }

  // Check if tables exist
  console.log('🔍 Checking database state...');
  try {
    const tableCheck = await pool.query(`
      SELECT table_name
      FROM information_schema.tables
      WHERE table_schema = 'public'
      AND table_name IN ('users', 'engineers', 'production_areas', 'machines')
    `);

    if (tableCheck.rows.length > 0) {
      console.log(`⚠️  Found ${tableCheck.rows.length} existing tables`);
      console.log('   Tables:', tableCheck.rows.map(r => r.table_name).join(', '));

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
      await pool.query(`
        DROP TABLE IF EXISTS audit_logs CASCADE;
        DROP TABLE IF EXISTS snapshots CASCADE;
        DROP TABLE IF EXISTS certifications CASCADE;
        DROP TABLE IF EXISTS core_skill_assessments CASCADE;
        DROP TABLE IF EXISTS core_skills CASCADE;
        DROP TABLE IF EXISTS core_skill_categories CASCADE;
        DROP TABLE IF EXISTS assessments CASCADE;
        DROP TABLE IF EXISTS competencies CASCADE;
        DROP TABLE IF EXISTS machines CASCADE;
        DROP TABLE IF EXISTS production_areas CASCADE;
        DROP TABLE IF EXISTS engineers CASCADE;
        DROP TABLE IF EXISTS users CASCADE;
      `);
      console.log('✅ Old tables dropped');
    } else {
      console.log('✅ Database is empty, ready for setup');
    }

    // Run schema
    console.log('\n📝 Creating database schema...');
    const schemaPath = path.join(__dirname, 'db', 'schema.sql');
    const schema = fs.readFileSync(schemaPath, 'utf8');

    // Split by semicolon and execute each statement
    const statements = schema
      .split(';')
      .map(s => s.trim())
      .filter(s => s.length > 0 && !s.startsWith('--'));

    for (let i = 0; i < statements.length; i++) {
      try {
        await pool.query(statements[i]);
      } catch (error) {
        // Ignore "already exists" errors for idempotency
        if (!error.message.includes('already exists')) {
          console.error(`   Error in statement ${i + 1}:`, error.message);
        }
      }
    }

    console.log('✅ Schema created successfully');

    // Verify tables
    console.log('\n🔍 Verifying installation...');
    const tables = await pool.query(`
      SELECT table_name
      FROM information_schema.tables
      WHERE table_schema = 'public'
      ORDER BY table_name
    `);

    console.log(`✅ Found ${tables.rows.length} tables:`);
    tables.rows.forEach(row => {
      console.log(`   ✓ ${row.table_name}`);
    });

    // Check default data
    console.log('\n📊 Checking default data...');
    const userCount = await pool.query('SELECT COUNT(*) FROM users');
    const coreSkillCount = await pool.query('SELECT COUNT(*) FROM core_skills');

    console.log(`   Users: ${userCount.rows[0].count}`);
    console.log(`   Core Skills: ${coreSkillCount.rows[0].count}`);

    if (userCount.rows[0].count === '0') {
      console.log('\n⚠️  No default admin user found. Creating one...');
      const bcrypt = require('bcryptjs');
      const hashedPassword = await bcrypt.hash('admin123', 10);

      await pool.query(
        `INSERT INTO users (id, username, password, role, engineer_id)
         VALUES ('admin', 'admin', $1, 'admin', NULL)`,
        [hashedPassword]
      );
      console.log('✅ Admin user created (username: admin, password: admin123)');
    }

    console.log('\n╔══════════════════════════════════════════════════╗');
    console.log('║   ✅ Database Setup Complete!                    ║');
    console.log('╚══════════════════════════════════════════════════╝\n');

    console.log('Next steps:');
    console.log('  1. Start the server: npm run server');
    console.log('  2. Start the frontend: npm run dev');
    console.log('  3. Open http://localhost:5173');
    console.log('  4. Login with: admin / admin123');
    console.log('\n⚠️  Remember to change the default password!\n');

  } catch (error) {
    console.error('\n❌ Setup failed:', error.message);
    console.error('Stack:', error.stack);
    process.exit(1);
  } finally {
    await pool.end();
  }
}

// Run setup
setupDatabase().catch(console.error);
