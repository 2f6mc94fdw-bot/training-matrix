# SQL Server Integration Guide

The Training Matrix application has been successfully integrated with Microsoft SQL Server, replacing the previous localStorage-based data persistence with a production-ready database system.

## Architecture Overview

```
┌─────────────────┐
│  React Frontend │  (Vite + React 18)
│  Port: 5173     │
└────────┬────────┘
         │ HTTP Requests
         ▼
┌─────────────────┐
│  Express API    │  (Node.js + Express)
│  Port: 3001     │
└────────┬────────┘
         │ SQL Queries (mssql)
         ▼
┌─────────────────┐
│  SQL Server     │  (Microsoft SQL Server 2016+)
│  Port: 1433     │
└─────────────────┘
```

## Components

### 1. Frontend (React)
- **Location:** `src/`
- **Key Files:**
  - `src/utils/api.js` - API client for backend communication
  - `src/hooks/useData.js` - React hook for data operations
  - `src/hooks/useAuth.js` - Authentication hook

### 2. Backend API Server
- **Location:** `server.cjs`
- **Port:** 3001
- **Framework:** Express.js with CORS enabled
- **Features:**
  - RESTful API endpoints
  - Connection pooling
  - Error handling
  - Request logging

### 3. Database Layer
- **Location:** `database/`
- **Key Files:**
  - `database/connection.js` - SQL Server connection pool
  - `database/queries.js` - Database query functions
  - `database/schema.sql` - Database schema (T-SQL)
  - `database/config.js` - Connection configuration (gitignored)

## Setup Instructions

### 1. Install Dependencies

```bash
npm install
```

New dependencies added:
- `express` - Web server framework
- `cors` - Cross-Origin Resource Sharing
- `concurrently` - Run multiple processes simultaneously

### 2. Configure Database Connection

Copy the example configuration:

```bash
cp database/config.example.js database/config.js
```

Edit `database/config.js` with your SQL Server credentials:

```javascript
const config = {
  server: 'your-server-name',
  database: 'training_matrix',
  user: 'your-username',
  password: 'your-password',
  options: {
    encrypt: true,
    trustServerCertificate: true,
    enableArithAbort: true
  }
};
```

### 3. Create Database and Schema

Run the schema script to create all tables:

```bash
# Using SQL Server Management Studio (SSMS)
# 1. Open SSMS and connect to your server
# 2. Open database/schema.sql
# 3. Select training_matrix database
# 4. Execute (F5)

# OR using sqlcmd
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/schema.sql
```

### 4. Install Data

Install production areas and core skills:

```bash
node database/install-all.js
```

This installs:
- 3 Production Areas (Line 7 Filling, Packing, Tank Room)
- 93 competencies across machines
- 3 Core Skill Categories (Mechanical, Software, Electrical)
- 101 core skills

**Total: 194 competencies**

### 5. Test Database Connection

```bash
node database/test-connection.js
```

You should see:
```
✅ Connection successful!
✅ Found 13 tables
```

### 6. Start the Application

```bash
npm run dev
```

This command starts:
- Backend API server on http://localhost:3001
- Frontend dev server on http://localhost:5173

To run separately:
```bash
# Backend only
npm run dev:backend

# Frontend only
npm run dev:frontend
```

### 7. Login

Navigate to http://localhost:5173

Default credentials:
- **Username:** admin
- **Password:** admin123

⚠️ **IMPORTANT:** Change the default password immediately after first login!

## API Endpoints

### Authentication
- `POST /api/auth/login` - User login

### Data
- `GET /api/data` - Get all data (initial load)

### Users
- `GET /api/users` - Get all users
- `POST /api/users` - Create user
- `DELETE /api/users/:id` - Delete user
- `PUT /api/users/:id/password` - Reset password

### Engineers
- `GET /api/engineers` - Get all engineers
- `POST /api/engineers` - Create engineer
- `PUT /api/engineers/:id` - Update engineer
- `DELETE /api/engineers/:id` - Delete engineer

### Production Areas
- `GET /api/production-areas` - Get all areas with machines and competencies
- `POST /api/production-areas` - Create area
- `PUT /api/production-areas/:id` - Update area
- `DELETE /api/production-areas/:id` - Delete area

### Machines
- `POST /api/production-areas/:areaId/machines` - Create machine
- `PUT /api/production-areas/:areaId/machines/:machineId` - Update machine
- `DELETE /api/production-areas/:areaId/machines/:machineId` - Delete machine

