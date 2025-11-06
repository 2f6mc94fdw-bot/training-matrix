-- Electrical Skills - Core Skills Update
-- Replace default electrical skills with detailed competencies

-- Delete existing electrical skills
DELETE FROM core_skills WHERE category_id = 'electrical';
GO

-- 1. Electrical Knowledge
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('electrical-theory', 'electrical', '1.1 Good understanding of electrical theory, principles and practices', 3),
    ('electrical-components', 'electrical', '1.2 Knowledge of components (motors, transformers, control panels, inverters)', 3),
    ('electrical-troubleshooting', 'electrical', '1.3 Knowledge of troubleshooting, diagnosing and resolving electrical faults', 3),
    ('electrical-drawings', 'electrical', '1.4 Proficient in reading electrical drawings and schematics', 3);

-- 2. Electrical Hand Skills
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('wire-termination', 'electrical', '2.1 Proficient in wire termination and cable management techniques', 3),
    ('testing-equipment', 'electrical', '2.2 Familiar with testing equipment (multimeters, MEGERs)', 3),
    ('soldering', 'electrical', '2.3 Familiar with correct soldering techniques', 3);

-- 3. Motors
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('motor-types', 'electrical', '3.1 Knowledge of motor types (AC, DC, Servo, single phase, 3 phase)', 3),
    ('motor-construction', 'electrical', '3.2 Understanding of motor construction (stator, rotor, windings, bearings)', 3),
    ('motor-ratings', 'electrical', '3.3 Able to identify motor ratings (voltage, current, power, speed)', 3),
    ('motor-winding', 'electrical', '3.4 Able to identify winding connections (star, delta etc)', 3),
    ('motor-testing', 'electrical', '3.5 Able to test serviceability using insulation resistance and multimeter', 3);

-- 4. Inverters
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('inverter-types', 'electrical', '4.1 Knowledge of inverter types (single phase, 3 phase)', 3),
    ('inverter-programming', 'electrical', '4.2 Familiar with inverter programming and parameter setting', 3),
    ('inverter-troubleshooting', 'electrical', '4.3 Able to interpret fault codes and troubleshoot issues', 3),
    ('inverter-testing', 'electrical', '4.4 Knowledge of inverter testing (insulation resistance, multimeter)', 3),
    ('inverter-replacement', 'electrical', '4.5 Able to identify and replace with suitable spares', 3);

-- 5. Control Panels
INSERT INTO core_skills (id, category_id, name, max_score) VALUES
    ('panel-components', 'electrical', '5.1 Able to identify components (contactors, power supplies, relays)', 3),
    ('panel-drawings', 'electrical', '5.2 Able to read and interpret control panel drawings and documentation', 3),
    ('panel-modification', 'electrical', '5.3 Able to perform component installation/modification per drawings', 3);

GO

PRINT 'Electrical Skills updated successfully!';
PRINT 'Categories: 5 (Electrical Knowledge, Electrical Hand Skills, Motors, Inverters, Control Panels)';
PRINT 'Total Skills: 20';
GO

-- Verify the update
SELECT
    cs.category_id,
    COUNT(*) AS SkillCount
FROM core_skills cs
WHERE cs.category_id = 'electrical'
GROUP BY cs.category_id;

SELECT
    cs.id,
    cs.name,
    cs.max_score
FROM core_skills cs
WHERE cs.category_id = 'electrical'
ORDER BY cs.id;
GO
