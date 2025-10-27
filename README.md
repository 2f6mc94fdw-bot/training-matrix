# Training Matrix System

A comprehensive training and competency management system for tracking engineer skills across production areas.

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

### Installation

1. **Clone the repository:**
```bash
git clone <your-repo-url>
cd training-matrix
```

2. **Install dependencies:**
```bash
npm install
```

3. **Start development server:**
```bash
npm run dev
```

4. **Open browser:**
Navigate to `http://localhost:3000`

### Default Login
- **Username:** admin
- **Password:** admin123

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

- **React 18** - UI framework
- **Vite** - Build tool
- **Tailwind CSS** - Styling
- **Recharts** - Data visualization
- **XLSX** - Excel import/export
- **Lucide React** - Icons

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
