# Core Skills

This folder contains SQL scripts to update core skills (competencies that are independent of specific production equipment).

## Core Skill Categories

The system includes 5 core skill categories:

1. **Mechanical Skills** - Welding, Lathe, Mill, Gearbox, Pumps, Pneumatics, Hydraulics
2. **Electrical Skills** - Wiring, Circuit Diagnosis, PLC Programming, Motor Control
3. **Software Skills** - Programming, HMI, Data Analysis, SCADA
4. **Safety Skills** - LOTO, Confined Spaces, Hazard ID, Emergency Response
5. **Leadership Skills** - Communication, Teamwork, Problem Solving, Training Others

## Available Core Skill Updates

### Mechanical Skills
- **File:** `mechanical-skills.sql`
- **Categories:** 7 (Welding, Lathe, Mill, Gearbox, Pumps, Pneumatics, Hydraulics)
- **Total Skills:** 28

**Detailed breakdown:**
- Welding (3 skills)
- Lathe (5 skills)
- Mill (5 skills)
- Gearbox (3 skills)
- Pumps (3 skills)
- Pneumatics (4 skills)
- Hydraulics (5 skills)

## How to Update Core Skills

### Option 1: SQL Server Management Studio (SSMS)

1. Connect to your SQL Server
2. Select the `training_matrix` database
3. Open the desired SQL file (e.g., `mechanical-skills.sql`)
4. Execute the script (F5 or click Execute)
5. Check the Messages tab for confirmation

### Option 2: Command Line (sqlcmd)

```bash
# Update mechanical skills
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/core-skills/mechanical-skills.sql

# Or specify your server
sqlcmd -S YOUR_SERVER -U your_username -P your_password -d training_matrix -i database/core-skills/mechanical-skills.sql
```

### Option 3: Using Node.js Script

```bash
# Update specific core skill category
node database/install-core-skills.js mechanical-skills
```

## ⚠️ Important Notes

### Data Loss Warning
- These scripts **DELETE existing skills** in the category before inserting new ones
- Any existing assessments for deleted skills will be preserved but orphaned
- Back up your data before running update scripts

### Backup Before Update

```sql
-- Backup existing mechanical skills
SELECT * INTO core_skills_backup FROM core_skills WHERE category_id = 'mechanical';

-- Backup existing assessments
SELECT * INTO core_skill_assessments_backup
FROM core_skill_assessments csa
JOIN core_skills cs ON cs.id = csa.skill_id
WHERE cs.category_id = 'mechanical';
```

## Verifying Installation

After running a script, verify the data was updated:

```sql
-- Check skill count by category
SELECT
    csc.name AS Category,
    COUNT(cs.id) AS SkillCount
FROM core_skill_categories csc
LEFT JOIN core_skills cs ON cs.category_id = csc.id
GROUP BY csc.name
ORDER BY csc.name;

-- View all mechanical skills
SELECT
    cs.id,
    cs.name,
    cs.max_score
FROM core_skills cs
WHERE cs.category_id = 'mechanical'
ORDER BY cs.id;

-- Check assessments count
SELECT
    cs.category_id,
    COUNT(DISTINCT csa.engineer_id) AS EngineersAssessed,
    COUNT(*) AS TotalAssessments
FROM core_skill_assessments csa
JOIN core_skills cs ON cs.id = csa.skill_id
WHERE cs.category_id = 'mechanical'
GROUP BY cs.category_id;
```

## Creating Your Own Core Skill Updates

To update a core skill category, create a SQL file following this template:

```sql
-- Category Name - Core Skills Update

-- Delete existing skills in category
DELETE FROM core_skills WHERE category_id = 'your-category-id';
GO

-- Add new skills
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('skill-id-1', 'your-category-id', 'Skill description 1', 3),
    ('skill-id-2', 'your-category-id', 'Skill description 2', 3),
    ('skill-id-3', 'your-category-id', 'Skill description 3', 3);

GO

PRINT 'Category updated successfully!';
GO
```

### Skill ID Naming Convention

Use descriptive, kebab-case IDs:
- `welding-setup` not `ws1`
- `lathe-operations` not `l3`
- `pneumatics-troubleshooting` not `pt2`

This makes the database more maintainable and queries more readable.

## Restoring Previous Skills

If you need to restore the default skills:

