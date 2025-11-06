// Database connection module
const { Pool } = require('pg');
const config = require('../config/database');

// Create connection pool
const pool = new Pool({
  connectionString: config.connectionString,
  ssl: config.ssl,
  ...config.pool
});

// Test connection
pool.on('connect', () => {
  console.log('✅ Connected to database');
});

pool.on('error', (err) => {
  console.error('❌ Unexpected database error:', err);
  process.exit(-1);
});

// Query helper function
const query = async (text, params) => {
  const start = Date.now();
  try {
    const res = await pool.query(text, params);
    const duration = Date.now() - start;
    console.log('Executed query', { text, duration, rows: res.rowCount });
    return res;
  } catch (error) {
    console.error('Query error:', error);
    throw error;
  }
};

// Transaction helper
const transaction = async (callback) => {
  const client = await pool.connect();
  try {
    await client.query('BEGIN');
    const result = await callback(client);
    await client.query('COMMIT');
    return result;
  } catch (error) {
    await client.query('ROLLBACK');
    throw error;
  } finally {
    client.release();
  }
};

// Test database connection
const testConnection = async () => {
  try {
    const result = await query('SELECT NOW() as now');
    console.log('✅ Database connection test successful:', result.rows[0].now);
    return true;
  } catch (error) {
    console.error('❌ Database connection test failed:', error.message);
    return false;
  }
};

module.exports = {
  pool,
  query,
  transaction,
  testConnection
};