### Competencies
- `POST /api/production-areas/:areaId/machines/:machineId/competencies` - Create competency
- `PUT /api/production-areas/:areaId/machines/:machineId/competencies/:compId` - Update competency
- `DELETE /api/production-areas/:areaId/machines/:machineId/competencies/:compId` - Delete competency

### Assessments
- `GET /api/assessments` - Get all assessments
- `POST /api/assessments` - Save assessment
- `POST /api/assessments/bulk` - Bulk update assessments

### Core Skills
- `GET /api/core-skills` - Get all core skills
- `GET /api/core-skills/assessments` - Get core skill assessments
- `POST /api/core-skills/assessments` - Save core skill assessment
- `POST /api/core-skills/categories` - Create category
- `POST /api/core-skills/categories/:categoryId/skills` - Create skill
- `DELETE /api/core-skills/:skillId` - Delete skill

### Certifications
- `GET /api/certifications` - Get all certifications
- `POST /api/certifications` - Create certification
- `DELETE /api/certifications/:id` - Delete certification

### Snapshots
- `GET /api/snapshots` - Get all snapshots
- `POST /api/snapshots` - Create snapshot

### Audit Logs
- `GET /api/audit-logs` - Get audit logs
- `POST /api/audit-logs` - Create audit log

### Health Check
- `GET /api/health` - Check API and database status

## Database Schema

### Key Tables

**users**
- Primary authentication table
- Passwords hashed with bcrypt
- Links to engineers via `engineer_id`

**engineers**
- Employee records
- Shift assignment
- Tracks training progress

**production_areas**
- Manufacturing areas (e.g., Line 7 Filling)
- Contains machines

**machines**
- Equipment within production areas
- Has importance rating (1-3)
- Contains competencies

**competencies**
- Specific skills for each machine
- Max score (typically 3)
- Used for assessments

**assessments**
- Engineer skill assessments
- Tracks scores over time
- Linked to engineer, area, machine, competency

**core_skill_categories**
- Skill categories (Mechanical, Software, Electrical, etc.)
- Independent of production areas

**core_skills**
- Transferable skills across areas
- Belongs to category

**core_skill_assessments**
- Core skill evaluations
- Similar to regular assessments

**certifications**
- Engineer certifications
- Expiry tracking

**snapshots**
- Point-in-time data snapshots
- Historical tracking

**audit_logs**
- Action logging
- User activity tracking

All tables have:
- `created_at` timestamp (default: GETDATE())
- `updated_at` timestamp (auto-updated)

## Migration from localStorage

The application was migrated from localStorage to SQL Server:

### What Changed
1. **Data Storage:** localStorage → SQL Server
2. **Data Access:** Synchronous → Asynchronous (async/await)
3. **Architecture:** Client-only → Client-Server
4. **Authentication:** In-memory → Database-backed

### What Stayed the Same
1. **Frontend Components:** No changes to UI
2. **User Experience:** Same interface and workflows
3. **Data Structure:** Similar schema structure
4. **Function Signatures:** useData hook maintains same API

### Breaking Changes
- Login and data operations are now async
- Requires backend server to be running
- Requires SQL Server connection

## Development Workflow

### Starting Development

```bash
npm run dev
```

This starts both backend and frontend in development mode with hot reloading.

### Building for Production

```bash
npm run build
```

For production deployment:
1. Build the frontend: `npm run build`
2. Serve static files from `dist/` folder
3. Run backend: `node server.cjs`
4. Configure reverse proxy (nginx/Apache) to route:
   - `/api/*` → Backend (port 3001)
   - `/*` → Frontend static files

### Environment Variables

Create `.env` file for production:

```env
VITE_API_URL=https://your-domain.com/api
PORT=3001
```

## Troubleshooting

### Backend Won't Start

**Error:** `Cannot find module 'express'`
```bash
npm install
```

**Error:** `Cannot connect to SQL Server`
- Check `database/config.js` credentials
- Verify SQL Server is running
- Check firewall allows port 1433
- Run: `node database/test-connection.js`

### Frontend Can't Connect to Backend

**Error:** `Failed to fetch` or CORS errors
- Ensure backend is running on port 3001
- Check `src/utils/api.js` has correct API_BASE_URL
- Verify CORS is enabled in `server.cjs`

### Login Fails

