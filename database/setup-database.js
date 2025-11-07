// Database Setup Script
// This script creates the database and runs all schema scripts

const sql = require('mssql');
const fs = require('fs');
const path = require('path');

// Connection config (without database specified for initial connection)
const masterConfig = {
  server: 'localhost',
  user: 'sa',
  password: 'YourStrong@Passw0rd',
  options: {
    encrypt: true,
    trustServerCertificate: true,
    enableArithAbort: true
  }
};

async function setupDatabase() {
  let pool = null;

  try {
    console.log('🚀 Starting database setup...\n');

    // Step 1: Connect to master database
    console.log('📡 Connecting to SQL Server...');
    pool = await sql.connect(masterConfig);
    console.log('✅ Connected!\n');

    // Step 2: Create database if it doesn't exist
    console.log('🗄️  Creating training_matrix database...');
    await pool.request().query(`
      IF NOT EXISTS (SELECT * FROM sys.databases WHERE name = 'training_matrix')
      BEGIN
        CREATE DATABASE training_matrix;
        PRINT 'Database created successfully';
      END
      ELSE
      BEGIN
        PRINT 'Database already exists';
      END
    `);
    console.log('✅ Database ready!\n');

    // Step 3: Close connection to master and reconnect to training_matrix
    await pool.close();
    console.log('📡 Connecting to training_matrix database...');
    const dbConfig = { ...masterConfig, database: 'training_matrix' };
    pool = await sql.connect(dbConfig);
    console.log('✅ Connected!\n');

    // Step 4: Run schema.sql
    console.log('📋 Creating database schema...');
    const schemaPath = path.join(__dirname, 'schema.sql');
    const schemaSQL = fs.readFileSync(schemaPath, 'utf8');

    // Split by GO statements and execute each batch
    const batches = schemaSQL.split(/\nGO\s*\n/i).filter(batch => batch.trim());

    for (let i = 0; i < batches.length; i++) {
      const batch = batches[i].trim();
      if (batch) {
        await pool.request().query(batch);
      }
    }
    console.log('✅ Schema created successfully!\n');

    // Step 5: Install core skills
    console.log('🎓 Installing core skills data...');
    const coreSkillsPath = path.join(__dirname, 'core-skills');
    const skillFiles = [
      'mechanical-skills.sql',
      'electrical-skills.sql',
      'software-skills.sql'
    ];

    for (const file of skillFiles) {
      const filePath = path.join(coreSkillsPath, file);
      if (fs.existsSync(filePath)) {
        console.log(`   📄 Loading ${file}...`);
        const sqlContent = fs.readFileSync(filePath, 'utf8');
        const batches = sqlContent.split(/\nGO\s*\n/i).filter(batch => batch.trim());

        for (const batch of batches) {
          if (batch.trim()) {
            await pool.request().query(batch);
          }
        }
      }
    }
    console.log('✅ Core skills installed!\n');

    // Step 6: Install production areas
    console.log('🏭 Installing production areas data...');
    const productionAreasPath = path.join(__dirname, 'production-areas');
    const areaFiles = [
      'line7-tank-room.sql',
      'line7-filling.sql',
      'line7-packing.sql'
    ];

    for (const file of areaFiles) {
      const filePath = path.join(productionAreasPath, file);
      if (fs.existsSync(filePath)) {
        console.log(`   📄 Loading ${file}...`);
        const sqlContent = fs.readFileSync(filePath, 'utf8');
        const batches = sqlContent.split(/\nGO\s*\n/i).filter(batch => batch.trim());

        for (const batch of batches) {
          if (batch.trim()) {
            await pool.request().query(batch);
          }
        }
      }
    }
    console.log('✅ Production areas installed!\n');

    // Step 7: Verify setup
    console.log('🔍 Verifying database setup...');
    const tables = await pool.request().query(`
      SELECT TABLE_NAME
      FROM INFORMATION_SCHEMA.TABLES
      WHERE TABLE_TYPE = 'BASE TABLE'
      ORDER BY TABLE_NAME
    `);

    console.log(`   ✅ ${tables.recordset.length} tables created`);

    const skillCount = await pool.request().query('SELECT COUNT(*) as count FROM core_skills');
    console.log(`   ✅ ${skillCount.recordset[0].count} core skills loaded`);

    const areaCount = await pool.request().query('SELECT COUNT(*) as count FROM production_areas');
    console.log(`   ✅ ${areaCount.recordset[0].count} production areas loaded`);

    console.log('\n🎉 DATABASE SETUP COMPLETE! 🎉\n');
    console.log('You can now start the application with: npm run dev\n');
    console.log('Default login credentials:');
    console.log('   Username: admin');
    console.log('   Password: admin123\n');

  } catch (error) {
    console.error('❌ Setup failed:', error.message);
    console.error('\nTroubleshooting:');
    console.error('1. Make sure Docker is running');
    console.error('2. Make sure SQL Server container is running: docker ps');
    console.error('3. Check the password in database/config.js matches the container password');
    throw error;
  } finally {
    if (pool) {
      await pool.close();
    }
  }
}

// Run the setup
setupDatabase()
  .then(() => {
    console.log('✅ All done!');
    process.exit(0);
  })
  .catch((error) => {
    console.error('❌ Setup failed:', error);
    process.exit(1);
  });
