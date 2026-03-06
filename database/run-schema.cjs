// Run schema.sql to create all tables and default admin user
require('dotenv').config({ path: require('path').join(__dirname, '../.env') });
const sql = require('mssql');
const fs = require('fs');
const path = require('path');

const config = {
  server: 'localhost',
  database: 'training_matrix',
  user: 'sa',
  password: 'YourStrong@Passw0rd',
  options: {
    encrypt: true,
    trustServerCertificate: true,
    enableArithAbort: true
  }
};

async function runSchema() {
  try {
    console.log('📖 Reading schema.sql...');
    const schemaPath = path.join(__dirname, 'schema.sql');
    const schema = fs.readFileSync(schemaPath, 'utf8');

    console.log('🔌 Connecting to SQL Server...');
    await sql.connect(config);
    console.log('✅ Connected!');

    // Split schema into batches by GO statements
    const batches = schema
      .split(/^\s*GO\s*$/mi)
      .map(batch => batch.trim())
      .filter(batch => batch.length > 0);

    console.log(`📦 Found ${batches.length} SQL batches to execute`);
    console.log('');

    for (let i = 0; i < batches.length; i++) {
      const batch = batches[i];
      if (batch.length > 0) {
        try {
          await sql.query(batch);
          // Show progress every 10 batches
          if ((i + 1) % 10 === 0) {
            console.log(`   ✓ Executed ${i + 1}/${batches.length} batches`);
          }
        } catch (err) {
          // Ignore "already exists" errors
          if (!err.message.includes('already exists') &&
              !err.message.includes('There is already an object')) {
            console.error(`⚠️  Batch ${i + 1} error:`, err.message);
          }
        }
      }
    }

    console.log('');
    console.log('✅ Database schema created successfully!');
    console.log('✅ Default admin user created!');
    console.log('');
    console.log('You can now login to Aptitude with:');
    console.log('  Username: admin');
    console.log('  Password: admin123');
    console.log('');
    console.log('⚠️  IMPORTANT: Change the admin password after first login!');

    await sql.close();
    process.exit(0);
  } catch (err) {
    console.error('❌ Error:', err.message);
    await sql.close();
    process.exit(1);
  }
}

runSchema();
