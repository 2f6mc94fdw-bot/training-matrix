# Phase 1 - Changed Files List

Copy these files from `/home/user/training-matrix` to your local project:

## New Files (Create these)
1. **server/validation.cjs** - Input validation module
2. **.env** - Environment variables (copy from .env.example and update password)

## Modified Files (Replace these)

### Configuration
3. **.env.example** - Updated with database settings
4. **database/config.cjs** - Environment variable support
5. **database/connection.cjs** - Added transaction support

### Server
6. **server/package.json** - Updated dependencies (pg → mssql)
7. **server/index.cjs** - Added validation, async bcrypt, /api/data/import
8. **server/setup-database.js** - Async bcrypt for default admin

### Frontend
9. **src/App.jsx** - Fixed all 18 missing await calls
10. **src/components/Login.jsx** - Fixed async/await in login
11. **src/hooks/useData.js** - Fixed return value in addEngineer

## Files Summary
- **11 files total**
- **2 new files**
- **9 modified files**

## Quick Copy Commands

If you have access to this directory, you can copy files like this:

```bash
# Create destination variable
DEST="/Users/danielabley/Desktop/training-matrix-main-2"

# Copy new files
cp /home/user/training-matrix/server/validation.cjs "$DEST/server/"
cp /home/user/training-matrix/.env.example "$DEST/"

# Copy modified files
cp /home/user/training-matrix/database/config.cjs "$DEST/database/"
cp /home/user/training-matrix/database/connection.cjs "$DEST/database/"
cp /home/user/training-matrix/server/package.json "$DEST/server/"
cp /home/user/training-matrix/server/index.cjs "$DEST/server/"
cp /home/user/training-matrix/server/setup-database.js "$DEST/server/"
cp /home/user/training-matrix/src/App.jsx "$DEST/src/"
cp /home/user/training-matrix/src/components/Login.jsx "$DEST/src/components/"
cp /home/user/training-matrix/src/hooks/useData.js "$DEST/src/hooks/"

# Create .env from example
cp "$DEST/.env.example" "$DEST/.env"
# Edit .env and set your DB_PASSWORD
```
