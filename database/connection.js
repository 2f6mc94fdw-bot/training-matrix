// SQL Server Database Connection Module
const sql = require('mssql');
const config = require('./config');

let pool = null;

// Initialize connection pool
async function getPool() {
  if (pool) {
    return pool;
  }

  try {
    console.log('🔌 Connecting to SQL Server...');

    // Create connection pool
    pool = await sql.connect(config);

    console.log('✅ Connected to SQL Server');
    console.log(`   Server: ${config.server}`);
    console.log(`   Database: ${config.database}`);

    return pool;
  } catch (error) {
    console.error('❌ SQL Server connection failed:', error.message);
    console.error('   Check your database/config.js file');
    throw error;
  }
}

// Execute a query
async function query(queryText, params = []) {
  try {
    const pool = await getPool();
    const request = pool.request();

    // Add parameters to the request
    params.forEach((param, index) => {
      request.input(`param${index}`, param);
    });

    const result = await request.query(queryText);
    return result;
  } catch (error) {
    console.error('❌ Query error:', error.message);
    console.error('   Query:', queryText);
    throw error;
  }
}

// Execute a stored procedure
async function execute(procedureName, params = {}) {
  try {
    const pool = await getPool();
    const request = pool.request();

    // Add parameters
    for (const [key, value] of Object.entries(params)) {
      request.input(key, value);
    }

    const result = await request.execute(procedureName);
    return result;
  } catch (error) {
    console.error('❌ Stored procedure error:', error.message);
    console.error('   Procedure:', procedureName);
    throw error;
  }
}

// Test the connection
async function testConnection() {
  try {
    const pool = await getPool();
    const result = await pool.request().query('SELECT GETDATE() as CurrentTime, @@VERSION as Version');

    console.log('✅ Connection test successful');
    console.log('   Time:', result.recordset[0].CurrentTime);
    console.log('   Version:', result.recordset[0].Version.split('\n')[0]);

    return true;
  } catch (error) {
    console.error('❌ Connection test failed:', error.message);
    return false;
  }
}

// Close the connection pool
async function closePool() {
  if (pool) {
    await pool.close();
    pool = null;
    console.log('🔌 SQL Server connection closed');
  }
}

// Handle process termination
process.on('SIGINT', async () => {
  await closePool();
  process.exit(0);
});

process.on('SIGTERM', async () => {
  await closePool();
  process.exit(0);
});

module.exports = {
  getPool,
  query,
  execute,
  testConnection,
  closePool,
  sql // Export sql for types (e.g., sql.Int, sql.VarChar)
};
