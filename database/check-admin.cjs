const sql = require('mssql');

const config = {
  server: 'localhost',
  database: 'training_matrix',
  user: 'sa',
  password: 'YourStrong@Passw0rd',
  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};

async function checkAdmin() {
  try {
    await sql.connect(config);
    console.log('✅ Connected to database\n');

    const result = await sql.query(`
      SELECT id, username, role, created_at
      FROM users
      WHERE username = 'admin'
    `);

    if (result.recordset.length > 0) {
      console.log('Admin user found:');
      console.log(result.recordset[0]);
    } else {
      console.log('❌ Admin user NOT found in database!');
    }

    await sql.close();
  } catch (err) {
    console.error('Error:', err.message);
    await sql.close();
  }
}

checkAdmin();
