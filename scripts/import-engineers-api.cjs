// Script to import engineers via API
// Run this when the server is running

const http = require('http');

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

const API_URL = process.env.API_URL || 'http://localhost:3001';

function makeRequest(path, method, data) {
  return new Promise((resolve, reject) => {
    const url = new URL(path, API_URL);
    const postData = data ? JSON.stringify(data) : '';

    const options = {
      hostname: url.hostname,
      port: url.port,
      path: url.pathname,
      method: method,
      headers: {
        'Content-Type': 'application/json',
        'Content-Length': Buffer.byteLength(postData)
      }
    };

    const req = http.request(options, (res) => {
      let body = '';
      res.on('data', (chunk) => body += chunk);
      res.on('end', () => {
        try {
          const response = JSON.parse(body);
          resolve({ status: res.statusCode, data: response });
        } catch (e) {
          resolve({ status: res.statusCode, data: body });
        }
      });
    });

    req.on('error', reject);
    if (postData) req.write(postData);
    req.end();
  });
}

async function importEngineers() {
  try {
    console.log('🔍 Checking API connection...');
    const health = await makeRequest('/api/health', 'GET');
    if (health.status !== 200) {
      console.error('❌ API is not responding. Make sure the server is running.');
      console.error('   Run: npm run dev (in another terminal)');
      process.exit(1);
    }
    console.log('✅ API connection OK\n');

    console.log('📋 Fetching existing engineers...');
    const existing = await makeRequest('/api/engineers', 'GET');
    const existingNames = new Set((existing.data || []).map(e => e.name));
    console.log(`Found ${existingNames.size} existing engineers\n`);

    let added = 0;
    let skipped = 0;

    for (const engineer of engineers) {
      if (existingNames.has(engineer.name)) {
        console.log(`⏭️  Skipping ${engineer.name} (already exists)`);
        skipped++;
        continue;
      }

      try {
        const result = await makeRequest('/api/engineers', 'POST', {
          name: engineer.name,
          shift: engineer.shift
        });

        if (result.status === 200 || result.status === 201) {
          console.log(`✅ Added engineer: ${engineer.name} (${engineer.shift})`);
          if (result.data.userCreated) {
            console.log(`   👤 User created: ${result.data.username} / password`);
          }
          added++;
        } else {
          console.error(`❌ Failed to add ${engineer.name}: ${result.status}`);
        }
      } catch (error) {
        console.error(`❌ Error adding ${engineer.name}:`, error.message);
      }
    }

    console.log('\n📊 Summary:');
    console.log(`   ✅ Engineers added: ${added}`);
    console.log(`   ⏭️  Engineers skipped: ${skipped}`);
    console.log('\n✨ Import complete!');

    process.exit(0);
  } catch (error) {
    console.error('❌ Error importing engineers:', error.message);
    console.error('\nMake sure the server is running:');
    console.error('   npm run dev');
    process.exit(1);
  }
}

console.log('👥 Engineer Import Tool\n');
importEngineers();
