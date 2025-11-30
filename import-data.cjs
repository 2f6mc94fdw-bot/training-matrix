#!/usr/bin/env node
// Import production areas, machines, and competencies from JSON backup
const fs = require('fs');
const path = require('path');
const db = require('./database/connection.cjs');

async function importData(jsonFilePath) {
  console.log('====================================');
  console.log('  Training Matrix - Data Import');
  console.log('====================================\n');

  try {
    // Read JSON file
    console.log('📖 Reading JSON file...');
    const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));

    console.log(`✅ Found ${jsonData.productionAreas.length} production areas\n`);

    // Start transaction
    await db.transaction(async (client) => {
      let areasImported = 0;
      let machinesImported = 0;
      let competenciesImported = 0;

      // Import each production area
      for (const area of jsonData.productionAreas) {
        console.log(`📦 Importing: ${area.name}`);

        // Insert production area (ignore if already exists)
        const areaResult = await client.query(
          `INSERT INTO production_areas (name)
           OUTPUT INSERTED.*
           VALUES (@param0)`,
          [area.name]
        );

        const areaId = areaResult.rows[0].id;
        areasImported++;
        console.log(`   ✓ Production area created (ID: ${areaId})`);

        // Import machines for this area
        for (const machine of area.machines) {
          const machineResult = await client.query(
            `INSERT INTO machines (production_area_id, name, importance)
             OUTPUT INSERTED.*
             VALUES (@param0, @param1, @param2)`,
            [areaId, machine.name, machine.importance || 1]
          );

          const machineId = machineResult.rows[0].id;
          machinesImported++;
          console.log(`   ✓ Machine: ${machine.name} (ID: ${machineId}, Importance: ${machine.importance})`);

          // Import competencies for this machine
          for (const competency of machine.competencies) {
            await client.query(
              `INSERT INTO competencies (machine_id, name, max_score)
               VALUES (@param0, @param1, @param2)`,
              [machineId, competency.name, competency.maxScore || 3]
            );
            competenciesImported++;
          }
          console.log(`      → ${machine.competencies.length} competencies`);
        }
        console.log('');
      }

      console.log('====================================');
      console.log('✅ Import Complete!');
      console.log('====================================\n');
      console.log(`📊 Summary:`);
      console.log(`   Production Areas: ${areasImported}`);
      console.log(`   Machines: ${machinesImported}`);
      console.log(`   Competencies: ${competenciesImported}`);
      console.log('');
    });

    await db.closePool();
    console.log('✅ Database connection closed');
    console.log('\n🎉 You can now view your data at http://localhost:3000\n');

  } catch (error) {
    console.error('\n❌ Import failed:', error.message);
    console.error('\nFull error:', error);
    process.exit(1);
  }
}

// Get JSON file path from command line argument or use default
const jsonFile = process.argv[2] || './production-data.json';

if (!fs.existsSync(jsonFile)) {
  console.error(`\n❌ Error: File not found: ${jsonFile}`);
  console.error('\nUsage: node import-data.js <path-to-json-file>');
  console.error('Example: node import-data.js ./my-backup.json\n');
  process.exit(1);
}

importData(jsonFile);
