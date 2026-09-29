-- Training Matrix Database Schema
-- Microsoft SQL Server (T-SQL)

SET ANSI_NULLS ON;
SET QUOTED_IDENTIFIER ON;
GO

-- Create database (run this separately if needed)
-- CREATE DATABASE training_matrix;
-- GO
-- USE training_matrix;
-- GO

-- Schema version ledger. Deployment migrations record their version here.
IF OBJECT_ID(N'[dbo].[app_schema_versions]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[app_schema_versions] (
        [version] BIGINT NOT NULL PRIMARY KEY,
        [description] NVARCHAR(250) NOT NULL,
        [applied_at] DATETIME2 NOT NULL CONSTRAINT [DF_app_schema_versions_applied_at] DEFAULT(SYSUTCDATETIME())
    );
END
GO

-- Users table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[users]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[users] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [username] NVARCHAR(100) UNIQUE NOT NULL,
        [password] NVARCHAR(255) NOT NULL,
        [role] NVARCHAR(20) NOT NULL CHECK ([role] IN ('admin', 'engineer')),
        [engineer_id] NVARCHAR(50),
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE()
    );
END
GO

-- Engineers table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[engineers]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[engineers] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [name] NVARCHAR(200) NOT NULL,
        [shift] NVARCHAR(50) NOT NULL,
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE()
    );
END
GO

IF NOT EXISTS (
    SELECT 1 FROM sys.foreign_keys
    WHERE [name] = 'FK_users_engineers'
      AND [parent_object_id] = OBJECT_ID('dbo.users')
)
AND NOT EXISTS (
    SELECT 1
    FROM [dbo].[users] u
    LEFT JOIN [dbo].[engineers] e ON e.[id] = u.[engineer_id]
    WHERE u.[engineer_id] IS NOT NULL AND e.[id] IS NULL
)
BEGIN
    ALTER TABLE [dbo].[users] WITH CHECK
        ADD CONSTRAINT [FK_users_engineers] FOREIGN KEY ([engineer_id])
        REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE;
END
GO

-- Production areas table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[production_areas]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[production_areas] (
        [id] INT IDENTITY(1,1) PRIMARY KEY,
        [name] NVARCHAR(200) NOT NULL,
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE()
    );
END
GO

-- Machines table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[machines]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[machines] (
        [id] INT IDENTITY(1,1) PRIMARY KEY,
        [production_area_id] INT NOT NULL,
        [name] NVARCHAR(200) NOT NULL,
        [importance] INT DEFAULT 1,
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_machines_production_areas] FOREIGN KEY ([production_area_id])
            REFERENCES [dbo].[production_areas]([id]) ON DELETE CASCADE
    );
END
GO

-- Competencies table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[competencies]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[competencies] (
        [id] INT IDENTITY(1,1) PRIMARY KEY,
        [machine_id] INT NOT NULL,
        [name] NVARCHAR(200) NOT NULL,
        [max_score] INT NOT NULL DEFAULT 3,
        [safety_impact] FLOAT NOT NULL CONSTRAINT [DF_competencies_safety_impact] DEFAULT(3.0),
        [production_impact] FLOAT NOT NULL CONSTRAINT [DF_competencies_production_impact] DEFAULT(3.0),
        [frequency] FLOAT NOT NULL CONSTRAINT [DF_competencies_frequency] DEFAULT(3.0),
        [complexity] FLOAT NOT NULL CONSTRAINT [DF_competencies_complexity] DEFAULT(3.0),
        [future_value] FLOAT NOT NULL CONSTRAINT [DF_competencies_future_value] DEFAULT(3.0),
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_competencies_machines] FOREIGN KEY ([machine_id])
            REFERENCES [dbo].[machines]([id]) ON DELETE CASCADE
    );
END
GO

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
GO

-- Assessments table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[assessments]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[assessments] (
        [id] INT IDENTITY(1,1) PRIMARY KEY,
        [engineer_id] NVARCHAR(50) NOT NULL,
        [production_area_id] INT NOT NULL,
        [machine_id] INT NOT NULL,
        [competency_id] INT NOT NULL,
        [score] INT NOT NULL DEFAULT 0,
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_assessments_engineers] FOREIGN KEY ([engineer_id])
            REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE,
        CONSTRAINT [FK_assessments_production_areas] FOREIGN KEY ([production_area_id])
            REFERENCES [dbo].[production_areas]([id]) ON DELETE CASCADE,
        CONSTRAINT [FK_assessments_machines] FOREIGN KEY ([machine_id])
            REFERENCES [dbo].[machines]([id]),
        CONSTRAINT [FK_assessments_competencies] FOREIGN KEY ([competency_id])
            REFERENCES [dbo].[competencies]([id]),
        CONSTRAINT [UQ_assessments] UNIQUE ([engineer_id], [production_area_id], [machine_id], [competency_id])
    );
