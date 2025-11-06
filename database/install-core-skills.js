// Install Core Skills Script
// Usage: node database/install-core-skills.js [skill-category]
// Example: node database/install-core-skills.js mechanical-skills

const fs = require('fs');
const path = require('path');
const db = require('./connection');

const CORE_SKILLS_DIR = path.join(__dirname, 'core-skills');

async function installCoreSkills(filename) {
  try {
    const filePath = path.join(CORE_SKILLS_DIR, filename);

    if (!fs.existsSync(filePath)) {
      console.error(`❌ File not found: ${filePath}`);
      console.log('\nAvailable core skill categories:');
      const files = fs.readdirSync(CORE_SKILLS_DIR)
        .filter(f => f.endsWith('.sql'))
        .map(f => f.replace('.sql', ''));
      files.forEach(f => console.log(`  - ${f}`));
      process.exit(1);
    }

    console.log(`\n📦 Updating core skills: ${filename.replace('.sql', '')}`);
    console.log('━'.repeat(60));

    // Warning about data deletion
    console.log('⚠️  WARNING: This will DELETE existing skills in this category!');
    console.log('   Existing assessments will be preserved.\n');

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

      // Skip PRINT statements
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
        csc.id AS CategoryID,
        csc.name AS Category,
        COUNT(cs.id) AS SkillCount
      FROM core_skill_categories csc
      LEFT JOIN core_skills cs ON cs.category_id = csc.id
      GROUP BY csc.id, csc.name
      ORDER BY csc.name
    `);

    if (result.recordset.length > 0) {
      console.log('✅ Core Skills by Category:');
      result.recordset.forEach(row => {
        const icon = row.CategoryID === 'mechanical' ? '🔧' :
                     row.CategoryID === 'electrical' ? '⚡' :
                     row.CategoryID === 'software' ? '💻' :
                     row.CategoryID === 'safety' ? '🦺' :
                     row.CategoryID === 'leadership' ? '👥' : '📋';
        console.log(`   ${icon} ${row.Category}: ${row.SkillCount} skills`);
      });
    }

    // Show detailed skills for the updated category
    const categoryId = filename.replace('-skills.sql', '').replace('.sql', '');
    const skillsResult = await pool.request().query(`
      SELECT TOP 5
        cs.id,
        cs.name,
        cs.max_score
      FROM core_skills cs
      WHERE cs.category_id = '${categoryId}'
      ORDER BY cs.id
    `);

    if (skillsResult.recordset.length > 0) {
      console.log(`\n📋 Sample skills from updated category:`);
      skillsResult.recordset.forEach(skill => {
        const shortName = skill.name.length > 60
          ? skill.name.substring(0, 60) + '...'
          : skill.name;
        console.log(`   • ${shortName}`);
      });
      if (skillsResult.recordset.length === 5) {
        console.log('   ... (and more)');
      }
    }

    console.log('\n━'.repeat(60));
    console.log('✅ Core skills update complete!\n');

  } catch (error) {
    console.error('\n❌ Update failed:', error.message);
    process.exit(1);
  } finally {
    await db.closePool();
  }
}

async function listCoreSkills() {
  console.log('\n📋 Available Core Skill Update Scripts:\n');

  const files = fs.readdirSync(CORE_SKILLS_DIR)
    .filter(f => f.endsWith('.sql'));

  if (files.length === 0) {
    console.log('   No core skill update scripts found.');
    return;
  }

  files.forEach(file => {
    const name = file.replace('.sql', '');
    const filePath = path.join(CORE_SKILLS_DIR, file);
    const content = fs.readFileSync(filePath, 'utf8');

    // Try to extract skill count from comments or content
    const skillMatches = content.match(/Total Skills:\s*(\d+)/i);
    const categoryMatches = content.match(/Categories:\s*(\d+)/i);

    let info = name;
    if (categoryMatches && skillMatches) {
      info += ` (${categoryMatches[1]} categories, ${skillMatches[1]} skills)`;
    } else if (skillMatches) {
      info += ` (${skillMatches[1]} skills)`;
    }

    console.log(`   • ${info}`);
    console.log(`     File: ${file}\n`);
  });

  console.log('Usage:');
  console.log('  node database/install-core-skills.js <filename>');
  console.log('\nExample:');
  console.log('  node database/install-core-skills.js mechanical-skills.sql\n');

  console.log('⚠️  Warning:');
  console.log('  These scripts DELETE existing skills before inserting new ones.');
  console.log('  Make sure to backup your data first!\n');
}

async function updateAll() {
  console.log('\n📦 Updating ALL core skill categories...\n');

  const files = fs.readdirSync(CORE_SKILLS_DIR)
    .filter(f => f.endsWith('.sql'))
    .sort();

  if (files.length === 0) {
    console.log('No core skill update scripts found.');
    return;
  }

  console.log(`⚠️  This will update ${files.length} core skill categories.`);
  console.log('   Press Ctrl+C to cancel, or wait 3 seconds to continue...\n');

  // Wait 3 seconds
  await new Promise(resolve => setTimeout(resolve, 3000));

  for (const file of files) {
    await installCoreSkills(file);
    console.log(''); // Space between updates
  }

  console.log('✅ All core skill categories updated!\n');
}

// Main execution
const args = process.argv.slice(2);

console.log('\n╔══════════════════════════════════════════════════╗');
console.log('║   Core Skills Installer                          ║');
console.log('╚══════════════════════════════════════════════════╝');

if (args.length === 0) {
  // No arguments - list available categories
  listCoreSkills().then(() => process.exit(0));
} else if (args[0] === '--all' || args[0] === '-a') {
  // Update all core skill categories
  updateAll().catch(console.error);
} else {
  // Update specific core skill category
  let filename = args[0];
  if (!filename.endsWith('.sql')) {
    filename += '.sql';
  }
  installCoreSkills(filename).catch(console.error);
}
