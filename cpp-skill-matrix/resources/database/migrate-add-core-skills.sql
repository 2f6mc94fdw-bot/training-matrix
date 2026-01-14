-- Migration: Add Core Skills Tables
-- Run this script against your training_matrix database to add core skills functionality

-- Core skills categories table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[core_skill_categories]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[core_skill_categories] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [name] NVARCHAR(200) NOT NULL,
        [created_at] DATETIME DEFAULT GETDATE()
    );
    PRINT 'Created table: core_skill_categories';
END
ELSE
BEGIN
    PRINT 'Table already exists: core_skill_categories';
END
GO

-- Core skills table
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[core_skills]') AND type in (N'U'))
BEGIN
    CREATE TABLE [dbo].[core_skills] (
        [id] NVARCHAR(50) PRIMARY KEY,
        [category_id] NVARCHAR(50) NOT NULL,
        [name] NVARCHAR(200) NOT NULL,
        [max_score] INT NOT NULL DEFAULT 3,
        [created_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_core_skills_categories] FOREIGN KEY ([category_id])
            REFERENCES [dbo].[core_skill_categories]([id]) ON DELETE CASCADE
    );
    PRINT 'Created table: core_skills';
END
ELSE
BEGIN
    PRINT 'Table already exists: core_skills';
END
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
    PRINT 'Created table: core_skill_assessments';
END
ELSE
BEGIN
    PRINT 'Table already exists: core_skill_assessments';
END
GO

-- Create indexes
IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_core_skills_category' AND object_id = OBJECT_ID('core_skills'))
BEGIN
    CREATE NONCLUSTERED INDEX [IX_core_skills_category] ON [dbo].[core_skills]([category_id]);
    PRINT 'Created index: IX_core_skills_category';
END
GO

IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = 'IX_core_skill_assessments_engineer' AND object_id = OBJECT_ID('core_skill_assessments'))
BEGIN
    CREATE NONCLUSTERED INDEX [IX_core_skill_assessments_engineer] ON [dbo].[core_skill_assessments]([engineer_id]);
    PRINT 'Created index: IX_core_skill_assessments_engineer';
END
GO

-- Insert default core skill categories
IF NOT EXISTS (SELECT * FROM [dbo].[core_skill_categories] WHERE [id] = 'mechanical')
BEGIN
    INSERT INTO [dbo].[core_skill_categories] ([id], [name]) VALUES
        ('mechanical', 'Mechanical Skills'),
        ('electrical', 'Electrical Skills'),
        ('software', 'Software Skills'),
        ('safety', 'Safety Skills'),
        ('leadership', 'Leadership & Soft Skills');
    PRINT 'Inserted default core skill categories';
END
ELSE
BEGIN
    PRINT 'Core skill categories already exist';
END
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
    PRINT 'Inserted default core skills';
END
ELSE
BEGIN
    PRINT 'Core skills already exist';
END
GO

PRINT '';
PRINT 'Migration complete! Core skills tables are now ready.';
GO
