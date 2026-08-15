/*
  Aptitude production schema migration
  Latest version: 2026072701

  Run this script while connected to the target Aptitude database. It is
  transactional and safe to run more than once. Do not run it against master.
*/

SET NOCOUNT ON;
SET XACT_ABORT ON;
SET ANSI_NULLS ON;
SET QUOTED_IDENTIFIER ON;

IF DB_NAME() IN (N'master', N'model', N'msdb', N'tempdb')
BEGIN
    THROW 51000, 'Connect to the target Aptitude database before running this migration.', 1;
END;

BEGIN TRY
    BEGIN TRANSACTION;

    IF EXISTS (
        SELECT required_tables.[table_name]
        FROM (VALUES
            (N'users'), (N'engineers'), (N'production_areas'), (N'machines'),
            (N'competencies'), (N'assessments'), (N'core_skill_categories'),
            (N'core_skills'), (N'core_skill_assessments'), (N'certifications'),
            (N'snapshots'), (N'audit_logs'), (N'targets')
        ) required_tables([table_name])
        WHERE OBJECT_ID(N'dbo.' + required_tables.[table_name], N'U') IS NULL
    )
    BEGIN
        THROW 51001, 'Base Aptitude tables are missing. Run schema.sql before this migration.', 1;
    END;

    IF OBJECT_ID(N'dbo.app_schema_versions', N'U') IS NULL
    BEGIN
        CREATE TABLE [dbo].[app_schema_versions] (
            [version] BIGINT NOT NULL PRIMARY KEY,
            [description] NVARCHAR(250) NOT NULL,
            [applied_at] DATETIME2 NOT NULL CONSTRAINT [DF_app_schema_versions_applied_at] DEFAULT(SYSUTCDATETIME())
        );
    END;

    IF COL_LENGTH('dbo.certifications', 'certificate_file_path') IS NULL
        ALTER TABLE [dbo].[certifications] ADD [certificate_file_path] NVARCHAR(500) NULL;

    IF COL_LENGTH('dbo.competencies', 'safety_impact') IS NULL
        ALTER TABLE [dbo].[competencies] ADD [safety_impact] FLOAT NOT NULL CONSTRAINT [DF_competencies_safety_impact] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.competencies', 'production_impact') IS NULL
        ALTER TABLE [dbo].[competencies] ADD [production_impact] FLOAT NOT NULL CONSTRAINT [DF_competencies_production_impact] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.competencies', 'frequency') IS NULL
        ALTER TABLE [dbo].[competencies] ADD [frequency] FLOAT NOT NULL CONSTRAINT [DF_competencies_frequency] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.competencies', 'complexity') IS NULL
        ALTER TABLE [dbo].[competencies] ADD [complexity] FLOAT NOT NULL CONSTRAINT [DF_competencies_complexity] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.competencies', 'future_value') IS NULL
        ALTER TABLE [dbo].[competencies] ADD [future_value] FLOAT NOT NULL CONSTRAINT [DF_competencies_future_value] DEFAULT(3.0) WITH VALUES;

    IF COL_LENGTH('dbo.core_skills', 'safety_impact') IS NULL
        ALTER TABLE [dbo].[core_skills] ADD [safety_impact] FLOAT NOT NULL CONSTRAINT [DF_core_skills_safety_impact] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.core_skills', 'production_impact') IS NULL
        ALTER TABLE [dbo].[core_skills] ADD [production_impact] FLOAT NOT NULL CONSTRAINT [DF_core_skills_production_impact] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.core_skills', 'frequency') IS NULL
        ALTER TABLE [dbo].[core_skills] ADD [frequency] FLOAT NOT NULL CONSTRAINT [DF_core_skills_frequency] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.core_skills', 'complexity') IS NULL
        ALTER TABLE [dbo].[core_skills] ADD [complexity] FLOAT NOT NULL CONSTRAINT [DF_core_skills_complexity] DEFAULT(3.0) WITH VALUES;
    IF COL_LENGTH('dbo.core_skills', 'future_value') IS NULL
        ALTER TABLE [dbo].[core_skills] ADD [future_value] FLOAT NOT NULL CONSTRAINT [DF_core_skills_future_value] DEFAULT(3.0) WITH VALUES;

    IF COL_LENGTH('dbo.core_skill_categories', 'discipline') IS NULL
        ALTER TABLE [dbo].[core_skill_categories] ADD [discipline] NVARCHAR(20) NULL;

    IF OBJECT_ID(N'dbo.notifications', N'U') IS NULL
    BEGIN
        CREATE TABLE [dbo].[notifications] (
            [id] NVARCHAR(50) NOT NULL PRIMARY KEY,
            [engineer_id] NVARCHAR(50) NULL,
            [title] NVARCHAR(200) NOT NULL,
            [message] NVARCHAR(MAX) NOT NULL,
            [is_read] BIT NOT NULL CONSTRAINT [DF_notifications_is_read] DEFAULT(0),
            [created_at] DATETIME2 NOT NULL CONSTRAINT [DF_notifications_created_at] DEFAULT(SYSUTCDATETIME()),
            [read_at] DATETIME2 NULL,
            [created_by_user_id] NVARCHAR(50) NULL,
            [created_by_name] NVARCHAR(100) NULL,
            [recipient_user_id] NVARCHAR(50) NULL
        );
    END;

    IF COL_LENGTH('dbo.notifications', 'recipient_user_id') IS NULL
        ALTER TABLE [dbo].[notifications] ADD [recipient_user_id] NVARCHAR(50) NULL;

    IF EXISTS (
        SELECT 1 FROM sys.columns
        WHERE [object_id] = OBJECT_ID('dbo.notifications')
          AND [name] = 'engineer_id'
          AND [is_nullable] = 0
    )
        ALTER TABLE [dbo].[notifications] ALTER COLUMN [engineer_id] NVARCHAR(50) NULL;

    IF OBJECT_ID(N'dbo.development_plan_items', N'U') IS NULL
    BEGIN
        CREATE TABLE [dbo].[development_plan_items] (
            [id] INT IDENTITY(1,1) NOT NULL PRIMARY KEY,
            [engineer_id] NVARCHAR(50) NOT NULL,
            [production_area_id] INT NOT NULL,
            [machine_id] INT NOT NULL,
            [competency_id] INT NOT NULL,
            [current_score] INT NOT NULL CONSTRAINT [DF_development_plan_current_score] DEFAULT(0),
            [target_score] INT NOT NULL CONSTRAINT [DF_development_plan_target_score] DEFAULT(2),
            [due_date] DATE NULL,
            [manager_notes] NVARCHAR(1000) NULL,
            [guidance] NVARCHAR(1500) NULL,
            [status] NVARCHAR(20) NOT NULL CONSTRAINT [DF_development_plan_status] DEFAULT('active'),
            [created_by_user_id] NVARCHAR(50) NULL,
            [created_by_name] NVARCHAR(100) NULL,
            [created_at] DATETIME2 NOT NULL CONSTRAINT [DF_development_plan_created_at] DEFAULT(SYSUTCDATETIME()),
            [updated_at] DATETIME2 NOT NULL CONSTRAINT [DF_development_plan_updated_at] DEFAULT(SYSUTCDATETIME())
        );
    END;

    IF OBJECT_ID(N'dbo.assessment_submissions', N'U') IS NULL
    BEGIN
        CREATE TABLE [dbo].[assessment_submissions] (
            [id] NVARCHAR(50) NOT NULL PRIMARY KEY,
            [review_key] NVARCHAR(220) NOT NULL,
            [assessment_type] NVARCHAR(20) NOT NULL,
            [engineer_id] NVARCHAR(50) NOT NULL,
            [production_area_id] INT NULL,
            [machine_id] INT NULL,
            [competency_id] INT NULL,
            [category_id] NVARCHAR(50) NULL,
            [skill_id] NVARCHAR(50) NULL,
            [proposed_score] INT NOT NULL,
            [base_score] INT NULL,
            [submission_note] NVARCHAR(500) NULL,
            [submitted_by_user_id] NVARCHAR(50) NULL,
            [submitted_by_name] NVARCHAR(100) NULL,
            [submitted_at] DATETIME2 NOT NULL CONSTRAINT [DF_assessment_submissions_submitted_at] DEFAULT(SYSUTCDATETIME()),
            [decision] NVARCHAR(20) NOT NULL CONSTRAINT [DF_assessment_submissions_decision] DEFAULT('pending'),
            [review_note] NVARCHAR(500) NULL,
            [reviewed_by_user_id] NVARCHAR(50) NULL,
            [reviewed_by_name] NVARCHAR(100) NULL,
            [reviewed_at] DATETIME2 NULL,
            [updated_at] DATETIME2 NOT NULL CONSTRAINT [DF_assessment_submissions_updated_at] DEFAULT(SYSUTCDATETIME()),
            [row_version] ROWVERSION NOT NULL,
            CONSTRAINT [CK_assessment_submissions_type] CHECK ([assessment_type] IN ('production','core')),
            CONSTRAINT [CK_assessment_submissions_decision] CHECK ([decision] IN ('pending','approved','rejected')),
            CONSTRAINT [CK_assessment_submissions_scores] CHECK ([proposed_score] BETWEEN 0 AND 3 AND ([base_score] IS NULL OR [base_score] BETWEEN 0 AND 3)),
            CONSTRAINT [CK_assessment_submissions_shape] CHECK (
                ([assessment_type] = 'production' AND [production_area_id] IS NOT NULL AND [machine_id] IS NOT NULL AND [competency_id] IS NOT NULL AND [category_id] IS NULL AND [skill_id] IS NULL)
                OR
                ([assessment_type] = 'core' AND [production_area_id] IS NULL AND [machine_id] IS NULL AND [competency_id] IS NULL AND [category_id] IS NOT NULL AND [skill_id] IS NOT NULL)
            ),
            CONSTRAINT [FK_assessment_submissions_engineer] FOREIGN KEY ([engineer_id]) REFERENCES [dbo].[engineers]([id]),
            CONSTRAINT [FK_assessment_submissions_area] FOREIGN KEY ([production_area_id]) REFERENCES [dbo].[production_areas]([id]),
            CONSTRAINT [FK_assessment_submissions_machine] FOREIGN KEY ([machine_id]) REFERENCES [dbo].[machines]([id]),
            CONSTRAINT [FK_assessment_submissions_competency] FOREIGN KEY ([competency_id]) REFERENCES [dbo].[competencies]([id]),
            CONSTRAINT [FK_assessment_submissions_category] FOREIGN KEY ([category_id]) REFERENCES [dbo].[core_skill_categories]([id]),
            CONSTRAINT [FK_assessment_submissions_skill] FOREIGN KEY ([skill_id]) REFERENCES [dbo].[core_skills]([id]),
            CONSTRAINT [FK_assessment_submissions_submitted_by] FOREIGN KEY ([submitted_by_user_id]) REFERENCES [dbo].[users]([id]),
            CONSTRAINT [FK_assessment_submissions_reviewed_by] FOREIGN KEY ([reviewed_by_user_id]) REFERENCES [dbo].[users]([id])
        );
    END;

    UPDATE [dbo].[assessments]
    SET [score] = CASE WHEN [score] < 0 THEN 0 WHEN [score] > 3 THEN 3 ELSE [score] END
    WHERE [score] < 0 OR [score] > 3;

    UPDATE [dbo].[core_skill_assessments]
    SET [score] = CASE WHEN [score] < 0 THEN 0 WHEN [score] > 3 THEN 3 ELSE [score] END
    WHERE [score] < 0 OR [score] > 3;

    UPDATE [dbo].[competencies]
    SET [safety_impact] = CASE WHEN [safety_impact] < 0 THEN 0 WHEN [safety_impact] > 5 THEN 5 ELSE [safety_impact] END,
        [production_impact] = CASE WHEN [production_impact] < 0 THEN 0 WHEN [production_impact] > 5 THEN 5 ELSE [production_impact] END,
        [frequency] = CASE WHEN [frequency] < 0 THEN 0 WHEN [frequency] > 5 THEN 5 ELSE [frequency] END,
        [complexity] = CASE WHEN [complexity] < 0 THEN 0 WHEN [complexity] > 5 THEN 5 ELSE [complexity] END,
        [future_value] = CASE WHEN [future_value] < 0 THEN 0 WHEN [future_value] > 5 THEN 5 ELSE [future_value] END
    WHERE [safety_impact] NOT BETWEEN 0 AND 5
       OR [production_impact] NOT BETWEEN 0 AND 5
       OR [frequency] NOT BETWEEN 0 AND 5
       OR [complexity] NOT BETWEEN 0 AND 5
       OR [future_value] NOT BETWEEN 0 AND 5;

    UPDATE [dbo].[core_skills]
    SET [safety_impact] = CASE WHEN [safety_impact] < 0 THEN 0 WHEN [safety_impact] > 5 THEN 5 ELSE [safety_impact] END,
        [production_impact] = CASE WHEN [production_impact] < 0 THEN 0 WHEN [production_impact] > 5 THEN 5 ELSE [production_impact] END,
        [frequency] = CASE WHEN [frequency] < 0 THEN 0 WHEN [frequency] > 5 THEN 5 ELSE [frequency] END,
        [complexity] = CASE WHEN [complexity] < 0 THEN 0 WHEN [complexity] > 5 THEN 5 ELSE [complexity] END,
        [future_value] = CASE WHEN [future_value] < 0 THEN 0 WHEN [future_value] > 5 THEN 5 ELSE [future_value] END
    WHERE [safety_impact] NOT BETWEEN 0 AND 5
       OR [production_impact] NOT BETWEEN 0 AND 5
       OR [frequency] NOT BETWEEN 0 AND 5
       OR [complexity] NOT BETWEEN 0 AND 5
       OR [future_value] NOT BETWEEN 0 AND 5;

    UPDATE [dbo].[development_plan_items]
    SET [current_score] = CASE WHEN [current_score] < 0 THEN 0 WHEN [current_score] > 3 THEN 3 ELSE [current_score] END,
        [target_score] = CASE WHEN [target_score] < 0 THEN 0 WHEN [target_score] > 3 THEN 3 ELSE [target_score] END,
        [status] = CASE WHEN [status] IN ('active', 'completed', 'cancelled') THEN [status] ELSE 'active' END
    WHERE [current_score] NOT BETWEEN 0 AND 3
       OR [target_score] NOT BETWEEN 0 AND 3
       OR [status] NOT IN ('active', 'completed', 'cancelled');

    IF NOT EXISTS (SELECT 1 FROM sys.check_constraints WHERE [name] = 'CK_assessments_score_range' AND [parent_object_id] = OBJECT_ID('dbo.assessments'))
        ALTER TABLE [dbo].[assessments] WITH CHECK ADD CONSTRAINT [CK_assessments_score_range] CHECK ([score] BETWEEN 0 AND 3);
    IF NOT EXISTS (SELECT 1 FROM sys.check_constraints WHERE [name] = 'CK_core_skill_assessments_score_range' AND [parent_object_id] = OBJECT_ID('dbo.core_skill_assessments'))
        ALTER TABLE [dbo].[core_skill_assessments] WITH CHECK ADD CONSTRAINT [CK_core_skill_assessments_score_range] CHECK ([score] BETWEEN 0 AND 3);
    IF NOT EXISTS (SELECT 1 FROM sys.check_constraints WHERE [name] = 'CK_competencies_weight_ranges' AND [parent_object_id] = OBJECT_ID('dbo.competencies'))
        ALTER TABLE [dbo].[competencies] WITH CHECK ADD CONSTRAINT [CK_competencies_weight_ranges] CHECK ([safety_impact] BETWEEN 0 AND 5 AND [production_impact] BETWEEN 0 AND 5 AND [frequency] BETWEEN 0 AND 5 AND [complexity] BETWEEN 0 AND 5 AND [future_value] BETWEEN 0 AND 5);
    IF NOT EXISTS (SELECT 1 FROM sys.check_constraints WHERE [name] = 'CK_core_skills_weight_ranges' AND [parent_object_id] = OBJECT_ID('dbo.core_skills'))
        ALTER TABLE [dbo].[core_skills] WITH CHECK ADD CONSTRAINT [CK_core_skills_weight_ranges] CHECK ([safety_impact] BETWEEN 0 AND 5 AND [production_impact] BETWEEN 0 AND 5 AND [frequency] BETWEEN 0 AND 5 AND [complexity] BETWEEN 0 AND 5 AND [future_value] BETWEEN 0 AND 5);
    IF NOT EXISTS (SELECT 1 FROM sys.check_constraints WHERE [name] = 'CK_development_plan_scores' AND [parent_object_id] = OBJECT_ID('dbo.development_plan_items'))
        ALTER TABLE [dbo].[development_plan_items] WITH CHECK ADD CONSTRAINT [CK_development_plan_scores] CHECK ([current_score] BETWEEN 0 AND 3 AND [target_score] BETWEEN 0 AND 3);
    IF NOT EXISTS (SELECT 1 FROM sys.check_constraints WHERE [name] = 'CK_development_plan_status' AND [parent_object_id] = OBJECT_ID('dbo.development_plan_items'))
        ALTER TABLE [dbo].[development_plan_items] WITH CHECK ADD CONSTRAINT [CK_development_plan_status] CHECK ([status] IN ('active','completed','cancelled'));

    IF EXISTS (
        SELECT 1
        FROM [dbo].[users] u
        LEFT JOIN [dbo].[engineers] e ON e.[id] = u.[engineer_id]
        WHERE u.[engineer_id] IS NOT NULL AND e.[id] IS NULL
    )
        THROW 51002, 'Users contain orphan engineer links. Repair them before rerunning this migration.', 1;

    IF EXISTS (
        SELECT 1 FROM [dbo].[development_plan_items] d
        LEFT JOIN [dbo].[engineers] e ON e.[id] = d.[engineer_id]
        LEFT JOIN [dbo].[production_areas] p ON p.[id] = d.[production_area_id]
        LEFT JOIN [dbo].[machines] m ON m.[id] = d.[machine_id]
        LEFT JOIN [dbo].[competencies] c ON c.[id] = d.[competency_id]
        WHERE e.[id] IS NULL OR p.[id] IS NULL OR m.[id] IS NULL OR c.[id] IS NULL
    )
        THROW 51004, 'Development-plan records contain orphan references.', 1;

    IF NOT EXISTS (SELECT 1 FROM sys.foreign_keys WHERE [name] = 'FK_users_engineers' AND [parent_object_id] = OBJECT_ID('dbo.users'))
        ALTER TABLE [dbo].[users] WITH CHECK ADD CONSTRAINT [FK_users_engineers] FOREIGN KEY ([engineer_id]) REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE;
    IF NOT EXISTS (SELECT 1 FROM sys.foreign_keys WHERE [name] = 'FK_development_plan_engineer' AND [parent_object_id] = OBJECT_ID('dbo.development_plan_items'))
        ALTER TABLE [dbo].[development_plan_items] WITH CHECK ADD CONSTRAINT [FK_development_plan_engineer] FOREIGN KEY ([engineer_id]) REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE;
    IF NOT EXISTS (SELECT 1 FROM sys.foreign_keys WHERE [name] = 'FK_development_plan_area' AND [parent_object_id] = OBJECT_ID('dbo.development_plan_items'))
        ALTER TABLE [dbo].[development_plan_items] WITH CHECK ADD CONSTRAINT [FK_development_plan_area] FOREIGN KEY ([production_area_id]) REFERENCES [dbo].[production_areas]([id]);
    IF NOT EXISTS (SELECT 1 FROM sys.foreign_keys WHERE [name] = 'FK_development_plan_machine' AND [parent_object_id] = OBJECT_ID('dbo.development_plan_items'))
        ALTER TABLE [dbo].[development_plan_items] WITH CHECK ADD CONSTRAINT [FK_development_plan_machine] FOREIGN KEY ([machine_id]) REFERENCES [dbo].[machines]([id]);
    IF NOT EXISTS (SELECT 1 FROM sys.foreign_keys WHERE [name] = 'FK_development_plan_competency' AND [parent_object_id] = OBJECT_ID('dbo.development_plan_items'))
        ALTER TABLE [dbo].[development_plan_items] WITH CHECK ADD CONSTRAINT [FK_development_plan_competency] FOREIGN KEY ([competency_id]) REFERENCES [dbo].[competencies]([id]);

    IF EXISTS (
        SELECT 1 FROM [dbo].[assessments]
        GROUP BY [engineer_id], [production_area_id], [machine_id], [competency_id]
        HAVING COUNT(*) > 1
    )
        THROW 51005, 'Duplicate production assessment rows must be resolved before migration.', 1;

    IF EXISTS (
        SELECT 1 FROM [dbo].[core_skill_assessments]
        GROUP BY [engineer_id], [category_id], [skill_id]
        HAVING COUNT(*) > 1
    )
        THROW 51006, 'Duplicate core-skill assessment rows must be resolved before migration.', 1;

    IF NOT EXISTS (SELECT 1 FROM sys.key_constraints WHERE [name] = 'UQ_assessments' AND [parent_object_id] = OBJECT_ID('dbo.assessments'))
        ALTER TABLE [dbo].[assessments] ADD CONSTRAINT [UQ_assessments] UNIQUE ([engineer_id], [production_area_id], [machine_id], [competency_id]);
    IF NOT EXISTS (SELECT 1 FROM sys.key_constraints WHERE [name] = 'UQ_core_skill_assessments' AND [parent_object_id] = OBJECT_ID('dbo.core_skill_assessments'))
        ALTER TABLE [dbo].[core_skill_assessments] ADD CONSTRAINT [UQ_core_skill_assessments] UNIQUE ([engineer_id], [category_id], [skill_id]);

    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_notifications_engineer_created' AND [object_id] = OBJECT_ID('dbo.notifications'))
        CREATE NONCLUSTERED INDEX [IX_notifications_engineer_created] ON [dbo].[notifications]([engineer_id], [created_at] DESC);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_development_plan_engineer_status' AND [object_id] = OBJECT_ID('dbo.development_plan_items'))
        CREATE NONCLUSTERED INDEX [IX_development_plan_engineer_status] ON [dbo].[development_plan_items]([engineer_id], [status], [due_date]);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_assessments_lookup' AND [object_id] = OBJECT_ID('dbo.assessments'))
        CREATE NONCLUSTERED INDEX [IX_assessments_lookup] ON [dbo].[assessments]([engineer_id], [production_area_id], [machine_id], [competency_id]);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_assessments_score_updated' AND [object_id] = OBJECT_ID('dbo.assessments'))
        CREATE NONCLUSTERED INDEX [IX_assessments_score_updated] ON [dbo].[assessments]([score], [updated_at] DESC) INCLUDE ([engineer_id], [production_area_id], [machine_id], [competency_id], [created_at]);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_assessments_area_machine_competency' AND [object_id] = OBJECT_ID('dbo.assessments'))
        CREATE NONCLUSTERED INDEX [IX_assessments_area_machine_competency] ON [dbo].[assessments]([production_area_id], [machine_id], [competency_id]) INCLUDE ([engineer_id], [score], [updated_at]);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_engineers_name' AND [object_id] = OBJECT_ID('dbo.engineers'))
        CREATE NONCLUSTERED INDEX [IX_engineers_name] ON [dbo].[engineers]([name]);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_production_areas_name' AND [object_id] = OBJECT_ID('dbo.production_areas'))
        CREATE NONCLUSTERED INDEX [IX_production_areas_name] ON [dbo].[production_areas]([name]);
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'UX_assessment_submissions_pending' AND [object_id] = OBJECT_ID('dbo.assessment_submissions'))
        CREATE UNIQUE NONCLUSTERED INDEX [UX_assessment_submissions_pending] ON [dbo].[assessment_submissions]([review_key]) WHERE [decision] = 'pending';
    IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE [name] = 'IX_assessment_submissions_queue' AND [object_id] = OBJECT_ID('dbo.assessment_submissions'))
        CREATE NONCLUSTERED INDEX [IX_assessment_submissions_queue] ON [dbo].[assessment_submissions]([decision], [submitted_at] DESC)
            INCLUDE ([engineer_id], [assessment_type], [proposed_score], [base_score]);

    UPDATE [dbo].[core_skill_categories]
    SET [discipline] = CASE
        WHEN LOWER([id]) = 'safety' THEN 'Compliance'
        WHEN LOWER([id]) = 'leadership' THEN 'Other'
        WHEN LOWER([id] + ' ' + [name]) LIKE '%mech%' THEN 'Mechanical'
        WHEN LOWER([id] + ' ' + [name]) LIKE '%elec%' THEN 'Electrical'
        WHEN LOWER([id] + ' ' + [name]) LIKE '%soft%' THEN 'Software'
        ELSE 'Other'
    END
    WHERE [discipline] IS NULL OR LTRIM(RTRIM([discipline])) = '';

    /* The app upgrades this one legacy placeholder after the first successful login. */
    UPDATE [dbo].[users]
    SET [password] = '762f720d5aad8ba562a395ef7a0296887ad20d78a4cb477be916fb9c85ef94eb',
        [updated_at] = GETDATE()
    WHERE [username] = 'admin'
      AND [password] = 'temporary_hash_change_me';

    IF NOT EXISTS (SELECT 1 FROM [dbo].[app_schema_versions] WHERE [version] = 2026072601)
    BEGIN
        INSERT INTO [dbo].[app_schema_versions] ([version], [description])
        VALUES (2026072601, N'Windows pilot baseline: complete schema, constraints, indexes and deployment validation');
    END;

    IF NOT EXISTS (SELECT 1 FROM [dbo].[app_schema_versions] WHERE [version] = 2026072701)
    BEGIN
        INSERT INTO [dbo].[app_schema_versions] ([version], [description])
        VALUES (2026072701, N'Controlled engineer assessment submissions, transactional manager decisions and conflict detection');
    END;

    COMMIT TRANSACTION;
    PRINT 'Aptitude production schema migration 2026072701 completed successfully.';
END TRY
BEGIN CATCH
    IF XACT_STATE() <> 0
        ROLLBACK TRANSACTION;
    THROW;
END CATCH;
GO
