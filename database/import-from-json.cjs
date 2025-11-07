// Import Production Areas from JSON Backup
// This script imports production areas, machines, competencies, and engineers from a JSON file

const sql = require('mssql');
const fs = require('fs');
const path = require('path');

// Get config
const config = require('./config.cjs');

async function importFromJSON(jsonFilePath) {
  let pool = null;

  try {
    console.log('🚀 Starting JSON import...\n');

    // Read JSON file
    console.log(`📄 Reading JSON file: ${jsonFilePath}`);
    const jsonData = JSON.parse(fs.readFileSync(jsonFilePath, 'utf8'));
    console.log('✅ JSON file loaded\n');

    // Connect to database
    console.log('📡 Connecting to SQL Server...');
    pool = await sql.connect(config);
    console.log('✅ Connected!\n');

    // NOTE: We're keeping existing production areas and adding new ones
    // If you want to clear all data first, uncomment the line below:
    // await pool.request().query('DELETE FROM production_areas');

    // Import Production Areas
    console.log('🏭 Importing production areas...');
    const productionAreas = jsonData.productionAreas || [];

    for (const area of productionAreas) {
      console.log(`   📍 ${area.name}`);

      // Insert production area
      const areaResult = await pool.request()
        .input('name', sql.NVarChar, area.name)
        .query(`
          INSERT INTO production_areas (name, created_at, updated_at)
          OUTPUT INSERTED.id
          VALUES (@name, GETDATE(), GETDATE())
        `);

      const areaId = areaResult.recordset[0].id;

      // Import machines for this area
      for (const machine of area.machines || []) {
        console.log(`      🔧 ${machine.name} (importance: ${machine.importance})`);

        const machineResult = await pool.request()
          .input('production_area_id', sql.Int, areaId)
          .input('name', sql.NVarChar, machine.name)
          .input('importance', sql.Int, machine.importance || 1)
          .query(`
            INSERT INTO machines (production_area_id, name, importance, created_at, updated_at)
            OUTPUT INSERTED.id
            VALUES (@production_area_id, @name, @importance, GETDATE(), GETDATE())
          `);

        const machineId = machineResult.recordset[0].id;

        // Import competencies for this machine
        for (const comp of machine.competencies || []) {
          await pool.request()
            .input('machine_id', sql.Int, machineId)
            .input('name', sql.NVarChar, comp.name)
            .input('max_score', sql.Int, comp.maxScore || 3)
            .query(`
              INSERT INTO competencies (machine_id, name, max_score, created_at, updated_at)
              VALUES (@machine_id, @name, @max_score, GETDATE(), GETDATE())
            `);
        }

        console.log(`         ✅ ${machine.competencies.length} competencies`);
      }
    }

    console.log(`✅ Imported ${productionAreas.length} production areas!\n`);

    // Import Engineers
    console.log('👷 Importing engineers...');
    const engineers = jsonData.engineers || [];

    // NOTE: We're keeping existing engineers and only adding new ones

    for (const engineer of engineers) {
      // Check if engineer exists
      const existing = await pool.request()
        .input('id', sql.NVarChar, engineer.id)
        .query('SELECT id FROM engineers WHERE id = @id');

      if (existing.recordset.length === 0) {
        await pool.request()
          .input('id', sql.NVarChar, engineer.id)
          .input('name', sql.NVarChar, engineer.name)
          .input('shift', sql.NVarChar, engineer.shift)
          .query(`
            INSERT INTO engineers (id, name, shift, created_at, updated_at)
            VALUES (@id, @name, @shift, GETDATE(), GETDATE())
          `);
        console.log(`   ✅ ${engineer.name} (${engineer.shift})`);
      }
    }

    console.log(`✅ Imported ${engineers.length} engineers!\n`);

    // Summary
    console.log('📊 Import Summary:');

    const areaSummary = await pool.request().query('SELECT COUNT(*) as count FROM production_areas');
    console.log(`   Production Areas: ${areaSummary.recordset[0].count}`);

    const machineSummary = await pool.request().query('SELECT COUNT(*) as count FROM machines');
    console.log(`   Machines: ${machineSummary.recordset[0].count}`);

    const compSummary = await pool.request().query('SELECT COUNT(*) as count FROM competencies');
    console.log(`   Competencies: ${compSummary.recordset[0].count}`);

    const engSummary = await pool.request().query('SELECT COUNT(*) as count FROM engineers');
    console.log(`   Engineers: ${engSummary.recordset[0].count}`);

    console.log('\n🎉 IMPORT COMPLETE! 🎉\n');

  } catch (error) {
    console.error('❌ Import failed:', error.message);
    console.error(error);
    throw error;
  } finally {
    if (pool) {
      await pool.close();
    }
  }
}

// Get JSON file path from command line or use default
const jsonFilePath = process.argv[2] || '/Users/danielabley/Desktop/JSON Backups/training-matrix-2025-10-30-2.json';

console.log(`Import file: ${jsonFilePath}\n`);

// Run the import
importFromJSON(jsonFilePath)
  .then(() => {
    console.log('✅ All done!');
    process.exit(0);
  })
  .catch((error) => {
    console.error('❌ Import failed:', error);
    process.exit(1);
  });
