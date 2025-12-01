// Script to import engineers and create user accounts
const db = require('../database/connection.cjs');
const bcrypt = require('bcryptjs');

const engineers = [
  { id: "eng-1", name: "John Irish", shift: "Day Shift" },
  { id: "eng-2", name: "Mark Castleton", shift: "Day Shift" },
  { id: "eng-import-1761509910877-0", name: "Dave Spauls", shift: "Day Shift" },
  { id: "eng-import-1761509910877-1", name: "Ben Barnham", shift: "Day Shift" },
  { id: "eng-import-1761509910877-2", name: "Mark Quin", shift: "B Shift" },
  { id: "eng-import-1761509910877-3", name: "Jon Anderson", shift: "C Shift" },
  { id: "eng-import-1761509910877-4", name: "Callum Clark", shift: "B Shift" },
  { id: "eng-import-1761509910877-5", name: "Dustin Lawrie", shift: "Day Shift" },
  { id: "eng-import-1761509910877-6", name: "Connor Clark", shift: "B Shift" },
  { id: "eng-import-1761509910877-7", name: "Matt Challis", shift: "B Shift" },
  { id: "eng-import-1761509910877-8", name: "Luke Hancy", shift: "B Shift" },
  { id: "eng-import-1761509910877-9", name: "Simon Brooks", shift: "D Shift" },
  { id: "eng-import-1761509910877-10", name: "Elliot McMullen", shift: "C Shift" },
  { id: "eng-import-1761509910877-11", name: "Richard Smith", shift: "C Shift" },
  { id: "eng-import-1761509910877-12", name: "Yauheni Shumanski", shift: "C Shift" },
  { id: "eng-import-1761509910877-13", name: "Daniel Wilson", shift: "D Shift" },
  { id: "eng-import-1761509910877-14", name: "Oliver Mills", shift: "D Shift" },
  { id: "eng-import-1761509910877-15", name: "James Sarosi", shift: "D Shift" },
  { id: "eng-import-1761509910877-16", name: "Scott Macfarlane", shift: "D Shift" }
];

async function importEngineers() {
  try {
    console.log('🔍 Checking current engineers...');
    const existing = await db.query('SELECT id, name FROM engineers');
    const existingIds = new Set(existing.rows.map(e => e.id));
    const existingNames = new Set(existing.rows.map(e => e.name));

    console.log(`Found ${existing.rows.length} existing engineers`);

    let added = 0;
    let skipped = 0;
    let usersCreated = 0;

    for (const engineer of engineers) {
      // Check if engineer exists by ID or name
      if (existingIds.has(engineer.id) || existingNames.has(engineer.name)) {
        console.log(`⏭️  Skipping ${engineer.name} (already exists)`);
        skipped++;
        continue;
      }

      // Add engineer
      await db.query(
        'INSERT INTO engineers (id, name, shift) VALUES ($1, $2, $3)',
        [engineer.id, engineer.name, engineer.shift]
      );
      console.log(`✅ Added engineer: ${engineer.name} (${engineer.shift})`);
      added++;

      // Create username from name (lowercase, replace spaces with dots)
      const username = engineer.name.toLowerCase().replace(/\s+/g, '.');

      // Check if user already exists
      const userCheck = await db.query(
        'SELECT id FROM users WHERE username = $1 OR engineer_id = $2',
        [username, engineer.id]
      );

      if (userCheck.rows.length === 0) {
        // Hash password
        const hashedPassword = await bcrypt.hash('password', 10);

        // Create user account
        const userId = `user-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
        await db.query(
          'INSERT INTO users (id, username, password, role, engineer_id) VALUES ($1, $2, $3, $4, $5)',
          [userId, username, hashedPassword, 'engineer', engineer.id]
        );
        console.log(`   👤 Created user: ${username} / password`);
        usersCreated++;
      } else {
        console.log(`   👤 User already exists for ${engineer.name}`);
      }
    }

    console.log('\n📊 Summary:');
    console.log(`   ✅ Engineers added: ${added}`);
    console.log(`   ⏭️  Engineers skipped: ${skipped}`);
    console.log(`   👤 User accounts created: ${usersCreated}`);
    console.log('\n✨ Import complete!');

    // Show final count
    const final = await db.query('SELECT COUNT(*) as count FROM engineers');
    console.log(`\n📈 Total engineers in database: ${final.rows[0].count}`);

    process.exit(0);
  } catch (error) {
    console.error('❌ Error importing engineers:', error);
    process.exit(1);
  }
}

importEngineers();
