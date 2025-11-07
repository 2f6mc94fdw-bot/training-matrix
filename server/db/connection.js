// Database connection module for SQL Server
// Reuses the main database connection from /database/connection.js

const path = require('path');
const dbConnection = require(path.join(__dirname, '../../database/connection'));

// Re-export the connection functions
module.exports = {
  pool: dbConnection.getPool,
  query: dbConnection.query,
  testConnection: dbConnection.testConnection,
  sql: dbConnection.sql
};
