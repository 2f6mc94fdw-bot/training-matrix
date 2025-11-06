// Install Production Areas Script
// Usage: node database/install-production-areas.js [area-name]
// Example: node database/install-production-areas.js line7-filling

const fs = require('fs');
const path = require('path');
const db = require('./connection');

const PRODUCTION_AREAS_DIR = path.join(__dirname, 'production-areas');

async function installProductionArea(filename) {
  try {
    const filePath = path.join(PRODUCTION_AREAS_DIR, filename);

    if (!fs.existsSync(filePath)) {
      console.error(`❌ File not found: ${filePath}`);
      console.log('\nAvailable production areas:');
      const files = fs.readdirSync(PRODUCTION_AREAS_DIR)
        .filter(f => f.endsWith('.sql'))
        .map(f => f.replace('.sql', ''));
      files.forEach(f => console.log(`  - ${f}`));
      process.exit(1);
    }

    console.log(`\n📦 Installing production area: ${filename.replace('.sql', '')}`);
    console.log('━'.repeat(60));

    // Read SQL file
    const sql = fs.readFileSync(filePath, 'utf8');

    // Split by GO statements and execute each batch
    const batches = sql
      .split(/\nGO\n/i)
      .map(batch => batch.trim())
      .filter(batch => batch.length > 0 && !batch.startsWith('--'));

    console.log(`📝 Executing ${batches.length} SQL batches...\n`);

    const pool = await db.getPool();

    for (let i = 0; i < batches.length; i++) {
      const batch = batches[i];

      // Skip PRINT statements (they don't execute well in mssql)
      if (batch.toUpperCase().startsWith('PRINT')) {
        const message = batch.match(/PRINT '(.+)'/i);
        if (message) {
          console.log(`   ${message[1]}`);
        }
        continue;
      }

      // Skip SELECT verification queries for now
      if (batch.toUpperCase().startsWith('SELECT')) {
        continue;
      }

      try {
        await pool.request().query(batch);
        process.stdout.write('.');
      } catch (error) {
        console.error(`\n❌ Error in batch ${i + 1}:`, error.message);
        throw error;
      }
    }

    console.log('\n');

    // Verify installation
    console.log('🔍 Verifying installation...\n');

    const result = await pool.request().query(`
      SELECT
        pa.name AS ProductionArea,
        COUNT(DISTINCT m.id) AS MachineCount,
        COUNT(c.id) AS CompetencyCount
      FROM production_areas pa
      LEFT JOIN machines m ON m.production_area_id = pa.id
      LEFT JOIN competencies c ON c.machine_id = m.id
      GROUP BY pa.name
      ORDER BY pa.name DESC
    `);

    if (result.recordset.length > 0) {
      const latest = result.recordset[0];
      console.log(`✅ Production Area: ${latest.ProductionArea}`);
      console.log(`   Machines: ${latest.MachineCount}`);
      console.log(`   Competencies: ${latest.CompetencyCount}`);
    }

    console.log('\n━'.repeat(60));
    console.log('✅ Installation complete!\n');

    // Show all production areas
    console.log('📊 All Production Areas:');
    result.recordset.forEach(row => {
      console.log(`   • ${row.ProductionArea} (${row.MachineCount} machines, ${row.CompetencyCount} competencies)`);
    });

    console.log('');

  } catch (error) {
    console.error('\n❌ Installation failed:', error.message);
    process.exit(1);
  } finally {
    await db.closePool();
  }
}

async function listProductionAreas() {
  console.log('\n📋 Available Production Area Scripts:\n');

  const files = fs.readdirSync(PRODUCTION_AREAS_DIR)
    .filter(f => f.endsWith('.sql'));

  if (files.length === 0) {
    console.log('   No production area scripts found.');
    return;
  }

  files.forEach(file => {
    const name = file.replace('.sql', '');
    const filePath = path.join(PRODUCTION_AREAS_DIR, file);
    const content = fs.readFileSync(filePath, 'utf8');

    // Try to extract machine count from comments or content
    const machineMatches = content.match(/Machines:\s*(\d+)/i);
    const competencyMatches = content.match(/Competencies:\s*(\d+)/i);

    let info = name;
    if (machineMatches && competencyMatches) {
      info += ` (${machineMatches[1]} machines, ${competencyMatches[1]} competencies)`;
    }

    console.log(`   • ${info}`);
    console.log(`     File: ${file}\n`);
  });

  console.log('Usage:');
  console.log('  node database/install-production-areas.js <filename>');
  console.log('\nExample:');
  console.log('  node database/install-production-areas.js line7-filling.sql\n');
}

async function installAll() {
  console.log('\n📦 Installing ALL production areas...\n');

  const files = fs.readdirSync(PRODUCTION_AREAS_DIR)
    .filter(f => f.endsWith('.sql'))
    .sort();

  if (files.length === 0) {
    console.log('No production area scripts found.');
    return;
  }

  for (const file of files) {
    await installProductionArea(file);
    console.log(''); // Space between installations
  }

  console.log('✅ All production areas installed!\n');
}

// Main execution
const args = process.argv.slice(2);

console.log('\n╔══════════════════════════════════════════════════╗');
console.log('║   Production Area Installer                      ║');
console.log('╚══════════════════════════════════════════════════╝');

if (args.length === 0) {
  // No arguments - list available areas
  listProductionAreas().then(() => process.exit(0));
} else if (args[0] === '--all' || args[0] === '-a') {
  // Install all production areas
  installAll().catch(console.error);
} else {
  // Install specific production area
  let filename = args[0];
  if (!filename.endsWith('.sql')) {
    filename += '.sql';
  }
  installProductionArea(filename).catch(console.error);
}
