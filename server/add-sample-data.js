// Add Sample Data to Training Matrix
// Run this to populate the database with example production areas, machines, and competencies
require('dotenv').config({ path: require('path').join(__dirname, '../.env') });
const sql = require('mssql');
const config = require('../database/config.cjs');

async function addSampleData() {
  console.log('\n╔══════════════════════════════════════════════════╗');
  console.log('║   Adding Sample Data                              ║');
  console.log('╚══════════════════════════════════════════════════╝\n');

  let pool;

  try {
    pool = await sql.connect(config);
    console.log('✅ Connected to database\n');

    // Check if data already exists
    const existingAreas = await pool.request().query('SELECT COUNT(*) as count FROM production_areas');

    if (existingAreas.recordset[0].count > 0) {
      console.log('⚠️  Production areas already exist.');
      console.log(`   Found ${existingAreas.recordset[0].count} areas.\n`);

      const readline = require('readline').createInterface({
        input: process.stdin,
        output: process.stdout
      });

      const answer = await new Promise(resolve => {
        readline.question('Do you want to add more sample data anyway? (yes/no): ', resolve);
      });
      readline.close();

      if (answer.toLowerCase() !== 'yes') {
        console.log('\n✋ Cancelled. No data added.\n');
        process.exit(0);
      }
    }

    console.log('📝 Adding sample production areas...\n');

    // Production Area 1: Line 7 - Filling
    const area1 = await pool.request()
      .input('name', sql.VarChar, 'Line 7 - Filling')
      .query('INSERT INTO production_areas (name) OUTPUT INSERTED.* VALUES (@name)');

    const areaId1 = area1.recordset[0].id;
    console.log('   ✓ Line 7 - Filling');

    // Machines for Line 7 - Filling
    const machine1 = await pool.request()
      .input('areaId', sql.Int, areaId1)
      .input('name', sql.VarChar, 'Filler Machine 1')
      .input('importance', sql.Int, 3)
      .query('INSERT INTO machines (production_area_id, name, importance) OUTPUT INSERTED.* VALUES (@areaId, @name, @importance)');

    const machineId1 = machine1.recordset[0].id;
    console.log('     ✓ Filler Machine 1');

    // Competencies for Filler Machine 1
    const competencies1 = [
      'Basic Operation',
      'CIP Cleaning',
      'Changeover Setup',
      'Troubleshooting'
    ];

    for (const comp of competencies1) {
      await pool.request()
        .input('machineId', sql.Int, machineId1)
        .input('name', sql.VarChar, comp)
        .input('maxScore', sql.Int, 3)
        .query('INSERT INTO competencies (machine_id, name, max_score) VALUES (@machineId, @name, @maxScore)');
      console.log(`       • ${comp}`);
    }

    // Production Area 2: Line 7 - Packing
    const area2 = await pool.request()
      .input('name', sql.VarChar, 'Line 7 - Packing')
      .query('INSERT INTO production_areas (name) OUTPUT INSERTED.* VALUES (@name)');

    const areaId2 = area2.recordset[0].id;
    console.log('   ✓ Line 7 - Packing');

    // Machines for Line 7 - Packing
    const machine2 = await pool.request()
      .input('areaId', sql.Int, areaId2)
      .input('name', sql.VarChar, 'Case Packer')
      .input('importance', sql.Int, 2)
      .query('INSERT INTO machines (production_area_id, name, importance) OUTPUT INSERTED.* VALUES (@areaId, @name, @importance)');

    const machineId2 = machine2.recordset[0].id;
    console.log('     ✓ Case Packer');

    // Competencies for Case Packer
    const competencies2 = [
      'Machine Setup',
      'Quality Checks',
      'Maintenance'
    ];

    for (const comp of competencies2) {
      await pool.request()
        .input('machineId', sql.Int, machineId2)
        .input('name', sql.VarChar, comp)
        .input('maxScore', sql.Int, 3)
        .query('INSERT INTO competencies (machine_id, name, max_score) VALUES (@machineId, @name, @maxScore)');
      console.log(`       • ${comp}`);
    }

    // Production Area 3: Line 7 - Tank Room
    const area3 = await pool.request()
      .input('name', sql.VarChar, 'Line 7 - Tank Room')
      .query('INSERT INTO production_areas (name) OUTPUT INSERTED.* VALUES (@name)');

    const areaId3 = area3.recordset[0].id;
    console.log('   ✓ Line 7 - Tank Room');

    // Machines for Line 7 - Tank Room
    const machine3 = await pool.request()
      .input('areaId', sql.Int, areaId3)
      .input('name', sql.VarChar, 'Mixing Tank 1')
      .input('importance', sql.Int, 3)
      .query('INSERT INTO machines (production_area_id, name, importance) OUTPUT INSERTED.* VALUES (@areaId, @name, @importance)');

    const machineId3 = machine3.recordset[0].id;
    console.log('     ✓ Mixing Tank 1');

    // Competencies for Mixing Tank
    const competencies3 = [
      'Recipe Programming',
      'Temperature Control',
      'Safety Procedures'
    ];

    for (const comp of competencies3) {
      await pool.request()
        .input('machineId', sql.Int, machineId3)
        .input('name', sql.VarChar, comp)
        .input('maxScore', sql.Int, 3)
        .query('INSERT INTO competencies (machine_id, name, max_score) VALUES (@machineId, @name, @maxScore)');
      console.log(`       • ${comp}`);
    }

    console.log('\n╔══════════════════════════════════════════════════╗');
    console.log('║   ✅ Sample Data Added Successfully!             ║');
    console.log('╚══════════════════════════════════════════════════╝\n');

    console.log('Summary:');
    console.log('  • 3 Production Areas');
    console.log('  • 3 Machines');
    console.log('  • 10 Competencies\n');

    console.log('You can now:');
    console.log('  1. Add engineers in Admin → Engineers');
    console.log('  2. Assess engineers on these competencies');
    console.log('  3. Add more production areas as needed\n');

  } catch (error) {
    console.error('\n❌ Error adding sample data:', error.message);
    console.error(error.stack);
    process.exit(1);
  } finally {
    if (pool) {
      await pool.close();
    }
  }
}

addSampleData().catch(console.error);
