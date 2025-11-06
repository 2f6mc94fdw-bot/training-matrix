// SQL Server Connection Configuration - EXAMPLE FILE
// Copy this file to config.js and update with your actual server details

const config = {
  // SQL Server Connection String
  // Format: Server=hostname;Database=database;User Id=username;Password=password;Encrypt=true

  // Option 1: Connection string (easiest - update this for your work server)
  connectionString: 'Server=YOUR_SERVER;Database=training_matrix;User Id=YOUR_USERNAME;Password=YOUR_PASSWORD;Encrypt=true;TrustServerCertificate=true',

  // Option 2: Individual parameters (alternative to connection string)
  server: 'YOUR_SERVER',          // Your SQL Server hostname or IP (e.g., 'SERVERNAME' or '192.168.1.100')
  database: 'training_matrix',    // Database name
  user: 'YOUR_USERNAME',          // SQL Server username
  password: 'YOUR_PASSWORD',      // SQL Server password

  // Connection options
  options: {
    encrypt: true,                // Use encryption (required for Azure SQL, recommended for all)
    trustServerCertificate: true, // Set to false for production with proper SSL certificates
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

// Export for use in application
module.exports = config;

/*
 * USAGE EXAMPLES FOR DIFFERENT SCENARIOS:
 *
 * ===========================================================================
 * LOCAL SQL SERVER (Developer Edition or Express):
 * ===========================================================================
 * connectionString: 'Server=localhost;Database=training_matrix;User Id=sa;Password=YourPassword123;Encrypt=true;TrustServerCertificate=true'
 *
 * Or with separate parameters:
 * server: 'localhost'
 * database: 'training_matrix'
 * user: 'sa'
 * password: 'YourPassword123'
 *
 * ===========================================================================
 * WORK SERVER WITH SQL AUTHENTICATION:
 * ===========================================================================
 * connectionString: 'Server=SERVERNAME;Database=training_matrix;User Id=your_username;Password=your_password;Encrypt=true;TrustServerCertificate=true'
 *
 * Or by IP address:
 * connectionString: 'Server=192.168.1.100;Database=training_matrix;User Id=your_username;Password=your_password;Encrypt=true;TrustServerCertificate=true'
 *
 * ===========================================================================
 * WORK SERVER WITH WINDOWS AUTHENTICATION (Trusted Connection):
 * ===========================================================================
 * connectionString: 'Server=SERVERNAME;Database=training_matrix;Trusted_Connection=yes;Encrypt=true'
 *
 * Or with separate parameters:
 * server: 'SERVERNAME'
 * database: 'training_matrix'
 * options: {
 *   trustedConnection: true,
 *   encrypt: true
 * }
 *
 * ===========================================================================
 * NAMED INSTANCE:
 * ===========================================================================
 * connectionString: 'Server=SERVERNAME\\SQLEXPRESS;Database=training_matrix;User Id=username;Password=password;Encrypt=true'
 *
 * Or:
 * connectionString: 'Server=SERVER01\\INSTANCE01;Database=training_matrix;User Id=username;Password=password;Encrypt=true'
 *
 * ===========================================================================
 * AZURE SQL DATABASE:
 * ===========================================================================
 * connectionString: 'Server=yourserver.database.windows.net;Database=training_matrix;User Id=username@yourserver;Password=password;Encrypt=true'
 *
 * ===========================================================================
 * WITH SPECIFIC PORT:
 * ===========================================================================
 * connectionString: 'Server=192.168.1.100,1433;Database=training_matrix;User Id=username;Password=password;Encrypt=true'
 *
 * Note: Port comes after comma, not colon
 *
 * ===========================================================================
 * TROUBLESHOOTING TIPS:
 * ===========================================================================
 * 1. Make sure SQL Server is running and allows remote connections
 * 2. Check firewall allows port 1433 (default SQL Server port)
 * 3. Verify SQL Server Authentication is enabled (not just Windows Auth)
 * 4. Use SQL Server Management Studio (SSMS) to test connection first
 * 5. For named instances, SQL Server Browser service must be running
 * 6. Check that TCP/IP protocol is enabled in SQL Server Configuration Manager
 */
