-- Add multi-criteria weighting columns to competencies table
-- These columns are used by the C++ Aptitude application

USE training_matrix;
GO

-- Check if columns exist and add them if they don't
IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND name = 'safety_impact')
BEGIN
    ALTER TABLE [dbo].[competencies]
    ADD [safety_impact] FLOAT NOT NULL DEFAULT 3.0;
    PRINT 'Added safety_impact column';
END
ELSE
BEGIN
    PRINT 'safety_impact column already exists';
END
GO

IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND name = 'production_impact')
BEGIN
    ALTER TABLE [dbo].[competencies]
    ADD [production_impact] FLOAT NOT NULL DEFAULT 3.0;
    PRINT 'Added production_impact column';
END
ELSE
BEGIN
    PRINT 'production_impact column already exists';
END
GO

IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND name = 'frequency')
BEGIN
    ALTER TABLE [dbo].[competencies]
    ADD [frequency] FLOAT NOT NULL DEFAULT 3.0;
    PRINT 'Added frequency column';
END
ELSE
BEGIN
    PRINT 'frequency column already exists';
END
GO

IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND name = 'complexity')
BEGIN
    ALTER TABLE [dbo].[competencies]
    ADD [complexity] FLOAT NOT NULL DEFAULT 3.0;
    PRINT 'Added complexity column';
END
ELSE
BEGIN
    PRINT 'complexity column already exists';
END
GO

IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND name = 'future_value')
BEGIN
    ALTER TABLE [dbo].[competencies]
    ADD [future_value] FLOAT NOT NULL DEFAULT 3.0;
    PRINT 'Added future_value column';
END
ELSE
BEGIN
    PRINT 'future_value column already exists';
END
GO

PRINT 'Migration complete! Multi-criteria weighting columns added to competencies table.';
PRINT 'All existing competencies now have default values of 3.0 for each weighting criteria.';
GO
