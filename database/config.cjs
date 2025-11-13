// SQL Server Connection Configuration
// Uses environment variables for security
// See .env.example for configuration template

require('dotenv').config({ path: require('path').join(__dirname, '../.env') });

const config = {
  // SQL Server connection settings
  server: process.env.DB_SERVER || 'localhost',
  database: process.env.DB_NAME || 'training_matrix',
  user: process.env.DB_USER || 'sa',
  password: process.env.DB_PASSWORD,  // Required - no default for security

  // Connection options
  options: {
    encrypt: process.env.DB_ENCRYPT === 'true',
    trustServerCertificate: process.env.DB_TRUST_SERVER_CERTIFICATE !== 'false',
    enableArithAbort: true,
    connectionTimeout: parseInt(process.env.DB_CONNECTION_TIMEOUT) || 30000,
    requestTimeout: parseInt(process.env.DB_REQUEST_TIMEOUT) || 30000
  },

  // Connection pool settings
  pool: {
    max: parseInt(process.env.DB_POOL_MAX) || 10,
    min: parseInt(process.env.DB_POOL_MIN) || 0,
    idleTimeoutMillis: parseInt(process.env.DB_POOL_IDLE_TIMEOUT) || 30000
  }
};

// Validate required configuration
if (!config.password) {
  console.error('❌ ERROR: DB_PASSWORD environment variable is required');
  console.error('   Please set DB_PASSWORD in your .env file');
  process.exit(1);
}

module.exports = config;
