# Training Matrix System

A comprehensive training and competency management system for tracking engineer skills across production areas.

## 🆕 Version 2.0 - SQL Server Database Support

The application now stores data in **Microsoft SQL Server** for centralized data storage, multi-user access, and remote connectivity to work servers.

**Quick links:**
- 📖 [SQL Server Setup Guide](./SQL_SERVER_SETUP.md) - Complete setup instructions
- 🔧 [Connecting to Work Server](./SQL_SERVER_SETUP.md#connecting-to-work-server) - Remote server connection guide
- 🧪 [Test Your Connection](./SQL_SERVER_SETUP.md#quick-start) - Verify database connection

## Features

### Phase 1: Data Management
- ✅ Import/Export CSV and Excel files
- ✅ Complete backup and restore functionality
- ✅ Audit trail of all changes

### Phase 2: Reports & Analytics
- ✅ Overall team competency dashboard
- ✅ Skills gap analysis with priorities
- ✅ Visual heatmaps
- ✅ Progress tracking over time
- ✅ Charts and graphs (Recharts)

### Phase 3: User Experience
- ✅ Search and filter capabilities
- ✅ Bulk operations for mass updates
- ✅ Comments and notes on assessments
- ✅ Mobile-responsive design

### Phase 4: Advanced Features
- ✅ Auto-generate training plans
- ✅ Certification tracking with expiry alerts
- ✅ Complete audit trail
- ✅ Progress history with snapshots

## Setup Instructions

### Prerequisites
- **Node.js 18+** installed
- **npm** package manager
- **Microsoft SQL Server** (2016 or later)
  - SQL Server Express (free) for local/small teams
  - SQL Server Standard/Enterprise for production
  - Azure SQL Database also supported

### Quick Setup

1. **Clone and install:**
```bash
git clone <your-repo-url>
cd training-matrix
npm install
```

2. **Configure connection:**
```bash
# Copy the example config file
cp database/config.example.js database/config.js

# Edit database/config.js with your SQL Server details
```

Example `database/config.js`:
```javascript
const config = {
  server: 'localhost',            // Your SQL Server name or IP
  database: 'training_matrix',
  user: 'sa',                     // Your SQL Server username
  password: 'YourPassword123',    // Your SQL Server password

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};
```

3. **Create database:**

Open SQL Server Management Studio (SSMS) or use `sqlcmd`:
```sql
CREATE DATABASE training_matrix;
GO
```

4. **Run schema script:**

In SSMS, open `database/schema.sql` and execute it (F5).

Or via command line:
```bash
sqlcmd -S localhost -U sa -P YourPassword -d training_matrix -i database/schema.sql
```

5. **Test connection:**
```bash
node database/test-connection.js
```

6. **Start application:**
```bash
npm run dev
```

Open `http://localhost:5173`

### Default Login
- **Username:** `admin`
- **Password:** `admin123`

⚠️ **Change the default password immediately after first login!**

## Connecting to Work Server

To connect to your company's SQL Server:

1. **Get credentials from IT:**
   - Server name (e.g., `SERVERNAME`, `192.168.1.100`, or `server.company.com`)
   - Instance name (if applicable, e.g., `SQLEXPRESS`)
   - Database name: `training_matrix`
   - Username and password
   - Port (usually `1433`)

2. **Update database/config.js:**
```javascript
const config = {
  server: 'YOUR_SERVER',          // e.g., '192.168.1.100' or 'SERVERNAME'
  database: 'training_matrix',
  user: 'your_username',
  password: 'your_password',

  options: {
    encrypt: true,
    trustServerCertificate: true
  }
};
```

3. **Setup database on work server:**

Connect to your work server via SSMS and run `database/schema.sql`

4. **Test connection:**
```bash
node database/test-connection.js
```

5. **Start application:**
```bash
npm run dev
```

**See [SQL_SERVER_SETUP.md](./SQL_SERVER_SETUP.md) for detailed instructions, connection string examples, and troubleshooting.**

## Building for Production

```bash
npm run build
```

The built files will be in the `dist` folder.

## Project Structure

```
src/
├── components/          # React components
│   ├── admin/          # Admin management components
│   ├── reports/        # Reporting components
│   └── advanced/       # Advanced feature components
├── hooks/              # Custom React hooks
└── utils/              # Utility functions
```

## Technology Stack

### Frontend
- **React 18** - UI framework
- **Vite** - Build tool
- **Tailwind CSS** - Styling
- **Recharts** - Data visualization
- **XLSX** - Excel import/export
- **Lucide React** - Icons

### Backend & Database (New in v2.0)
- **Microsoft SQL Server** - Database (2016+ / Express / Azure SQL)
- **mssql** - SQL Server client for Node.js
- **bcryptjs** - Password hashing and authentication
- Direct SQL queries - No REST API overhead

## Usage

### Admin Functions
1. Manage production areas and machines
2. Add/edit engineers and shifts
3. Configure competencies and scoring
4. Create user accounts

### Data Management
1. Import data from Excel/CSV
2. Export complete reports
3. Backup and restore data

### Reports
1. View team dashboard
2. Analyze skills gaps
3. Generate heatmaps
4. Track progress over time

### Advanced Features
1. Generate training plans automatically
2. Track certifications and expiries
3. View audit trail of changes
4. Compare historical snapshots

## Support

For issues or questions, please open an issue on GitHub.
