// Simple Login Test Script
// Run this to test if login works: node test-login.js

const bcrypt = require('bcryptjs');
const sql = require('mssql');
const config = require('./database/config.cjs');

async function testLogin() {
  console.log('\n=== LOGIN TEST ===\n');

  try {
    // 1. Test database connection
    console.log('1. Testing database connection...');
    const pool = await sql.connect(config);
    console.log('   ✅ Database connected\n');

    // 2. Check if admin user exists
    console.log('2. Looking for admin user...');
    const result = await pool.request()
      .input('username', sql.VarChar, 'admin')
      .query('SELECT * FROM users WHERE username = @username');

    if (result.recordset.length === 0) {
      console.log('   ❌ Admin user NOT FOUND in database!');
      console.log('   Run: node server/setup-database-simple.js');
      process.exit(1);
    }

    const user = result.recordset[0];
    console.log('   ✅ Admin user found');
    console.log('   User ID:', user.id);
    console.log('   Username:', user.username);
    console.log('   Role:', user.role);
    console.log('   Password hash:', user.password.substring(0, 20) + '...\n');

    // 3. Test password
    console.log('3. Testing password "admin123"...');
    const validPassword = await bcrypt.compare('admin123', user.password);

    if (!validPassword) {
      console.log('   ❌ Password does NOT match!');
      console.log('   The stored hash doesn\'t match "admin123"');
      console.log('   Run: node server/setup-database-simple.js');
      process.exit(1);
    }

    console.log('   ✅ Password matches!\n');

    // 4. Test what login would return
    console.log('4. Testing login response...');
    delete user.password;
    const loginResponse = { user };
    console.log('   Login would return:', JSON.stringify(loginResponse, null, 2));
    console.log();

    console.log('=== ALL TESTS PASSED ===');
    console.log('\nDatabase authentication works correctly!');
    console.log('If login still fails in the browser, the issue is:');
    console.log('  - Backend server not running (node server/index.cjs)');
    console.log('  - Frontend server not running (npm run dev)');
    console.log('  - Network/CORS issue');
    console.log('  - Check browser console (F12) for errors\n');

    await pool.close();

  } catch (error) {
    console.error('\n❌ TEST FAILED:', error.message);
    console.error('\nFull error:', error);
    process.exit(1);
  }
}

testLogin();
