# Aptitude Windows Deployment

Aptitude is a native 64-bit desktop application. Each Windows workstation runs `Aptitude.exe` and connects directly to one central Microsoft SQL Server database. No web server is required.

## Supported Platform

- Windows 10 or Windows 11, x64
- Microsoft SQL Server 2016 or later
- Microsoft ODBC Driver 18 for SQL Server, x64, on every client
- Visual Studio 2022 Build Tools with **Desktop development with C++**
- Qt 6.8.3 MSVC 2022 64-bit with Qt Charts
- CMake 3.20 or later

Use the same MSVC toolchain for Qt and the application. Do not mix MinGW and MSVC packages.

## Build A Release Package

Run from a clean, committed source checkout on Windows:

```powershell
cd <repository>\cpp-skill-matrix\deployment\windows
Set-ExecutionPolicy -Scope Process Bypass
.\Build-Aptitude.ps1 -QtRoot "C:\Qt\6.8.3\msvc2022_64"
```

The script:

- builds the active source under `cpp-skill-matrix`, not a copied source snapshot;
- refuses an uncommitted source tree for a release build;
- deploys Qt runtime libraries and verifies the Windows, Charts, and ODBC plugins;
- packages the SQL scripts and pilot documents;
- writes `BUILD_INFO.txt` with the source commit and dirty-state flag;
- writes `SHA256SUMS.txt` for package integrity; and
- creates `output\Aptitude-Windows-x64.zip`.

`-AllowDirtySource` is for development builds only. Do not give a package with `SourceDirty=True` to work IT.

The GitHub Actions workflow `.github/workflows/aptitude-windows-release.yml` runs the same Windows build and a clean SQL Server deployment test. Use its artifact only after both jobs pass.

## Prepare SQL Server

Take a full backup before changing an existing Aptitude database. Run SQL scripts from SQL Server Management Studio using a DBA account.

### New Database

1. Create an empty database named `training_matrix`.
2. Select `training_matrix` in SSMS and run `database\schema.sql`.
3. Keep `training_matrix` selected and run `database\PRODUCTION_SCHEMA_MIGRATION.sql`.
4. Keep `training_matrix` selected and run `database\VALIDATE_DATABASE.sql`.
5. Confirm validation reports schema version `2026072701` and prints `Aptitude database validation passed.`
6. Edit both placeholder passwords in `database\PRODUCTION_SQL_SETUP.sql`.
7. Run `PRODUCTION_SQL_SETUP.sql` as a server administrator.
8. Store the generated `aptitude_app` password in the approved password vault.

### Existing Database

1. Stop Aptitude clients and take a full SQL backup.
2. Run `PRODUCTION_SCHEMA_MIGRATION.sql` against the existing Aptitude database.
3. Run `VALIDATE_DATABASE.sql` against that database.
4. If migration reports orphan user links, repair the affected user-to-engineer assignments before rerunning it. Do not bypass this check.
5. Run `PRODUCTION_SQL_SETUP.sql` if the least-privilege logins have not already been created.

The migration is transactional, versioned, idempotent, and refuses to run against a system database. Application accounts receive object-level data permissions rather than database-owner access.

## Install A Client

1. Verify `SHA256SUMS.txt` before deployment. For example:

```powershell
Get-FileHash .\Aptitude.exe -Algorithm SHA256
```

2. Install Microsoft ODBC Driver 18 for SQL Server, x64.
3. Extract the complete package to a controlled folder such as `C:\Program Files\Aptitude`. Do not distribute only `Aptitude.exe`.
4. Permit outbound TCP traffic to the SQL Server host and configured port, normally 1433.
5. Launch `Aptitude.exe`.
6. Enter the SQL host, database `training_matrix`, port, and `aptitude_app` credentials.
7. Keep **Encrypt** enabled. Use **Trust server certificate** only during a controlled pilot while IT is arranging a trusted SQL certificate.
8. Select **Test Connection**, connect, and sign in as `admin` with temporary password `admin123`.
9. Change the Aptitude admin password immediately.

The client validates required tables, columns, and schema version during connection. An old or incomplete database is rejected with a migration message rather than opening a partially working application.

## Assessment Approval Control

- Engineer production and core-skill changes are saved as pending submissions, not official scores.
- A manager must approve a submission before it updates the official assessment used by dashboards and reports.
- Rejections require a reason and leave the official score unchanged.
- Approval, audit history, and the engineer notification are committed in one SQL transaction.
- If another user changes a submission or official score after a page was loaded, Aptitude blocks the stale update and requires a refresh.
- Manager-direct core-skill changes are saved as one batch, so a partial set cannot be committed.

Use the **Approval Queue** during the pilot and reconcile a sample of approved scores against the Audit Log. Do not treat pending submissions as verified competency evidence.

## Local Client Data

Settings and logs are stored under the signed-in Windows user's local AppData area, normally `%LOCALAPPDATA%\Training Matrix\Aptitude`.

Certificate uploads currently copy the file to the local workstation and store that local path in SQL Server. A certificate uploaded on one PC may not open on another PC. Treat this as a pilot limitation: use a secured shared document location or implement central attachment storage before certifications become an authoritative multi-workstation archive.

## Security And Operations

- Never expose SQL Server directly to the public internet.
- Restrict SQL access to approved work devices and office/VPN networks.
- Use `aptitude_app`, never `sa` or a personal administrator account.
- Deploy a trusted SQL Server certificate before wider production use.
- Back up `training_matrix` nightly and prove restoration into a non-production database.
- Code-sign the Windows executable or installer before broad deployment.
- Start with 3-5 pilot users and use `WINDOWS_PILOT_CHECKLIST.md` to record evidence.
- Retain the previous client package and database backup for rollback.

## Updating Aptitude

1. Build from a clean release commit and wait for both GitHub Actions jobs to pass.
2. Back up SQL Server.
3. Apply and validate any newer database migration before releasing its matching client.
4. Close Aptitude on each client and replace the complete application folder.
5. Run focused smoke tests before reopening access to the pilot group.

Client binaries can be rolled back by restoring the previous application folder. Restore a database backup only through the approved DBA recovery process because that discards data entered after the backup.
