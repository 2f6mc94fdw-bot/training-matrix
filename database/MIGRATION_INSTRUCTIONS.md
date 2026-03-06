# Database Migration: Add Weighting Columns

## Problem

The competencies you imported are not showing in Aptitude because the database is missing required columns. The C++ application expects these multi-criteria weighting columns in the `competencies` table:

- `safety_impact`
- `production_impact`
- `frequency`
- `complexity`
- `future_value`

When Aptitude tries to load competencies, the SQL query fails because these columns don't exist.

## Solution

Run the migration script to add the missing columns to your database.

## How to Run the Migration

### Option 1: Using SQL Server Management Studio (SSMS) or Azure Data Studio

1. Open SQL Server Management Studio or Azure Data Studio
2. Connect to your database server (localhost)
3. Open the file: `database/add-weighting-columns.sql`
4. Execute the script (press F5 or click Execute)
5. You should see messages confirming each column was added

### Option 2: Using sqlcmd Command Line

```bash
sqlcmd -S localhost -U sa -P YourStrong@Passw0rd -d training_matrix -i database/add-weighting-columns.sql
```

### Option 3: Using Docker (if running SQL Server in Docker)

```bash
docker exec -it <container-name> /opt/mssql-tools/bin/sqlcmd \
  -S localhost -U sa -P YourStrong@Passw0rd \
  -d training_matrix -i /path/to/add-weighting-columns.sql
```

## After Running the Migration

1. Close Aptitude if it's running
2. Reopen Aptitude
3. Log in
4. Navigate to Production Areas
5. Your 188 competencies should now be visible!

## What the Migration Does

- Adds 5 new columns to the `competencies` table
- Sets default values of 3.0 for all existing competencies
- These values represent weighting criteria on a scale of 0.0 - 5.0:
  - **Safety Impact (30%)**: Risk if competency is lacking
  - **Production Impact (25%)**: Effect on output/quality
  - **Frequency (20%)**: How often the skill is used
  - **Complexity (15%)**: Difficulty to master
  - **Future Value (10%)**: Career/strategic importance

## Editing Competencies

After the migration, you can edit any competency in Aptitude to adjust these weighting values through the UI. The current defaults (all 3.0) give you a calculated weight of 3.0 for each competency.

## Notes

- The base schema.sql has also been updated, so future database setups will include these columns automatically
- The import-competencies.cjs script has been left unchanged - it will continue to use defaults
- All 188 imported competencies will remain intact and visible after the migration
