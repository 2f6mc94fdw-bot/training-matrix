-- Multi-Criteria Weighting System Migration
-- Adds weighting columns to competencies and core_skills tables
-- Based on Multi-Criteria Weighted Scoring methodology

USE training_matrix;
GO

-- Add weighting columns to competencies table
IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND name = 'safety_impact')
BEGIN
    ALTER TABLE [dbo].[competencies] ADD
        [safety_impact] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [production_impact] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [frequency] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [complexity] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [future_value] DECIMAL(3,1) DEFAULT 3.0 NOT NULL;

    PRINT 'Added multi-criteria weighting columns to competencies table';
END
GO

-- Add weighting columns to core_skills table
IF NOT EXISTS (SELECT * FROM sys.columns WHERE object_id = OBJECT_ID(N'[dbo].[core_skills]') AND name = 'safety_impact')
BEGIN
    ALTER TABLE [dbo].[core_skills] ADD
        [safety_impact] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [production_impact] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [frequency] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [complexity] DECIMAL(3,1) DEFAULT 3.0 NOT NULL,
        [future_value] DECIMAL(3,1) DEFAULT 3.0 NOT NULL;

    PRINT 'Added multi-criteria weighting columns to core_skills table';
END
GO

-- Create view for calculated competency weights
IF OBJECT_ID('[dbo].[v_competency_weights]', 'V') IS NOT NULL
    DROP VIEW [dbo].[v_competency_weights];
GO

CREATE VIEW [dbo].[v_competency_weights] AS
SELECT
    id,
    name,
    machine_id,
    max_score,
    safety_impact,
    production_impact,
    frequency,
    complexity,
    future_value,
    -- Calculate final weight (0.0 - 5.0 scale for each criterion)
    ROUND(
        (safety_impact * 0.30) +
        (production_impact * 0.25) +
        (frequency * 0.20) +
        (complexity * 0.15) +
        (future_value * 0.10),
        2
    ) AS calculated_weight
FROM [dbo].[competencies];
GO

-- Create view for calculated core skill weights
IF OBJECT_ID('[dbo].[v_core_skill_weights]', 'V') IS NOT NULL
    DROP VIEW [dbo].[v_core_skill_weights];
GO

CREATE VIEW [dbo].[v_core_skill_weights] AS
SELECT
    id,
    name,
    category_id,
    max_score,
    safety_impact,
    production_impact,
    frequency,
    complexity,
    future_value,
    -- Calculate final weight (0.0 - 5.0 scale for each criterion)
    ROUND(
        (safety_impact * 0.30) +
        (production_impact * 0.25) +
        (frequency * 0.20) +
        (complexity * 0.15) +
        (future_value * 0.10),
        2
    ) AS calculated_weight
FROM [dbo].[core_skills];
GO

PRINT 'Multi-criteria weighting system migration completed successfully';
PRINT 'Weighting Scale: 0.0 (Not Applicable) to 5.0 (Critical)';
PRINT 'Weight Distribution: Safety 30%, Production 25%, Frequency 20%, Complexity 15%, Future Value 10%';
GO