```sql
-- Restore default mechanical skills
DELETE FROM core_skills WHERE category_id = 'mechanical';

INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('troubleshooting', 'mechanical', 'Troubleshooting', 3),
    ('preventive-maintenance', 'mechanical', 'Preventive Maintenance', 3),
    ('repair', 'mechanical', 'Equipment Repair', 3),
    ('installation', 'mechanical', 'Installation', 3);
```

## Assessment Data

### Orphaned Assessments
When you delete and replace skills, existing assessments remain in the database but reference old skill IDs.

To clean up orphaned assessments:

```sql
-- Find orphaned assessments (assessments for skills that no longer exist)
SELECT csa.*
FROM core_skill_assessments csa
LEFT JOIN core_skills cs ON cs.id = csa.skill_id
WHERE cs.id IS NULL;

-- Delete orphaned assessments (⚠️ THIS WILL DELETE DATA)
DELETE FROM core_skill_assessments
WHERE skill_id NOT IN (SELECT id FROM core_skills);
```

### Migrating Assessments
If you want to map old assessments to new skills:

```sql
-- Example: Map old 'troubleshooting' to new 'gearbox-troubleshooting'
UPDATE core_skill_assessments
SET skill_id = 'gearbox-troubleshooting'
WHERE skill_id = 'troubleshooting';
```

## Adding Skills Without Deleting

If you want to ADD skills without deleting existing ones:

```sql
-- Just insert new skills (don't delete first)
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('new-skill-id', 'mechanical', 'New skill description', 3);
```

## Default Core Skills

The system comes with these default core skills:

**Mechanical:** troubleshooting, preventive-maintenance, repair, installation
**Electrical:** wiring, circuit-diagnosis, plc, motor-control
**Software:** programming, hmi, data-analysis, scada
**Safety:** lockout-tagout, confined-spaces, hazard-identification, emergency-response
**Leadership:** communication, teamwork, problem-solving, training-others

## Batch Updates

To update multiple categories at once:

```sql
-- Update all categories master script
:r mechanical-skills.sql
-- :r electrical-skills.sql
-- :r software-skills.sql
-- :r safety-skills.sql
-- :r leadership-skills.sql
```

Or via command line:

```bash
sqlcmd -S localhost -U sa -d training_matrix -i mechanical-skills.sql
sqlcmd -S localhost -U sa -d training_matrix -i electrical-skills.sql
# ... etc
```

## Exporting Core Skills

To backup or share core skill definitions:

```sql
-- Export mechanical skills structure
SELECT
    'INSERT INTO core_skills (id, category_id, name, max_score) VALUES' +
    ' (''' + cs.id + ''', ''' + cs.category_id + ''', ''' +
    REPLACE(cs.name, '''', '''''') + ''', ' +
    CAST(cs.max_score AS VARCHAR) + ');'
FROM core_skills cs
WHERE cs.category_id = 'mechanical'
ORDER BY cs.id;
```

## Troubleshooting

### Error: "Cannot insert duplicate key"
- Skill ID already exists
- Use unique IDs for each skill
- Check existing skills: `SELECT * FROM core_skills WHERE category_id = 'mechanical';`

### Error: "The DELETE statement conflicted with the REFERENCE constraint"
- Cannot delete skills that have assessments referencing them
- The script should handle this with CASCADE delete
- Check your foreign key constraints

### Error: "Invalid object name"
- Wrong database selected
- Run: `USE training_matrix; GO`

### Assessments Disappeared
- Assessments are deleted when you delete the skill (CASCADE)
- Always backup assessments before updating skills
- Consider migrating assessments to new skill IDs instead

## Testing Updates

Before applying to production:

1. **Test on a copy of the database**
   ```sql
   -- Create test database
   CREATE DATABASE training_matrix_test;

   -- Copy data
   -- ... (use backup/restore or SELECT INTO)
   ```

2. **Run update script on test database**

3. **Verify results**
   ```sql
   SELECT * FROM core_skills WHERE category_id = 'mechanical';
   ```

4. **Check existing assessments still work**
   ```sql
   SELECT COUNT(*) FROM core_skill_assessments csa
   JOIN core_skills cs ON cs.id = csa.skill_id
   WHERE cs.category_id = 'mechanical';
   ```

5. **Apply to production when satisfied**

## Next Steps

After updating core skills:
1. Restart the application to load the new skills
2. Go to Core Skills tab to verify
3. Start assessing engineers on the new competencies
4. View progress in Core Skills Analytics

---

**Need to add more core skill categories?** Create a new SQL file following the template above.
