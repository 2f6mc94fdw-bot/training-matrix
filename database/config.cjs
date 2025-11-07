// SQL Server Connection Configuration
// For Docker SQL Server running on Mac

const config = {
  // SQL Server connection settings for Docker
  server: 'localhost',            // Docker container on localhost
  database: 'training_matrix',    // Database name
  user: 'sa',                     // SQL Server admin user
  password: 'YourStrong@Passw0rd', // Strong password (change if you want)

  // Connection options
  options: {
    encrypt: true,                // Use encryption
    trustServerCertificate: true, // Trust self-signed cert from Docker
    enableArithAbort: true,
    connectionTimeout: 30000,     // 30 seconds
    requestTimeout: 30000         // 30 seconds
  },

  // Connection pool settings
  pool: {
    max: 10,
    min: 0,
    idleTimeoutMillis: 30000
  }
};

module.exports = config;
