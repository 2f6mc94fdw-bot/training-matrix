const sql = require('mssql');
const fs = require('fs');
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

async function runMigration() {
  try {
    console.log('🔌 Connecting to database...');
    await sql.connect(config);
    console.log('✅ Connected!\n');

    // Add safety_impact column
    console.log('Adding safety_impact column...');
    try {
      await sql.query(`
        IF NOT EXISTS (SELECT * FROM sys.columns
                       WHERE object_id = OBJECT_ID(N'[dbo].[competencies]')
                       AND name = 'safety_impact')
        BEGIN
          ALTER TABLE [dbo].[competencies]
          ADD [safety_impact] FLOAT NOT NULL DEFAULT 3.0;
          PRINT 'Added safety_impact column';
        END
      `);
      console.log('✅ safety_impact column added');
    } catch (err) {
      if (err.message.includes('already exists')) {
        console.log('⚠️  safety_impact column already exists');
      } else {
        throw err;
      }
    }

    // Add production_impact column
    console.log('Adding production_impact column...');
    try {
      await sql.query(`
        IF NOT EXISTS (SELECT * FROM sys.columns
                       WHERE object_id = OBJECT_ID(N'[dbo].[competencies]')
                       AND name = 'production_impact')
        BEGIN
          ALTER TABLE [dbo].[competencies]
          ADD [production_impact] FLOAT NOT NULL DEFAULT 3.0;
          PRINT 'Added production_impact column';
        END
      `);
      console.log('✅ production_impact column added');
    } catch (err) {
      if (err.message.includes('already exists')) {
        console.log('⚠️  production_impact column already exists');
      } else {
        throw err;
      }
    }

    // Add frequency column
    console.log('Adding frequency column...');
    try {
      await sql.query(`
        IF NOT EXISTS (SELECT * FROM sys.columns
                       WHERE object_id = OBJECT_ID(N'[dbo].[competencies]')
                       AND name = 'frequency')
        BEGIN
          ALTER TABLE [dbo].[competencies]
          ADD [frequency] FLOAT NOT NULL DEFAULT 3.0;
          PRINT 'Added frequency column';
        END
      `);
      console.log('✅ frequency column added');
    } catch (err) {
      if (err.message.includes('already exists')) {
        console.log('⚠️  frequency column already exists');
      } else {
        throw err;
      }
    }

    // Add complexity column
    console.log('Adding complexity column...');
    try {
      await sql.query(`
        IF NOT EXISTS (SELECT * FROM sys.columns
                       WHERE object_id = OBJECT_ID(N'[dbo].[competencies]')
                       AND name = 'complexity')
        BEGIN
          ALTER TABLE [dbo].[competencies]
          ADD [complexity] FLOAT NOT NULL DEFAULT 3.0;
          PRINT 'Added complexity column';
        END
      `);
      console.log('✅ complexity column added');
    } catch (err) {
      if (err.message.includes('already exists')) {
        console.log('⚠️  complexity column already exists');
      } else {
        throw err;
      }
    }

    // Add future_value column
    console.log('Adding future_value column...');
    try {
      await sql.query(`
        IF NOT EXISTS (SELECT * FROM sys.columns
                       WHERE object_id = OBJECT_ID(N'[dbo].[competencies]')
                       AND name = 'future_value')
        BEGIN
          ALTER TABLE [dbo].[competencies]
          ADD [future_value] FLOAT NOT NULL DEFAULT 3.0;
          PRINT 'Added future_value column';
        END
      `);
      console.log('✅ future_value column added');
    } catch (err) {
      if (err.message.includes('already exists')) {
        console.log('⚠️  future_value column already exists');
      } else {
        throw err;
      }
    }

    // Verify the columns were added
    console.log('\n📊 Verifying columns...');
    const result = await sql.query(`
      SELECT COLUMN_NAME
      FROM INFORMATION_SCHEMA.COLUMNS
      WHERE TABLE_NAME = 'competencies'
      AND COLUMN_NAME IN ('safety_impact', 'production_impact', 'frequency', 'complexity', 'future_value')
      ORDER BY COLUMN_NAME
    `);

    console.log('\nColumns found in competencies table:');
    result.recordset.forEach(col => {
      console.log(`  ✓ ${col.COLUMN_NAME}`);
    });

    // Check competency count
    const countResult = await sql.query('SELECT COUNT(*) as count FROM competencies');
    console.log(`\n✅ Migration complete! ${countResult.recordset[0].count} competencies ready to display in Aptitude.`);
    console.log('\n🎉 You can now open Aptitude and see all your competencies!');

    await sql.close();
  } catch (err) {
    console.error('❌ Error:', err.message);
    await sql.close();
    process.exit(1);
  }
}

runMigration();
