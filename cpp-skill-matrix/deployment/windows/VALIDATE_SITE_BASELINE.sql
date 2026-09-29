/*
  Aptitude site-baseline validation.
  Run after SEED_SITE_BASELINE.sql and Palletiser_2ABC_Viaflo_Packing.sql.
*/

SET NOCOUNT ON;

IF DB_NAME() IN (N'master', N'model', N'msdb', N'tempdb')
    THROW 51210, 'Connect to the target Aptitude database before validating the site baseline.', 1;

DECLARE @ExpectedAreas TABLE ([name] NVARCHAR(200) NOT NULL PRIMARY KEY);
INSERT INTO @ExpectedAreas ([name]) VALUES
    (N'Viaflo Fill'),
    (N'L7 Packing'),
    (N'LINE 1'),
    (N'LINE 6'),
    (N'VIAFLO PACKING'),
    (N'Viaflo Racking');

IF EXISTS (
    SELECT 1
    FROM @ExpectedAreas AS expected
    WHERE NOT EXISTS (
        SELECT 1
        FROM [dbo].[production_areas] AS existing
        WHERE UPPER(existing.[name]) = UPPER(expected.[name])
    )
)
    THROW 51211, 'One or more expected production areas are missing. Run SEED_SITE_BASELINE.sql.', 1;

IF (SELECT COUNT(*) FROM [dbo].[machines]) < 36
    THROW 51212, 'The site baseline is incomplete: expected at least 36 machines.', 1;

IF (SELECT COUNT(*) FROM [dbo].[competencies]) < 120
    THROW 51213, 'The site baseline is incomplete: expected at least 120 production questions.', 1;

IF NOT EXISTS (
    SELECT 1
    FROM [dbo].[production_areas] AS area
    JOIN [dbo].[machines] AS machine ON machine.[production_area_id] = area.[id]
    WHERE UPPER(area.[name]) = N'VIAFLO PACKING'
      AND machine.[name] = N'Palletiser 2ABC'
)
    THROW 51214, 'Palletiser 2ABC is missing. Run Palletiser_2ABC_Viaflo_Packing.sql.', 1;

IF (
    SELECT COUNT(*)
    FROM [dbo].[production_areas] AS area
    JOIN [dbo].[machines] AS machine ON machine.[production_area_id] = area.[id]
    JOIN [dbo].[competencies] AS competency ON competency.[machine_id] = machine.[id]
    WHERE UPPER(area.[name]) = N'VIAFLO PACKING'
      AND machine.[name] = N'Palletiser 2ABC'
) < 20
    THROW 51215, 'Palletiser 2ABC is incomplete: expected 20 production questions.', 1;

SELECT
    (SELECT COUNT(*) FROM [dbo].[production_areas]) AS [production_area_count],
    (SELECT COUNT(*) FROM [dbo].[machines]) AS [machine_count],
    (SELECT COUNT(*) FROM [dbo].[competencies]) AS [production_question_count];

PRINT 'Aptitude site baseline validation passed.';
GO
