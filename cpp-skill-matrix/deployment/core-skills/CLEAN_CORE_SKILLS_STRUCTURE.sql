USE [training_matrix];
GO

SET XACT_ABORT ON;
IF COL_LENGTH('dbo.core_skill_categories', 'discipline') IS NULL
    ALTER TABLE dbo.core_skill_categories ADD discipline NVARCHAR(20) NULL;
GO

SET XACT_ABORT ON;
BEGIN TRANSACTION;

-- One-time safety copies retain the exact pre-cleanup records and assessment links.
IF OBJECT_ID('dbo.core_skill_categories_backup_20260619', 'U') IS NULL
    SELECT * INTO dbo.core_skill_categories_backup_20260619 FROM dbo.core_skill_categories;
IF OBJECT_ID('dbo.core_skills_backup_20260619', 'U') IS NULL
    SELECT * INTO dbo.core_skills_backup_20260619 FROM dbo.core_skills;
IF OBJECT_ID('dbo.core_skill_assessments_backup_20260619', 'U') IS NULL
    SELECT * INTO dbo.core_skill_assessments_backup_20260619 FROM dbo.core_skill_assessments;

UPDATE dbo.core_skill_categories
SET discipline = CASE
    WHEN LOWER(id + ' ' + name) LIKE '%mech%' THEN 'Mechanical'
    WHEN LOWER(id + ' ' + name) LIKE '%elec%' THEN 'Electrical'
    WHEN LOWER(id + ' ' + name) LIKE '%soft%' THEN 'Software'
    WHEN LOWER(id + ' ' + name) LIKE '%safety%' THEN 'Compliance'
    ELSE 'Other'
END
WHERE discipline IS NULL OR LTRIM(RTRIM(discipline)) = '';

-- Named non-technical groups must never be absorbed by the word "soft".
UPDATE dbo.core_skill_categories SET discipline = 'Compliance' WHERE LOWER(id) = 'safety';
UPDATE dbo.core_skill_categories SET discipline = 'Other' WHERE LOWER(id) = 'leadership';

-- Remove the repeated topic prefix from competency wording before topic names are cleaned.
UPDATE skill
SET skill.name = LTRIM(SUBSTRING(skill.name, LEN(topic.topic_name) + 4, 4000))
FROM dbo.core_skills skill
JOIN dbo.core_skill_categories category ON category.id = skill.category_id
CROSS APPLY (VALUES (
    LTRIM(CASE
        WHEN CHARINDEX(' - ', category.name) > 0
            THEN SUBSTRING(category.name, CHARINDEX(' - ', category.name) + 3, 4000)
        ELSE category.name
    END)
)) topic(topic_name)
WHERE skill.name LIKE topic.topic_name + ' - %';

-- Topic labels should not repeat the discipline or spreadsheet section number.
UPDATE dbo.core_skill_categories
SET name = LTRIM(CASE
    WHEN CHARINDEX(' - ', name) > 0 THEN SUBSTRING(name, CHARINDEX(' - ', name) + 3, 4000)
    ELSE name
END)
WHERE discipline IN ('Mechanical', 'Electrical', 'Software');

UPDATE dbo.core_skill_categories
SET name = LTRIM(SUBSTRING(name, CHARINDEX(' ', name) + 1, 4000))
WHERE discipline = 'Mechanical'
  AND name LIKE '[0-9].[0-9] %';

UPDATE dbo.core_skill_categories
SET name = 'General'
WHERE LOWER(id) IN ('mechanical', 'electrical', 'software');

-- These imported rows describe the score rubric; they are not assessable competencies.
DECLARE @rubric_skills TABLE (id NVARCHAR(50) PRIMARY KEY);
INSERT INTO @rubric_skills (id) VALUES
('elec_07bb8c452c30aea5'), ('elec_7f2250e6eafa94eb'),
('elec_ef65ee5458afd5f2'), ('elec_42b6ea6d3b30dc4d'),
('elec_615d5dd91408bce2'), ('mech_62ceae0a938df482'),
('mech_65c4fa5f6b502a6a'), ('mech_be59028beaf3b09a'),
('mech_f310f27c57c49e3d'), ('soft_7872edd0651261b5'),
('soft_8716166a2c9ebb26'), ('soft_20db5a98a37d631e');

DELETE assessment
FROM dbo.core_skill_assessments assessment
JOIN @rubric_skills rubric ON rubric.id = assessment.skill_id;

DELETE skill
FROM dbo.core_skills skill
JOIN @rubric_skills rubric ON rubric.id = skill.id;

-- Conservative corrections only; wording is otherwise preserved verbatim.
UPDATE dbo.core_skills
SET name = REPLACE(REPLACE(REPLACE(REPLACE(name,
    'indentify', 'identify'),
    'occurences', 'occurrences'),
    'invertor', 'inverter'),
    'less that', 'less than');

COMMIT TRANSACTION;
GO

SELECT discipline, COUNT(*) AS topic_count
FROM dbo.core_skill_categories
GROUP BY discipline
ORDER BY discipline;

SELECT COUNT(*) AS competency_count FROM dbo.core_skills;
SELECT COUNT(*) AS assessment_count FROM dbo.core_skill_assessments;
GO
