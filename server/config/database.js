// Database configuration
// You can set the connection string via environment variable or directly here

const config = {
  // Option 1: Use environment variable (recommended for production)
  // Set DB_CONNECTION_STRING in your environment or .env file
  connectionString: process.env.DB_CONNECTION_STRING ||
    // Option 2: Direct connection string (for testing/development)
    'postgresql://username:password@localhost:5432/training_matrix',

  // Alternative: Individual connection parameters
  connection: {
    host: process.env.DB_HOST || 'localhost',
    port: process.env.DB_PORT || 5432,
    database: process.env.DB_NAME || 'training_matrix',
    user: process.env.DB_USER || 'postgres',
    password: process.env.DB_PASSWORD || 'password',
  },

  // Pool configuration
  pool: {
    min: 2,
    max: 10,
    idleTimeoutMillis: 30000,
    connectionTimeoutMillis: 2000,
  },

  // SSL configuration for production
  ssl: process.env.DB_SSL === 'true' ? { rejectUnauthorized: false } : false
};

module.exports = config;
