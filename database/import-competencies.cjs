const sql = require('mssql');
const XLSX = require('xlsx');
const path = require('path');

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

async function importCompetencies(excelFilePath) {
  try {
    console.log('📖 Reading Excel file:', excelFilePath);
    const workbook = XLSX.readFile(excelFilePath);
    const sheetName = workbook.SheetNames[0];
    const worksheet = workbook.Sheets[sheetName];
    const data = XLSX.utils.sheet_to_json(worksheet);

    console.log(`✅ Found ${data.length} rows in Excel file\n`);

    console.log('🔌 Connecting to database...');
    await sql.connect(config);
    console.log('✅ Connected!\n');

    let stats = {
      productionAreas: 0,
      machines: 0,
      competencies: 0,
      skipped: 0
    };

    for (let i = 0; i < data.length; i++) {
      const row = data[i];

      // Expected columns: Production Area, Machine, Competency, Max Score
      // Adjust these column names to match your Excel file
      const productionAreaName = row['Production Area'] || row['ProductionArea'] || row['production_area'] || row['Area'];
      const machineName = row['Machine'] || row['machine'];
      const competencyName = row['Competency'] || row['competency'] || row['Skill'];
      const maxScore = row['Max Score'] || row['MaxScore'] || row['max_score'] || 3;

      if (!productionAreaName || !machineName || !competencyName) {
        console.log(`⚠️  Row ${i + 1}: Missing required fields, skipping...`);
        stats.skipped++;
        continue;
      }

      try {
        // 1. Get or create Production Area
        let productionAreaResult = await sql.query`
          SELECT id FROM production_areas WHERE name = ${productionAreaName}
        `;

        let productionAreaId;
        if (productionAreaResult.recordset.length === 0) {
          const insertResult = await sql.query`
            INSERT INTO production_areas (name, created_at, updated_at)
            OUTPUT INSERTED.id
            VALUES (${productionAreaName}, GETDATE(), GETDATE())
          `;
          productionAreaId = insertResult.recordset[0].id;
          stats.productionAreas++;
          console.log(`✓ Created production area: ${productionAreaName}`);
        } else {
          productionAreaId = productionAreaResult.recordset[0].id;
        }

        // 2. Get or create Machine
        let machineResult = await sql.query`
          SELECT id FROM machines
          WHERE name = ${machineName} AND production_area_id = ${productionAreaId}
        `;

        let machineId;
        if (machineResult.recordset.length === 0) {
          const insertResult = await sql.query`
            INSERT INTO machines (production_area_id, name, created_at, updated_at)
            OUTPUT INSERTED.id
            VALUES (${productionAreaId}, ${machineName}, GETDATE(), GETDATE())
          `;
          machineId = insertResult.recordset[0].id;
          stats.machines++;
          console.log(`  ✓ Created machine: ${machineName}`);
        } else {
          machineId = machineResult.recordset[0].id;
        }

        // 3. Get or create Competency
        let competencyResult = await sql.query`
          SELECT id FROM competencies
          WHERE name = ${competencyName} AND machine_id = ${machineId}
        `;

        if (competencyResult.recordset.length === 0) {
          await sql.query`
            INSERT INTO competencies (machine_id, name, max_score, created_at, updated_at)
            VALUES (${machineId}, ${competencyName}, ${maxScore}, GETDATE(), GETDATE())
          `;
          stats.competencies++;
          console.log(`    ✓ Created competency: ${competencyName} (max score: ${maxScore})`);
        } else {
          console.log(`    - Competency already exists: ${competencyName}`);
        }

      } catch (err) {
        console.error(`❌ Row ${i + 1} error:`, err.message);
        stats.skipped++;
      }
    }

    console.log('\n' + '='.repeat(60));
    console.log('✅ Import complete!');
    console.log('='.repeat(60));
    console.log(`📊 Statistics:`);
    console.log(`   Production Areas created: ${stats.productionAreas}`);
    console.log(`   Machines created: ${stats.machines}`);
    console.log(`   Competencies created: ${stats.competencies}`);
    console.log(`   Rows skipped: ${stats.skipped}`);
    console.log('='.repeat(60));

    await sql.close();
  } catch (err) {
    console.error('❌ Error:', err.message);
    await sql.close();
    process.exit(1);
  }
}

// Get filename from command line argument
const filePath = process.argv[2];

if (!filePath) {
  console.log('Usage: node import-competencies.cjs <excel-file-path>');
  console.log('\nExample: node import-competencies.cjs competencies.xlsx');
  console.log('\nExpected Excel columns:');
  console.log('  - Production Area (or Area)');
  console.log('  - Machine');
  console.log('  - Competency (or Skill)');
  console.log('  - Max Score (optional, defaults to 3)');
  process.exit(1);
}

importCompetencies(filePath);