END
GO

-- Core skills categories table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[core_skill_categories]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[core_skill_categories] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [name] NVARCHAR(200) NOT NULL,
        [discipline] NVARCHAR(20),
        [created_at] DATETIME DEFAULT GETDATE()
    );
END
GO

IF COL_LENGTH('dbo.core_skill_categories', 'discipline') IS NULL
    ALTER TABLE [dbo].[core_skill_categories] ADD [discipline] NVARCHAR(20) NULL;
GO

-- Core skills table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[core_skills]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[core_skills] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [category_id] NVARCHAR(50) NOT NULL,
        [name] NVARCHAR(200) NOT NULL,
        [max_score] INT NOT NULL DEFAULT 3,
        [safety_impact] FLOAT NOT NULL CONSTRAINT [DF_core_skills_safety_impact] DEFAULT(3.0),
        [production_impact] FLOAT NOT NULL CONSTRAINT [DF_core_skills_production_impact] DEFAULT(3.0),
        [frequency] FLOAT NOT NULL CONSTRAINT [DF_core_skills_frequency] DEFAULT(3.0),
        [complexity] FLOAT NOT NULL CONSTRAINT [DF_core_skills_complexity] DEFAULT(3.0),
        [future_value] FLOAT NOT NULL CONSTRAINT [DF_core_skills_future_value] DEFAULT(3.0),
        [created_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_core_skills_categories] FOREIGN KEY ([category_id])
            REFERENCES [dbo].[core_skill_categories]([id]) ON DELETE CASCADE
    );
END
GO

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
GO

-- Core skill assessments table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[core_skill_assessments]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[core_skill_assessments] (
        [id] INT IDENTITY(1,1) PRIMARY KEY,
        [engineer_id] NVARCHAR(50) NOT NULL,
        [category_id] NVARCHAR(50) NOT NULL,
        [skill_id] NVARCHAR(50) NOT NULL,
        [score] INT NOT NULL DEFAULT 0,
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_core_skill_assessments_engineers] FOREIGN KEY ([engineer_id])
            REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE,
        CONSTRAINT [FK_core_skill_assessments_categories] FOREIGN KEY ([category_id])
            REFERENCES [dbo].[core_skill_categories]([id]),
        CONSTRAINT [FK_core_skill_assessments_skills] FOREIGN KEY ([skill_id])
            REFERENCES [dbo].[core_skills]([id]),
        CONSTRAINT [UQ_core_skill_assessments] UNIQUE ([engineer_id], [category_id], [skill_id])
    );
END
GO

-- Certifications table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[certifications]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[certifications] (
        [id] INT IDENTITY(1,1) PRIMARY KEY,
        [engineer_id] NVARCHAR(50) NOT NULL,
        [name] NVARCHAR(200) NOT NULL,
        [date_earned] DATE NOT NULL,
        [expiry_date] DATE,
        [certificate_file_path] NVARCHAR(500) NULL,
        [created_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_certifications_engineers] FOREIGN KEY ([engineer_id])
            REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE
    );
END
GO


IF COL_LENGTH('dbo.certifications', 'certificate_file_path') IS NULL
    ALTER TABLE [dbo].[certifications] ADD [certificate_file_path] NVARCHAR(500) NULL;
GO

-- Snapshots table (for progress history)
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[snapshots]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[snapshots] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [description] NVARCHAR(MAX),
        [timestamp] DATETIME NOT NULL,
        [data] NVARCHAR(MAX) NOT NULL,
        [created_at] DATETIME DEFAULT GETDATE()
    );
END
GO

-- Audit log table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[audit_logs]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[audit_logs] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [timestamp] DATETIME NOT NULL,
        [user_id] NVARCHAR(50),
        [action] NVARCHAR(100) NOT NULL,
        [details] NVARCHAR(MAX),
        [created_at] DATETIME DEFAULT GETDATE()
    );
END
GO

