# Production Areas

This folder contains SQL scripts to add production areas, machines, and competencies to the Training Matrix database.

## Available Production Areas

### Line 7 Filling
- **File:** `line7-filling.sql`
- **Machines:** 13
- **Total Competencies:** 33

**Machines included:**
1. Vision System (4 competencies)
2. Robot A (3 competencies)
3. Robot B (2 competencies)
4. Robot C (2 competencies)
5. Robot D (2 competencies)
6. Robot F (3 competencies)
7. Check Weigh (2 competencies)
8. HDPE (4 competencies)
9. LM2 (3 competencies)
10. Pocket Conveyor (2 competencies)
11. Cadencing Conveyor (1 competency)
12. Printers (3 competencies)
13. General HMI (2 competencies)

## How to Add Production Areas

### Option 1: SQL Server Management Studio (SSMS)

1. Connect to your SQL Server
2. Select the `training_matrix` database
3. Open the desired SQL file (e.g., `line7-filling.sql`)
4. Execute the script (F5 or click Execute)
5. Check the Messages tab for confirmation

### Option 2: Command Line (sqlcmd)

```bash
# Single production area
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/production-areas/line7-filling.sql

# Or specify your server
sqlcmd -S YOUR_SERVER -U your_username -P your_password -d training_matrix -i database/production-areas/line7-filling.sql
```

### Option 3: Using Node.js Script

```bash
# Run the production area installer
node database/install-production-areas.js line7-filling
```

## Verifying Installation

After running a script, verify the data was inserted:

```sql
-- Check production areas
SELECT * FROM production_areas;

-- Check machines for Line 7 Filling
SELECT
    pa.name AS ProductionArea,
    m.name AS Machine,
    m.importance,
    COUNT(c.id) AS CompetencyCount
FROM production_areas pa
JOIN machines m ON m.production_area_id = pa.id
LEFT JOIN competencies c ON c.machine_id = m.id
WHERE pa.name = 'Line 7 Filling'
GROUP BY pa.name, m.name, m.importance
ORDER BY m.name;

-- Check all competencies for a specific machine
SELECT
    pa.name AS ProductionArea,
    m.name AS Machine,
    c.name AS Competency,
    c.max_score
FROM production_areas pa
JOIN machines m ON m.production_area_id = pa.id
JOIN competencies c ON c.machine_id = m.id
WHERE pa.name = 'Line 7 Filling'
  AND m.name = 'Vision System'
ORDER BY c.id;
```

## Creating Your Own Production Areas

To add a new production area, create a SQL file following this template:

```sql
-- Your Production Area Name
INSERT INTO production_areas (name) VALUES ('Your Area Name');
GO

DECLARE @AreaId INT = (SELECT id FROM production_areas WHERE name = 'Your Area Name');

-- Machine 1
DECLARE @Machine1Id INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'Machine Name', 2);
SET @Machine1Id = SCOPE_IDENTITY();

INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@Machine1Id, 'Competency 1 description', 3),
    (@Machine1Id, 'Competency 2 description', 3),
    (@Machine1Id, 'Competency 3 description', 3);

-- Add more machines as needed...

GO
```

### Importance Levels

When adding machines, set the `importance` value:
- **3** = Critical/High importance
- **2** = Medium importance
- **1** = Low importance

This affects weighted score calculations in reports.

## Removing Production Areas

To remove a production area (⚠️ this will delete all associated data):

```sql
-- Delete a specific production area
DELETE FROM production_areas WHERE name = 'Line 7 Filling';

-- This will CASCADE delete:
-- - All machines in that area
-- - All competencies for those machines
-- - All assessments for those competencies
```

## Updating Production Areas

### Adding a Machine to Existing Area

```sql
-- Get the area ID
DECLARE @AreaId INT = (SELECT id FROM production_areas WHERE name = 'Line 7 Filling');

-- Add new machine
DECLARE @NewMachineId INT;
INSERT INTO machines (production_area_id, name, importance) VALUES (@AreaId, 'New Machine', 2);
SET @NewMachineId = SCOPE_IDENTITY();

-- Add competencies
INSERT INTO competencies (machine_id, name, max_score) VALUES
    (@NewMachineId, 'New competency 1', 3),
    (@NewMachineId, 'New competency 2', 3);
```

### Adding a Competency to Existing Machine

```sql
-- Get the machine ID
DECLARE @MachineId INT = (
    SELECT m.id
    FROM machines m
    JOIN production_areas pa ON pa.id = m.production_area_id
    WHERE pa.name = 'Line 7 Filling'
      AND m.name = 'Vision System'
);

-- Add new competency
INSERT INTO competencies (machine_id, name, max_score)
VALUES (@MachineId, 'New competency description', 3);
```

## Batch Installation

To install multiple production areas at once, create a master script:

```sql
-- Install all production areas
:r line7-filling.sql
-- :r line8-packaging.sql
-- :r line9-assembly.sql
-- Add more as needed
```

Or run them sequentially via command line:

```bash
sqlcmd -S localhost -U sa -P password -d training_matrix -i line7-filling.sql
sqlcmd -S localhost -U sa -P password -d training_matrix -i line8-packaging.sql
```

## Exporting Production Area Data

To backup or share a production area structure:

```sql
-- Export production area structure
SELECT
    'INSERT INTO production_areas (name) VALUES (''' + pa.name + ''');' AS SQL
FROM production_areas
WHERE name = 'Line 7 Filling'

UNION ALL

SELECT
    'INSERT INTO machines (production_area_id, name, importance) VALUES (' +
    CAST(m.production_area_id AS VARCHAR) + ', ''' +
    m.name + ''', ' + CAST(m.importance AS VARCHAR) + ');'
FROM machines m
JOIN production_areas pa ON pa.id = m.production_area_id
WHERE pa.name = 'Line 7 Filling';

-- Export competencies separately
SELECT
    'INSERT INTO competencies (machine_id, name, max_score) VALUES (' +
    CAST(c.machine_id AS VARCHAR) + ', ''' +
    REPLACE(c.name, '''', '''''') + ''', ' +
    CAST(c.max_score AS VARCHAR) + ');'
FROM competencies c
JOIN machines m ON m.id = c.machine_id
JOIN production_areas pa ON pa.id = m.production_area_id
WHERE pa.name = 'Line 7 Filling'
ORDER BY m.name, c.id;
```

## Troubleshooting

### Error: "Cannot insert duplicate key"
- The production area already exists
- Check existing areas: `SELECT * FROM production_areas;`
- Delete the existing one first or skip the insert

### Error: "The INSERT statement conflicted with the FOREIGN KEY constraint"
- The production area must exist before adding machines
- Ensure the production area insert succeeded first

### Error: "Invalid object name"
- Wrong database selected
- Make sure you're using the `training_matrix` database
- Run: `USE training_matrix; GO`

## Next Steps

After adding production areas:
1. Restart the application to load the new data
2. Go to Admin tab → Production Areas to verify
3. Start assessing engineers on the new competencies
4. View progress in Reports tab

---

**Need to add more production areas?** Create a new SQL file following the template above, or ask your admin to add them via the application UI.
