/*
  Aptitude database deployment validation.
  Run while connected to the target database after schema and migration scripts.
  Any failed requirement stops the deployment.
*/

SET NOCOUNT ON;

IF DB_NAME() IN (N'master', N'model', N'msdb', N'tempdb')
BEGIN
    THROW 51100, 'Connect to the target Aptitude database before running validation.', 1;
END;

DECLARE @RequiredSchemaVersion BIGINT = 2026072701;

IF OBJECT_ID(N'dbo.app_schema_versions', N'U') IS NULL
    THROW 51101, 'Missing dbo.app_schema_versions. Run PRODUCTION_SCHEMA_MIGRATION.sql.', 1;

IF NOT EXISTS (SELECT 1 FROM [dbo].[app_schema_versions] WHERE [version] >= @RequiredSchemaVersion)
    THROW 51102, 'The Aptitude database schema is older than this client package.', 1;

IF EXISTS (
    SELECT required_tables.[table_name]
    FROM (VALUES
        (N'users'), (N'engineers'), (N'production_areas'), (N'machines'),
        (N'competencies'), (N'assessments'), (N'core_skill_categories'),
        (N'core_skills'), (N'core_skill_assessments'), (N'certifications'),
        (N'snapshots'), (N'audit_logs'), (N'targets'), (N'notifications'),
        (N'development_plan_items'), (N'assessment_submissions')
    ) required_tables([table_name])
    WHERE OBJECT_ID(N'dbo.' + required_tables.[table_name], N'U') IS NULL
)
    THROW 51103, 'One or more required Aptitude tables are missing.', 1;

IF COL_LENGTH('dbo.competencies', 'safety_impact') IS NULL
    OR COL_LENGTH('dbo.competencies', 'production_impact') IS NULL
    OR COL_LENGTH('dbo.competencies', 'frequency') IS NULL
    OR COL_LENGTH('dbo.competencies', 'complexity') IS NULL
    OR COL_LENGTH('dbo.competencies', 'future_value') IS NULL
    THROW 51104, 'Production competency weighting columns are missing.', 1;

IF COL_LENGTH('dbo.core_skills', 'safety_impact') IS NULL
    OR COL_LENGTH('dbo.core_skills', 'production_impact') IS NULL
    OR COL_LENGTH('dbo.core_skills', 'frequency') IS NULL
    OR COL_LENGTH('dbo.core_skills', 'complexity') IS NULL
    OR COL_LENGTH('dbo.core_skills', 'future_value') IS NULL
    THROW 51105, 'Core-skill weighting columns are missing.', 1;

IF COL_LENGTH('dbo.core_skill_categories', 'discipline') IS NULL
    THROW 51106, 'Core-skill discipline support is missing.', 1;

IF COL_LENGTH('dbo.certifications', 'certificate_file_path') IS NULL
    THROW 51107, 'Certification attachment support is missing.', 1;

IF COL_LENGTH('dbo.assessment_submissions', 'row_version') IS NULL
    THROW 51114, 'Assessment submission concurrency support is missing.', 1;

IF EXISTS (SELECT 1 FROM [dbo].[assessments] WHERE [score] NOT BETWEEN 0 AND 3)
    OR EXISTS (SELECT 1 FROM [dbo].[core_skill_assessments] WHERE [score] NOT BETWEEN 0 AND 3)
    THROW 51108, 'One or more assessment scores are outside the supported 0-3 range.', 1;

IF EXISTS (
    SELECT 1 FROM [dbo].[competencies]
    WHERE [safety_impact] NOT BETWEEN 0 AND 5
       OR [production_impact] NOT BETWEEN 0 AND 5
       OR [frequency] NOT BETWEEN 0 AND 5
       OR [complexity] NOT BETWEEN 0 AND 5
       OR [future_value] NOT BETWEEN 0 AND 5
)
    OR EXISTS (
        SELECT 1 FROM [dbo].[core_skills]
        WHERE [safety_impact] NOT BETWEEN 0 AND 5
           OR [production_impact] NOT BETWEEN 0 AND 5
           OR [frequency] NOT BETWEEN 0 AND 5
           OR [complexity] NOT BETWEEN 0 AND 5
           OR [future_value] NOT BETWEEN 0 AND 5
    )
    THROW 51109, 'One or more weighting values are outside the supported 0-5 range.', 1;

IF EXISTS (
    SELECT 1 FROM [dbo].[assessments]
    GROUP BY [engineer_id], [production_area_id], [machine_id], [competency_id]
    HAVING COUNT(*) > 1
)
    THROW 51110, 'Duplicate production assessment records were detected.', 1;

IF EXISTS (
    SELECT 1 FROM [dbo].[core_skill_assessments]
    GROUP BY [engineer_id], [category_id], [skill_id]
    HAVING COUNT(*) > 1
)
    THROW 51111, 'Duplicate core-skill assessment records were detected.', 1;

IF EXISTS (
    SELECT 1
    FROM [dbo].[users] u
    LEFT JOIN [dbo].[engineers] e ON e.[id] = u.[engineer_id]
    WHERE u.[engineer_id] IS NOT NULL AND e.[id] IS NULL
)
    THROW 51112, 'User accounts contain orphan engineer links.', 1;

IF EXISTS (
    SELECT required_constraints.[constraint_name]
    FROM (VALUES
        (N'FK_users_engineers'),
        (N'UQ_assessments'),
        (N'UQ_core_skill_assessments'),
        (N'CK_assessments_score_range'),
        (N'CK_core_skill_assessments_score_range'),
        (N'CK_competencies_weight_ranges'),
        (N'CK_core_skills_weight_ranges'),
        (N'CK_development_plan_scores'),
        (N'CK_development_plan_status'),
        (N'CK_assessment_submissions_type'),
        (N'CK_assessment_submissions_decision'),
        (N'CK_assessment_submissions_scores'),
        (N'CK_assessment_submissions_shape')
    ) required_constraints([constraint_name])
    WHERE OBJECT_ID(N'dbo.' + required_constraints.[constraint_name], N'F') IS NULL
      AND OBJECT_ID(N'dbo.' + required_constraints.[constraint_name], N'UQ') IS NULL
      AND OBJECT_ID(N'dbo.' + required_constraints.[constraint_name], N'C') IS NULL
)
    THROW 51113, 'One or more required database constraints are missing.', 1;

IF NOT EXISTS (
    SELECT 1 FROM sys.indexes
    WHERE [name] = 'UX_assessment_submissions_pending'
      AND [object_id] = OBJECT_ID('dbo.assessment_submissions')
      AND [is_unique] = 1
      AND [has_filter] = 1
)
    THROW 51115, 'The one-pending-assessment-submission rule is missing.', 1;

SELECT
    DB_NAME() AS [database_name],
    MAX([version]) AS [schema_version],
    COUNT_BIG(*) AS [applied_migrations]
FROM [dbo].[app_schema_versions];

PRINT 'Aptitude database validation passed.';
GO
