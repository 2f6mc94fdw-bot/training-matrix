/*
  Aptitude production SQL login setup.
  Run after schema.sql and PRODUCTION_SCHEMA_MIGRATION.sql as a SQL Server DBA.

  Replace both placeholder passwords before running this script. The application
  account is intentionally limited to data operations on Aptitude tables.
*/

USE [master];
GO

DECLARE @DatabaseName sysname = N'training_matrix';
DECLARE @AppLogin sysname = N'aptitude_app';
DECLARE @AppPassword nvarchar(128) = N'REPLACE_WITH_STRONG_PASSWORD_24+';
DECLARE @ReadOnlyLogin sysname = N'aptitude_readonly';
DECLARE @ReadOnlyPassword nvarchar(128) = N'REPLACE_WITH_DIFFERENT_STRONG_PASSWORD_24+';

IF @AppPassword LIKE N'REPLACE[_]%' OR @ReadOnlyPassword LIKE N'REPLACE[_]%'
    THROW 51000, 'Replace both placeholder passwords before running this script.', 1;

IF @AppPassword = @ReadOnlyPassword
    THROW 51001, 'The read/write and read-only SQL accounts must use different passwords.', 1;

IF DB_ID(@DatabaseName) IS NULL
    THROW 51002, 'The training_matrix database does not exist.', 1;

IF NOT EXISTS (SELECT 1 FROM sys.sql_logins WHERE [name] = @AppLogin)
BEGIN
    DECLARE @CreateAppLogin nvarchar(max) =
        N'CREATE LOGIN ' + QUOTENAME(@AppLogin) +
        N' WITH PASSWORD = ' + QUOTENAME(@AppPassword, '''') +
        N', CHECK_POLICY = ON, CHECK_EXPIRATION = OFF;';
    EXEC sys.sp_executesql @CreateAppLogin;
END;

IF NOT EXISTS (SELECT 1 FROM sys.sql_logins WHERE [name] = @ReadOnlyLogin)
BEGIN
    DECLARE @CreateReadOnlyLogin nvarchar(max) =
        N'CREATE LOGIN ' + QUOTENAME(@ReadOnlyLogin) +
        N' WITH PASSWORD = ' + QUOTENAME(@ReadOnlyPassword, '''') +
        N', CHECK_POLICY = ON, CHECK_EXPIRATION = OFF;';
    EXEC sys.sp_executesql @CreateReadOnlyLogin;
END;

DECLARE @DatabaseSetup nvarchar(max) = N'
USE ' + QUOTENAME(@DatabaseName) + N';

IF NOT EXISTS (SELECT 1 FROM sys.database_principals WHERE [name] = N''' + REPLACE(@AppLogin, '''', '''''') + N''')
    CREATE USER ' + QUOTENAME(@AppLogin) + N' FOR LOGIN ' + QUOTENAME(@AppLogin) + N';

IF NOT EXISTS (SELECT 1 FROM sys.database_principals WHERE [name] = N''' + REPLACE(@ReadOnlyLogin, '''', '''''') + N''')
    CREATE USER ' + QUOTENAME(@ReadOnlyLogin) + N' FOR LOGIN ' + QUOTENAME(@ReadOnlyLogin) + N';

IF NOT EXISTS (SELECT 1 FROM sys.database_principals WHERE [name] = N''aptitude_app_role'' AND [type] = ''R'')
    CREATE ROLE [aptitude_app_role];

IF NOT EXISTS (SELECT 1 FROM sys.database_principals WHERE [name] = N''aptitude_read_role'' AND [type] = ''R'')
    CREATE ROLE [aptitude_read_role];

GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.users TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.engineers TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.production_areas TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.machines TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.competencies TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.assessments TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.core_skill_categories TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.core_skills TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.core_skill_assessments TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.certifications TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.snapshots TO [aptitude_app_role];
GRANT SELECT, INSERT ON OBJECT::dbo.audit_logs TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.targets TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.notifications TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE, DELETE ON OBJECT::dbo.development_plan_items TO [aptitude_app_role];
GRANT SELECT, INSERT, UPDATE ON OBJECT::dbo.assessment_submissions TO [aptitude_app_role];
GRANT SELECT ON OBJECT::dbo.app_schema_versions TO [aptitude_app_role];

GRANT SELECT ON OBJECT::dbo.users TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.engineers TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.production_areas TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.machines TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.competencies TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.assessments TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.core_skill_categories TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.core_skills TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.core_skill_assessments TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.certifications TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.snapshots TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.audit_logs TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.targets TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.notifications TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.development_plan_items TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.assessment_submissions TO [aptitude_read_role];
GRANT SELECT ON OBJECT::dbo.app_schema_versions TO [aptitude_read_role];

IF NOT EXISTS (
    SELECT 1 FROM sys.database_role_members drm
    JOIN sys.database_principals r ON r.principal_id = drm.role_principal_id
    JOIN sys.database_principals m ON m.principal_id = drm.member_principal_id
    WHERE r.[name] = N''aptitude_app_role'' AND m.[name] = N''' + REPLACE(@AppLogin, '''', '''''') + N''')
    ALTER ROLE [aptitude_app_role] ADD MEMBER ' + QUOTENAME(@AppLogin) + N';

IF NOT EXISTS (
    SELECT 1 FROM sys.database_role_members drm
    JOIN sys.database_principals r ON r.principal_id = drm.role_principal_id
    JOIN sys.database_principals m ON m.principal_id = drm.member_principal_id
    WHERE r.[name] = N''aptitude_read_role'' AND m.[name] = N''' + REPLACE(@ReadOnlyLogin, '''', '''''') + N''')
    ALTER ROLE [aptitude_read_role] ADD MEMBER ' + QUOTENAME(@ReadOnlyLogin) + N';
';

EXEC sys.sp_executesql @DatabaseSetup;
PRINT 'Aptitude SQL users and least-privilege roles configured successfully.';
GO
