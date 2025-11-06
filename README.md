# Training Matrix System

A comprehensive training and competency management system for tracking engineer skills across production areas.

## 🆕 Version 2.0 - SQL Database Support

The application now supports **SQL databases** (PostgreSQL, MySQL, SQL Server) for centralized data storage, multi-user access, and remote connectivity to work servers.

**Quick links:**
- 🚀 [Quick Start Guide](./QUICKSTART_DATABASE.md) - Get running in 5 minutes
- 📖 [Full Database Setup](./DATABASE_SETUP.md) - Complete documentation
- 🔧 [Connecting to Work Server](#connecting-to-work-server) - Remote database setup

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
- Node.js 18+ installed
- npm or yarn package manager
- PostgreSQL 12+ (or MySQL/SQL Server)

### Quick Setup (5 minutes)

1. **Clone and install:**
```bash
git clone <your-repo-url>
cd training-matrix
npm install
cd server && npm install && cd ..
```

2. **Create database:**
```bash
# Create PostgreSQL database
createdb training_matrix

# Import schema
psql -d training_matrix -f server/db/schema.sql
```

3. **Configure connection:**
```bash
# Copy environment template
cp .env.example .env

# Edit .env and update with your database credentials
# Example: DB_CONNECTION_STRING=postgresql://postgres:password@localhost:5432/training_matrix
```

4. **Start application:**
```bash
# Terminal 1 - Start backend
npm run server

# Terminal 2 - Start frontend
npm run dev
```

5. **Open browser:**
Navigate to `http://localhost:5173`

### Default Login
- **Username:** admin
- **Password:** admin123

⚠️ **Change the default password after first login!**

## Connecting to Work Server

To connect to your company's database server:

1. **Get credentials from IT:**
   - Server address (e.g., `db.company.com` or `192.168.1.100`)
   - Port (usually `5432` for PostgreSQL)
   - Database name
   - Username and password

2. **Update .env file:**
```env
DB_CONNECTION_STRING=postgresql://username:password@db.company.com:5432/training_matrix
DB_SSL=true
NODE_ENV=production
```

3. **Setup database on server:**
```bash
psql "postgresql://username:password@server:5432/database" -f server/db/schema.sql
```

4. **Start application:**
```bash
npm run server  # Backend
npm run dev     # Frontend
```

**See [DATABASE_SETUP.md](./DATABASE_SETUP.md) for detailed instructions and troubleshooting.**

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

### Backend (New in v2.0)
- **Node.js** - Server runtime
- **Express** - Web framework
- **PostgreSQL** - Primary database (MySQL/SQL Server supported)
- **bcryptjs** - Password hashing
- **pg** - PostgreSQL client

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
