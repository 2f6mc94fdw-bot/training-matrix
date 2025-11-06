// Master Installation Script
// Installs all production areas and core skills to SQL Server database
// Usage: node database/install-all.js

const fs = require('fs');
const path = require('path');
const db = require('./connection');

const PRODUCTION_AREAS_DIR = path.join(__dirname, 'production-areas');
const CORE_SKILLS_DIR = path.join(__dirname, 'core-skills');

async function executeScriptFile(filePath, description) {
  console.log(`\n📝 Installing: ${description}`);
  console.log('   File:', path.basename(filePath));

  const sql = fs.readFileSync(filePath, 'utf8');

  // Split by GO statements
  const batches = sql
    .split(/\nGO\n/i)
    .map(batch => batch.trim())
    .filter(batch => batch.length > 0 && !batch.startsWith('--'));

  const pool = await db.getPool();

  for (let i = 0; i < batches.length; i++) {
    const batch = batches[i];

    // Skip PRINT and SELECT statements
    if (batch.toUpperCase().startsWith('PRINT') || batch.toUpperCase().startsWith('SELECT')) {
      continue;
    }

    try {
      await pool.request().query(batch);
      process.stdout.write('.');
    } catch (error) {
      console.error(`\n   ❌ Error in batch ${i + 1}:`, error.message);
      throw error;
    }
  }

  console.log(' ✓');
}

async function installAll() {
  console.log('\n╔══════════════════════════════════════════════════════════════╗');
  console.log('║   Training Matrix - Complete Installation                   ║');
  console.log('╚══════════════════════════════════════════════════════════════╝\n');

  try {
    // Step 1: Test connection
    console.log('🔌 Step 1: Testing database connection...');
    const connected = await db.testConnection();

    if (!connected) {
      console.error('\n❌ Cannot connect to database. Please check your database/config.js file.');
      process.exit(1);
    }

    console.log('   ✅ Database connection successful\n');

    // Step 2: Install Production Areas
    console.log('━'.repeat(62));
    console.log('📦 Step 2: Installing Production Areas...');
    console.log('━'.repeat(62));

    const productionAreaFiles = [
      { file: 'line7-filling.sql', name: 'Line 7 Filling (13 machines, 33 competencies)' },
      { file: 'line7-packing.sql', name: 'Line 7 Packing (9 machines, 26 competencies)' },
      { file: 'line7-tank-room.sql', name: 'Line 7 Tank Room (7 systems, 34 competencies)' }
    ];

    for (const area of productionAreaFiles) {
      const filePath = path.join(PRODUCTION_AREAS_DIR, area.file);
      if (fs.existsSync(filePath)) {
        await executeScriptFile(filePath, area.name);
      } else {
        console.log(`   ⚠️  Skipping ${area.file} (file not found)`);
      }
    }

    // Step 3: Install Core Skills
    console.log('\n━'.repeat(62));
    console.log('🎓 Step 3: Installing Core Skills...');
    console.log('━'.repeat(62));

    const coreSkillFiles = [
      { file: 'mechanical-skills.sql', name: 'Mechanical Skills (7 categories, 28 skills)' },
      { file: 'software-skills.sql', name: 'Software Skills (12 systems, 53 skills)' },
      { file: 'electrical-skills.sql', name: 'Electrical Skills (5 categories, 20 skills)' }
    ];

    for (const skill of coreSkillFiles) {
      const filePath = path.join(CORE_SKILLS_DIR, skill.file);
      if (fs.existsSync(filePath)) {
        await executeScriptFile(filePath, skill.name);
      } else {
        console.log(`   ⚠️  Skipping ${skill.file} (file not found)`);
      }
    }

    // Step 4: Verify Installation
    console.log('\n━'.repeat(62));
    console.log('🔍 Step 4: Verifying Installation...');
    console.log('━'.repeat(62));

    const pool = await db.getPool();

    // Check production areas
    const areasResult = await pool.request().query(`
      SELECT
        pa.name AS ProductionArea,
        COUNT(DISTINCT m.id) AS Machines,
        COUNT(c.id) AS Competencies
      FROM production_areas pa
      LEFT JOIN machines m ON m.production_area_id = pa.id
      LEFT JOIN competencies c ON c.machine_id = m.id
      GROUP BY pa.name
      ORDER BY pa.name
    `);

    console.log('\n📊 Production Areas:');
    let totalMachines = 0;
    let totalCompetencies = 0;

    areasResult.recordset.forEach(row => {
      console.log(`   ✓ ${row.ProductionArea}`);
      console.log(`     Machines: ${row.Machines} | Competencies: ${row.Competencies}`);
      totalMachines += row.Machines;
      totalCompetencies += row.Competencies;
    });

    console.log(`   ─────────────────────────────────────────────`);
    console.log(`   Total: ${areasResult.recordset.length} areas | ${totalMachines} machines | ${totalCompetencies} competencies`);

    // Check core skills
    const skillsResult = await pool.request().query(`
      SELECT
        csc.name AS Category,
        COUNT(cs.id) AS SkillCount
      FROM core_skill_categories csc
      LEFT JOIN core_skills cs ON cs.category_id = csc.id
      GROUP BY csc.name
      ORDER BY csc.name
    `);

    console.log('\n🎓 Core Skills:');
    let totalSkills = 0;

    skillsResult.recordset.forEach(row => {
      const icon = row.Category.includes('Mechanical') ? '🔧' :
                   row.Category.includes('Software') ? '💻' :
                   row.Category.includes('Electrical') ? '⚡' :
                   row.Category.includes('Safety') ? '🦺' :
                   row.Category.includes('Leadership') ? '👥' : '📋';
      console.log(`   ${icon} ${row.Category}: ${row.SkillCount} skills`);
      totalSkills += row.SkillCount;
    });

    console.log(`   ─────────────────────────────────────────────`);
    console.log(`   Total: ${totalSkills} skills across ${skillsResult.recordset.length} categories`);

    // Grand total
    const grandTotal = totalCompetencies + totalSkills;

    console.log('\n━'.repeat(62));
    console.log('✅ Installation Complete!');
    console.log('━'.repeat(62));
    console.log(`\n📈 System Summary:`);
    console.log(`   Production Areas: ${areasResult.recordset.length} (${totalMachines} machines, ${totalCompetencies} competencies)`);
    console.log(`   Core Skills: ${skillsResult.recordset.length} categories (${totalSkills} skills)`);
    console.log(`   Grand Total: ${grandTotal} competencies\n`);

    console.log('🎉 Your Training Matrix database is ready to use!\n');

    console.log('Next steps:');
    console.log('  1. Start the application: npm run dev');
    console.log('  2. Login with: admin / admin123');
    console.log('  3. Change the default password');
    console.log('  4. Start adding engineers and assessments\n');

  } catch (error) {
    console.error('\n❌ Installation failed:', error.message);
    console.error('\nPlease check:');
    console.error('  1. SQL Server is running');
    console.error('  2. Database "training_matrix" exists');
    console.error('  3. Connection details in database/config.js are correct');
    console.error('  4. Schema has been run (database/schema.sql)\n');
    process.exit(1);
  } finally {
    await db.closePool();
  }
}

// Run installation
console.log('\n⚠️  WARNING: This will DELETE and REPLACE existing data in:');
console.log('   - Production Areas (all areas, machines, competencies)');
console.log('   - Core Skills (mechanical, software, electrical categories)\n');

console.log('Press Ctrl+C to cancel, or wait 5 seconds to continue...\n');

setTimeout(() => {
  installAll().catch(console.error);
}, 5000);