**Error:** `Invalid credentials`
- Ensure database has default admin user
- Check password was migrated correctly
- Verify bcrypt is comparing correctly

### Data Not Loading

**Error:** `Failed to load data`
- Check browser console for errors
- Verify backend logs for SQL errors
- Check database connection
- Ensure tables exist: `SELECT * FROM INFORMATION_SCHEMA.TABLES`

### Port Already in Use

**Error:** `EADDRINUSE: address already in use`
```bash
# Kill process on port 3001
# Windows
netstat -ano | findstr :3001
taskkill /PID <PID> /F

# Linux/Mac
lsof -ti:3001 | xargs kill -9
```

## Performance Considerations

### Connection Pooling
- Backend uses connection pooling (max 10 connections)
- Connections reused across requests
- Idle connections closed after 30 seconds

### Data Loading
- Initial load fetches all data via `/api/data`
- Subsequent operations update locally + API call
- Optimistic updates for better UX

### Caching
- No caching currently implemented
- Consider Redis for production
- Use ETags for conditional requests

## Security

### Implemented
- ✅ Password hashing with bcrypt (10 rounds)
- ✅ SQL injection prevention (parameterized queries)
- ✅ CORS enabled for frontend origin
- ✅ Session management (sessionStorage)
- ✅ Audit logging for all actions

### Recommended for Production
- ⚠️ Add JWT authentication
- ⚠️ Implement rate limiting
- ⚠️ Use HTTPS only
- ⚠️ Add request validation
- ⚠️ Implement role-based access control (RBAC)
- ⚠️ Add API key authentication
- ⚠️ Enable SQL Server encryption at rest
- ⚠️ Regular security audits

## Backup and Restore

### Manual Backup
```sql
BACKUP DATABASE training_matrix
TO DISK = 'C:\\Backups\\training_matrix.bak'
WITH COMPRESSION;
```

### Automated Backups
Set up SQL Server Agent jobs for:
- Daily full backups
- Hourly transaction log backups
- Weekly maintenance

### Restore
```sql
RESTORE DATABASE training_matrix
FROM DISK = 'C:\\Backups\\training_matrix.bak'
WITH REPLACE;
```

## Monitoring

### Health Check Endpoint
```bash
curl http://localhost:3001/api/health
```

Response:
```json
{
  "status": "healthy",
  "database": "connected",
  "timestamp": "2024-01-15T10:30:00.000Z"
}
```

### Logging
- Backend logs all requests to console
- SQL errors logged with query details
- Failed login attempts logged

### Metrics to Monitor
- API response times
- Database connection pool usage
- Failed requests
- Active user sessions
- Assessment save rate

## Support

For issues:
1. Check logs in terminal running backend
2. Check browser console for frontend errors
3. Verify database connection: `node database/test-connection.js`
4. Review this documentation
5. Check `INSTALLATION.md` for setup steps

## File Structure

```
training-matrix/
├── src/
│   ├── components/         # React components
│   ├── hooks/
│   │   ├── useAuth.js     # Authentication hook (updated for API)
│   │   └── useData.js     # Data management hook (updated for API)
│   └── utils/
│       ├── api.js         # API client (NEW)
│       └── storage.js     # Legacy localStorage utils
├── database/
│   ├── connection.js      # SQL Server connection pool
│   ├── queries.js         # Database query functions
│   ├── schema.sql         # Database schema
│   ├── config.js          # Connection config (gitignored)
│   ├── config.example.js  # Config template
│   ├── production-areas/  # Production area SQL scripts
│   ├── core-skills/       # Core skill SQL scripts
│   └── install-all.js     # Master data installer
├── server.cjs             # Express API server (NEW)
├── package.json           # Updated with new dependencies
├── INSTALLATION.md        # Complete setup guide
└── SQL_SERVER_INTEGRATION.md  # This file
```

## Next Steps

1. ✅ Install dependencies: `npm install`
2. ✅ Configure database connection
3. ✅ Create database schema
4. ✅ Install data: `node database/install-all.js`
5. ✅ Test connection: `node database/test-connection.js`
6. ✅ Start application: `npm run dev`
7. ✅ Login and verify functionality
8. ⚠️ Change default admin password
9. ⚠️ Add engineers and start assessments
10. ⚠️ Configure backups
11. ⚠️ Set up monitoring

---

**Version:** 2.0.0
**Last Updated:** 2024-01-15
**Database:** Microsoft SQL Server 2016+
