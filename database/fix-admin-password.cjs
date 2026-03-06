const sql = require('mssql');
const crypto = require('crypto');

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

// Generate hash compatible with C++ Crypto::hashPassword implementation
function hashPasswordCppStyle(password) {
  // Generate random salt (16 bytes = 32 hex chars, same as C++ generateSalt(16))
  const salt = crypto.randomBytes(16).toString('hex');

  // Perform PBKDF2-like derivation with 10000 iterations (matching C++ implementation)
  let derived = Buffer.concat([Buffer.from(password, 'utf8'), Buffer.from(salt, 'hex')]);

  // Apply 10000 rounds of hashing (matching C++ for loop)
  for (let i = 0; i < 10000; i++) {
    const hasher = crypto.createHash('sha256');
    hasher.update(derived);
    hasher.update(Buffer.from(i.toString())); // Include iteration count
    derived = hasher.digest();
  }

  // Return format: salt$hash (same as C++)
  return salt + '$' + derived.toString('hex');
}

async function fixAdminPassword() {
  try {
    console.log('🔐 Generating new password hash for admin...');
    const newPasswordHash = hashPasswordCppStyle('admin123');
    console.log('✅ Generated hash:', newPasswordHash);
    console.log('');

    console.log('🔌 Connecting to database...');
    await sql.connect(config);
    console.log('✅ Connected!');
    console.log('');

    console.log('📝 Updating admin password...');
    await sql.query`
      UPDATE users
      SET password = ${newPasswordHash}, updated_at = GETDATE()
      WHERE username = 'admin'
    `;

    console.log('✅ Admin password updated successfully!');
    console.log('');
    console.log('You can now login to Aptitude with:');
    console.log('  Username: admin');
    console.log('  Password: admin123');
    console.log('');

    await sql.close();
  } catch (err) {
    console.error('❌ Error:', err.message);
    await sql.close();
    process.exit(1);
  }
}

fixAdminPassword();
