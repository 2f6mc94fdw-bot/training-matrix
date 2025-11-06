// Test SQL Server Connection
// Run this script to verify your database connection works
// Usage: node database/test-connection.js

const db = require('./connection');

console.log('\n╔══════════════════════════════════════════════════╗');
console.log('║   Training Matrix - SQL Server Connection Test  ║');
console.log('╚══════════════════════════════════════════════════╝\n');

async function testConnection() {
  try {
    console.log('📋 Testing connection...');

    // Test basic connection
    const success = await db.testConnection();

    if (!success) {
      console.log('\n❌ Connection test failed!');
      console.log('   Check your database/config.js file\n');
      process.exit(1);
    }

    console.log('\n✅ Connection successful!\n');

    // Test querying tables
    console.log('📊 Checking database tables...');

    const result = await db.query(`
      SELECT TABLE_NAME
      FROM INFORMATION_SCHEMA.TABLES
      WHERE TABLE_TYPE = 'BASE TABLE'
        AND TABLE_SCHEMA = 'dbo'
      ORDER BY TABLE_NAME
    `);

    if (result.recordset.length === 0) {
      console.log('⚠️  No tables found in database');
      console.log('   You need to run the schema.sql script first:\n');
      console.log('   1. Open SQL Server Management Studio (SSMS)');
      console.log('   2. Connect to your server');
      console.log('   3. Open database/schema.sql');
      console.log('   4. Execute the script\n');
    } else {
      console.log(`✅ Found ${result.recordset.length} tables:`);
      result.recordset.forEach(row => {
        console.log(`   ✓ ${row.TABLE_NAME}`);
      });
    }

    // Test for default data
    console.log('\n📊 Checking default data...');

    const usersResult = await db.query('SELECT COUNT(*) as count FROM users');
    const usersCount = usersResult.recordset[0].count;

    console.log(`   Users: ${usersCount}`);

    if (usersCount === 0) {
      console.log('   ⚠️  No users found - schema may not be initialized');
    } else {
      console.log('   ✅ Default admin user exists');
    }

    const coreSkillsResult = await db.query('SELECT COUNT(*) as count FROM core_skills');
    const coreSkillsCount = coreSkillsResult.recordset[0].count;

    console.log(`   Core Skills: ${coreSkillsCount}`);

    if (coreSkillsCount === 0) {
      console.log('   ⚠️  No core skills found - schema may not be initialized');
    } else {
      console.log('   ✅ Core skills data loaded');
    }

    console.log('\n╔══════════════════════════════════════════════════╗');
    console.log('║   ✅ All tests passed!                           ║');
    console.log('╚══════════════════════════════════════════════════╝\n');

    console.log('Next steps:');
    console.log('  1. If tables are missing, run database/schema.sql');
    console.log('  2. Start the application: npm run dev');
    console.log('  3. Login with: admin / admin123\n');

  } catch (error) {
    console.error('\n❌ Error during connection test:', error.message);
    console.error('\nCommon issues:');
    console.error('  • SQL Server is not running');
    console.error('  • Firewall is blocking the connection');
    console.error('  • Incorrect server name/IP address');
    console.error('  • Incorrect username/password');
    console.error('  • Database does not exist');
    console.error('  • TCP/IP protocol is not enabled\n');
    console.error('Check your database/config.js file and try again.\n');
    process.exit(1);
  } finally {
    await db.closePool();
  }
}

// Run the test
testConnection();
