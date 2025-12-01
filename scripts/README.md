# Engineer Import Scripts

This directory contains scripts to import all engineers and create user accounts with the password "password".

## 📋 Engineer List

Total engineers to import: **19**

- 5 on Day Shift
- 5 on B Shift
- 4 on C Shift
- 5 on D Shift

## 🚀 Option 1: Import via API (Recommended)

**Requirements**: Server must be running

```bash
# 1. Start the server (in one terminal)
npm run dev

# 2. Run the import script (in another terminal)
node scripts/import-engineers-api.cjs
```

**Features**:
- ✅ Automatically checks for existing engineers
- ✅ Skips duplicates
- ✅ Creates user accounts with password "password"
- ✅ Usernames: firstname.lastname (e.g., john.irish)
- ✅ Shows progress and summary

## 📊 Option 2: Direct SQL Import

**Requirements**: Access to SQL Server database

```bash
# Run the SQL file in your database client
# File: scripts/engineers.sql
```

Or copy and paste the SQL statements directly into SQL Server Management Studio or Azure Data Studio.

**Note**: All users will have:
- Username: `firstname.lastname` (lowercase, dots instead of spaces)
- Password: `password`
- Role: `engineer`

## 👥 User Accounts Created

| Engineer | Username | Password |
|----------|----------|----------|
| John Irish | john.irish | password |
| Mark Castleton | mark.castleton | password |
| Dave Spauls | dave.spauls | password |
| Ben Barnham | ben.barnham | password |
| Mark Quin | mark.quin | password |
| Jon Anderson | jon.anderson | password |
| Callum Clark | callum.clark | password |
| Dustin Lawrie | dustin.lawrie | password |
| Connor Clark | connor.clark | password |
| Matt Challis | matt.challis | password |
| Luke Hancy | luke.hancy | password |
| Simon Brooks | simon.brooks | password |
| Elliot McMullen | elliot.mcmullen | password |
| Richard Smith | richard.smith | password |
| Yauheni Shumanski | yauheni.shumanski | password |
| Daniel Wilson | daniel.wilson | password |
| Oliver Mills | oliver.mills | password |
| James Sarosi | james.sarosi | password |
| Scott Macfarlane | scott.macfarlane | password |

## ⚠️ Security Note

**IMPORTANT**: All users are created with the default password "password".

Engineers should change their passwords after first login through the application settings.

## 🔍 Verification

After import, verify the data:

```sql
-- Check total engineers
SELECT COUNT(*) as total_engineers FROM engineers;

-- Check engineers by shift
SELECT shift, COUNT(*) as count
FROM engineers
GROUP BY shift
ORDER BY shift;

-- Check user accounts
SELECT COUNT(*) as total_engineer_users
FROM users
WHERE role = 'engineer';
```
