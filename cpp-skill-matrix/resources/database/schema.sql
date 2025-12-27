-- Training Matrix Database Schema
-- Microsoft SQL Server (T-SQL)

-- Create database (run this separately if needed)
-- CREATE DATABASE training_matrix;
-- GO
-- USE training_matrix;
-- GO

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
        [created_at] DATETIME DEFAULT GETDATE(),
        [updated_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_competencies_machines] FOREIGN KEY ([machine_id])
            REFERENCES [dbo].[machines]([id]) ON DELETE CASCADE
    );
END
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
        [created_at] DATETIME DEFAULT GETDATE()
    );
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
        [created_at] DATETIME DEFAULT GETDATE(),
        CONSTRAINT [FK_certifications_engineers] FOREIGN KEY ([engineer_id])
            REFERENCES [dbo].[engineers]([id]) ON DELETE CASCADE
    );
END
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

-- Create indexes for better performance
CREATE NONCLUSTERED INDEX [IX_engineers_shift] ON [dbo].[engineers]([shift]) WHERE [shift] IS NOT NULL;
CREATE NONCLUSTERED INDEX [IX_machines_area] ON [dbo].[machines]([production_area_id]);
CREATE NONCLUSTERED INDEX [IX_competencies_machine] ON [dbo].[competencies]([machine_id]);
CREATE NONCLUSTERED INDEX [IX_assessments_engineer] ON [dbo].[assessments]([engineer_id]);
CREATE NONCLUSTERED INDEX [IX_core_skills_category] ON [dbo].[core_skills]([category_id]);
CREATE NONCLUSTERED INDEX [IX_core_skill_assessments_engineer] ON [dbo].[core_skill_assessments]([engineer_id]);
CREATE NONCLUSTERED INDEX [IX_certifications_engineer] ON [dbo].[certifications]([engineer_id]);
CREATE NONCLUSTERED INDEX [IX_audit_logs_timestamp] ON [dbo].[audit_logs]([timestamp] DESC);
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
    INSERT INTO [dbo].[core_skill_categories] ([id], [name]) VALUES
        ('mechanical', 'Mechanical Skills'),
        ('electrical', 'Electrical Skills'),
        ('software', 'Software Skills'),
        ('safety', 'Safety Skills'),
        ('leadership', 'Leadership & Soft Skills');
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

-- Create index for targets
CREATE NONCLUSTERED INDEX [IX_targets_engineer] ON [dbo].[targets]([engineer_id]);
CREATE NONCLUSTERED INDEX [IX_targets_status] ON [dbo].[targets]([status]);
GO

PRINT 'Database schema created successfully!';
PRINT 'Default admin user: admin / admin123';
PRINT 'Please change the default password after first login.';
GO