-- User and manager notifications
IF OBJECT_ID(N'[dbo].[notifications]', N'U') IS NULL
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
        [recipient_user_id] NVARCHAR(50) NULL,
        CONSTRAINT [FK_notifications_engineer] FOREIGN KEY ([engineer_id]) REFERENCES [dbo].[engineers]([id]),
        CONSTRAINT [FK_notifications_created_by] FOREIGN KEY ([created_by_user_id]) REFERENCES [dbo].[users]([id]),
        CONSTRAINT [FK_notifications_recipient] FOREIGN KEY ([recipient_user_id]) REFERENCES [dbo].[users]([id])
    );
END
GO

-- Append-only engineer submissions awaiting manager approval
IF OBJECT_ID(N'[dbo].[assessment_submissions]', N'U') IS NULL
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
        CONSTRAINT [CK_assessment_submissions_scores] CHECK (
            [proposed_score] BETWEEN 0 AND 3 AND ([base_score] IS NULL OR [base_score] BETWEEN 0 AND 3)
        ),
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
END
GO

-- Create indexes for better performance
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_engineers_shift' AND object_id = OBJECT_ID('dbo.engineers'))
    CREATE NONCLUSTERED INDEX [IX_engineers_shift] ON [dbo].[engineers]([shift]) WHERE [shift] IS NOT NULL;
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_machines_area' AND object_id = OBJECT_ID('dbo.machines'))
    CREATE NONCLUSTERED INDEX [IX_machines_area] ON [dbo].[machines]([production_area_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_competencies_machine' AND object_id = OBJECT_ID('dbo.competencies'))
    CREATE NONCLUSTERED INDEX [IX_competencies_machine] ON [dbo].[competencies]([machine_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_assessments_engineer' AND object_id = OBJECT_ID('dbo.assessments'))
    CREATE NONCLUSTERED INDEX [IX_assessments_engineer] ON [dbo].[assessments]([engineer_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_core_skills_category' AND object_id = OBJECT_ID('dbo.core_skills'))
    CREATE NONCLUSTERED INDEX [IX_core_skills_category] ON [dbo].[core_skills]([category_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_core_skill_assessments_engineer' AND object_id = OBJECT_ID('dbo.core_skill_assessments'))
    CREATE NONCLUSTERED INDEX [IX_core_skill_assessments_engineer] ON [dbo].[core_skill_assessments]([engineer_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_certifications_engineer' AND object_id = OBJECT_ID('dbo.certifications'))
    CREATE NONCLUSTERED INDEX [IX_certifications_engineer] ON [dbo].[certifications]([engineer_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_audit_logs_timestamp' AND object_id = OBJECT_ID('dbo.audit_logs'))
    CREATE NONCLUSTERED INDEX [IX_audit_logs_timestamp] ON [dbo].[audit_logs]([timestamp] DESC);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_notifications_engineer_created' AND object_id = OBJECT_ID('dbo.notifications'))
    CREATE NONCLUSTERED INDEX [IX_notifications_engineer_created] ON [dbo].[notifications]([engineer_id], [created_at] DESC);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'UX_assessment_submissions_pending' AND object_id = OBJECT_ID('dbo.assessment_submissions'))
    CREATE UNIQUE NONCLUSTERED INDEX [UX_assessment_submissions_pending]
    ON [dbo].[assessment_submissions]([review_key]) WHERE [decision] = 'pending';
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_assessment_submissions_queue' AND object_id = OBJECT_ID('dbo.assessment_submissions'))
    CREATE NONCLUSTERED INDEX [IX_assessment_submissions_queue]
    ON [dbo].[assessment_submissions]([decision], [submitted_at] DESC) INCLUDE ([engineer_id], [assessment_type], [proposed_score], [base_score]);
GO

-- Score guardrails to protect analytics integrity
IF NOT EXISTS (
    SELECT 1 FROM sys.check_constraints
    WHERE name = 'CK_assessments_score_range'
      AND parent_object_id = OBJECT_ID('dbo.assessments')
)
BEGIN
    ALTER TABLE [dbo].[assessments]
    ADD CONSTRAINT [CK_assessments_score_range] CHECK ([score] >= 0 AND [score] <= 3);
END
GO

IF NOT EXISTS (
    SELECT 1 FROM sys.check_constraints
    WHERE name = 'CK_competencies_weight_ranges'
      AND parent_object_id = OBJECT_ID('dbo.competencies')
)
BEGIN
    ALTER TABLE [dbo].[competencies] WITH CHECK
    ADD CONSTRAINT [CK_competencies_weight_ranges] CHECK (
        [safety_impact] BETWEEN 0 AND 5 AND
        [production_impact] BETWEEN 0 AND 5 AND
        [frequency] BETWEEN 0 AND 5 AND
        [complexity] BETWEEN 0 AND 5 AND
        [future_value] BETWEEN 0 AND 5
    );
END
GO

IF NOT EXISTS (
    SELECT 1 FROM sys.check_constraints
    WHERE name = 'CK_core_skills_weight_ranges'
      AND parent_object_id = OBJECT_ID('dbo.core_skills')
)
BEGIN
    ALTER TABLE [dbo].[core_skills] WITH CHECK
    ADD CONSTRAINT [CK_core_skills_weight_ranges] CHECK (
        [safety_impact] BETWEEN 0 AND 5 AND
        [production_impact] BETWEEN 0 AND 5 AND
        [frequency] BETWEEN 0 AND 5 AND
        [complexity] BETWEEN 0 AND 5 AND
        [future_value] BETWEEN 0 AND 5
    );
END
GO

IF NOT EXISTS (
    SELECT 1 FROM sys.check_constraints
    WHERE name = 'CK_core_skill_assessments_score_range'
      AND parent_object_id = OBJECT_ID('dbo.core_skill_assessments')
)
BEGIN
    ALTER TABLE [dbo].[core_skill_assessments]
    ADD CONSTRAINT [CK_core_skill_assessments_score_range] CHECK ([score] >= 0 AND [score] <= 3);
END
GO

-- Insert default admin user
-- IMPORTANT: After running this schema, you MUST update the admin password hash
-- The password hash must be generated using the app's Crypto::hashPassword function
--
-- To set the admin password, run this SQL AFTER computing the hash:
-- UPDATE users SET password = '<hash_from_app>' WHERE username = 'admin';
--
-- Or create the admin user manually through the application
IF NOT EXISTS (SELECT * FROM [dbo].[users] WHERE [id] = 'admin')
BEGIN
    INSERT INTO [dbo].[users] ([id], [username], [password], [role], [engineer_id])
    VALUES ('admin', 'admin', 'temporary_hash_change_me', 'admin', NULL);
END
GO

-- Insert default core skill categories
IF NOT EXISTS (SELECT * FROM [dbo].[core_skill_categories] WHERE [id] = 'mechanical')
BEGIN
    INSERT INTO [dbo].[core_skill_categories] ([id], [name], [discipline]) VALUES
    ('mechanical', 'General', 'Mechanical'),
    ('electrical', 'General', 'Electrical'),
    ('software', 'General', 'Software'),
    ('safety', 'Safety Skills', 'Compliance'),
    ('leadership', 'Leadership & Soft Skills', 'Other');
END
GO


UPDATE [dbo].[core_skill_categories]
SET [discipline] = CASE
    WHEN LOWER([id]) = 'mechanical' THEN 'Mechanical'
    WHEN LOWER([id]) = 'electrical' THEN 'Electrical'
    WHEN LOWER([id]) = 'software' THEN 'Software'
    WHEN LOWER([id]) = 'safety' THEN 'Compliance'
    WHEN LOWER([id]) = 'leadership' THEN 'Other'
    ELSE [discipline]
END
WHERE [discipline] IS NULL;

UPDATE [dbo].[core_skill_categories] SET [discipline] = 'Compliance' WHERE LOWER([id]) = 'safety';
UPDATE [dbo].[core_skill_categories] SET [discipline] = 'Other' WHERE LOWER([id]) = 'leadership';
GO

-- Insert default core skills
IF NOT EXISTS (SELECT * FROM [dbo].[core_skills] WHERE [id] = 'troubleshooting')
BEGIN
    INSERT INTO [dbo].[core_skills] ([id], [category_id], [name], [max_score]) VALUES
        ('troubleshooting', 'mechanical', 'Troubleshooting', 3),
        ('preventive-maintenance', 'mechanical', 'Preventive Maintenance', 3),
        ('repair', 'mechanical', 'Equipment Repair', 3),
        ('installation', 'mechanical', 'Installation', 3),
        ('wiring', 'electrical', 'Wiring & Circuits', 3),
        ('circuit-diagnosis', 'electrical', 'Circuit Diagnosis', 3),
        ('plc', 'electrical', 'PLC Programming', 3),
        ('motor-control', 'electrical', 'Motor Control Systems', 3),
        ('programming', 'software', 'Basic Programming', 3),
        ('hmi', 'software', 'HMI Operation', 3),
        ('data-analysis', 'software', 'Data Analysis', 3),
        ('scada', 'software', 'SCADA Systems', 3),
        ('lockout-tagout', 'safety', 'Lockout/Tagout', 3),
        ('confined-spaces', 'safety', 'Confined Spaces', 3),
        ('hazard-identification', 'safety', 'Hazard Identification', 3),
        ('emergency-response', 'safety', 'Emergency Response', 3),
        ('communication', 'leadership', 'Communication', 3),
        ('teamwork', 'leadership', 'Teamwork & Collaboration', 3),
        ('problem-solving', 'leadership', 'Problem Solving', 3),
        ('training-others', 'leadership', 'Training Others', 3);
END
GO

-- Targets table (for setting goals during 1-1 meetings)
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[targets]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[targets] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [engineer_id] NVARCHAR(50) NOT NULL,
        [title] NVARCHAR(200) NOT NULL,
        [description] NVARCHAR(MAX),
        [target_type] NVARCHAR(50) NOT NULL CHECK ([target_type] IN ('competency', 'certification', 'core_skill', 'general')),
        [target_area_id] NVARCHAR(50),  -- ID of competency, skill, etc. (optional)
        [target_value] INT,  -- Target score or count
        [current_value] INT,  -- Current achievement level
        [due_date] DATE,
        [status] NVARCHAR(20) NOT NULL DEFAULT 'active' CHECK ([status] IN ('active', 'completed', 'cancelled')),
        [set_by_user_id] NVARCHAR(50),
        [notes] NVARCHAR(MAX),
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE(),
        [completed_at] DATETIME,
        CONSTRAINT [FK_targets_engineers] FOREIGN KEY ([engineer_id])
            REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE,
        CONSTRAINT [FK_targets_set_by] FOREIGN KEY ([set_by_user_id])
            REFERENCES [dbo].[users]([id])
    );
END
GO

-- Create indexes for targets
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_targets_engineer' AND object_id = OBJECT_ID('dbo.targets'))
    CREATE NONCLUSTERED INDEX [IX_targets_engineer] ON [dbo].[targets]([engineer_id]);
IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_targets_status' AND object_id = OBJECT_ID('dbo.targets'))
    CREATE NONCLUSTERED INDEX [IX_targets_status] ON [dbo].[targets]([status]);
GO

-- Manager-created engineer development plans
IF OBJECT_ID(N'[dbo].[development_plan_items]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[development_plan_items] (
        [id] INT IDENTITY(1,1) NOT NULL PRIMARY KEY,
        [engineer_id] NVARCHAR(50) NOT NULL,
        [production_area_id] INT NOT NULL,
        [machine_id] INT NOT NULL,
        [competency_id] INT NOT NULL,
        [current_score] INT NOT NULL DEFAULT(0),
        [target_score] INT NOT NULL DEFAULT(2),
        [due_date] DATE NULL,
        [manager_notes] NVARCHAR(1000) NULL,
        [guidance] NVARCHAR(1500) NULL,
        [status] NVARCHAR(20) NOT NULL DEFAULT('active'),
        [created_by_user_id] NVARCHAR(50) NULL,
        [created_by_name] NVARCHAR(100) NULL,
        [created_at] DATETIME2 NOT NULL DEFAULT(GETDATE()),
        [updated_at] DATETIME2 NOT NULL DEFAULT(GETDATE()),
        CONSTRAINT [CK_development_plan_scores] CHECK ([current_score] BETWEEN 0 AND 3 AND [target_score] BETWEEN 0 AND 3),
        CONSTRAINT [CK_development_plan_status] CHECK ([status] IN ('active','completed','cancelled')),
        CONSTRAINT [FK_development_plan_engineer] FOREIGN KEY ([engineer_id]) REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE,
        CONSTRAINT [FK_development_plan_area] FOREIGN KEY ([production_area_id]) REFERENCES [dbo].[production_areas]([id]),
        CONSTRAINT [FK_development_plan_machine] FOREIGN KEY ([machine_id]) REFERENCES [dbo].[machines]([id]),
        CONSTRAINT [FK_development_plan_competency] FOREIGN KEY ([competency_id]) REFERENCES [dbo].[competencies]([id])
    );
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.indexes WHERE name = 'IX_development_plan_engineer_status' AND object_id = OBJECT_ID('dbo.development_plan_items'))
    CREATE NONCLUSTERED INDEX [IX_development_plan_engineer_status]
    ON [dbo].[development_plan_items]([engineer_id], [status], [due_date]);
GO

PRINT 'Database schema created successfully!';
PRINT 'Default admin user: admin / admin123';
PRINT 'Please change the default password after first login.';
GO
