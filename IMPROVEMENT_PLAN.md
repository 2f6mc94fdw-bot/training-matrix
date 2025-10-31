# Training Matrix - Improvement Plan
*Prioritized roadmap for feature enhancements*

---

## 🎯 PHASE 1: CRITICAL FIXES & FOUNDATION (Week 1)
**Priority: IMMEDIATE - These fix current issues and set up architecture for future features**

### 1.1 Fix Training Plan Display Bug
- **Issue**: Training plan not showing results when engineer has scores filled in
- **Fix**: Debug modal display logic and ensure proper feedback
- **Time**: 1 hour
- **Impact**: HIGH - Core feature not working

### 1.2 Auto-Create User Accounts with Engineers
- **Feature**: When engineer is added, automatically create user account
  - Default password: `password`
  - Auto-link engineer profile to user account
- **Time**: 2 hours
- **Impact**: HIGH - Reduces admin workload significantly

### 1.3 Password Reset Functionality
- **Feature**: Admin tab ability to reset any user's password
- **UI**: Button next to each user in User Accounts section
- **Time**: 2 hours
- **Impact**: HIGH - Essential admin function

---

## 🚀 PHASE 2: UI/UX OVERHAUL (Week 2)
**Priority: HIGH - Makes app professional and usable**

### 2.1 Dashboard (New Landing Page)
- **Features**:
  - High-level team overview cards
  - Percentage breakdown of 0/1/2/3 scores across all engineers
  - Top performers / Engineers needing attention
  - Recent activity feed
  - Quick stats: Total engineers, Total competencies, Average skill level
- **Visualizations**:
  - Pie chart: Score distribution (0/1/2/3)
  - Bar chart: Engineers by overall competency level
  - Trend line: Team progress over time (using snapshots)
- **Time**: 8 hours
- **Impact**: VERY HIGH - First impression, executive summary

### 2.2 Professional UI Redesign
- **Changes**:
  - Modern color palette (primary, secondary, accent colors)
  - Consistent spacing and typography
  - Card-based layouts with shadows and borders
  - Gradient headers
  - Better button styling (hover states, focus states)
  - Icon improvements using Lucide React
  - Loading skeletons instead of plain text
  - Animations (slide-ins, fades)
- **Time**: 12 hours
- **Impact**: VERY HIGH - Professional appearance

### 2.3 Light/Dark Mode Toggle
- **Implementation**:
  - Context API for theme state
  - CSS variables for colors
  - Toggle switch in header
  - Persist preference to localStorage
  - Smooth transition between modes
- **Time**: 6 hours
- **Impact**: MEDIUM - Nice-to-have, modern feature

### 2.4 Admin Tab Sub-Navigation
- **Structure**:
  - Horizontal tabs at top: Engineers | Production Areas | Users | Settings
  - Each section on separate panel
  - Better organization than vertical scrolling
- **Time**: 4 hours
- **Impact**: HIGH - Much better UX for admins

---

## 📊 PHASE 3: REPORTS & ANALYTICS (Week 3)
**Priority: HIGH - Core business value**

### 3.1 Reports Tab Restructure
- **Sub-tabs**:
  1. **Skill Gap Analysis**
     - Heatmap of all engineers vs all competencies
     - Color-coded cells (red=0, orange=1, yellow=2, green=3)
     - Filter by production area
     - Identify weakest competencies across team

  2. **Engineer Progress Overview**
     - Individual engineer timeline
     - Score changes over snapshots
     - Improvement/regression indicators (↑↓ arrows)
     - Comparison to team average

  3. **Competency Heatmap**
     - Matrix view: Production Areas × Machines × Competencies
     - Team average score per competency
     - Highlight critical gaps

  4. **Shift Comparison** (NEW)
     - Compare performance between shifts
     - Requires shift data field on engineers
     - Raw (unweighted) view
     - Weighted view (using machine importance)

  5. **Individual Analysis**
     - Deep dive on single engineer
     - Radar chart of competencies
     - Progress over time graphs
     - Training recommendations

- **Time**: 16 hours (all sub-tabs)
- **Impact**: VERY HIGH - Core analytical capability

### 3.2 Machine Weighting System
- **Features**:
  - Editable importance value per machine (1-10 scale)
  - Already exists in data structure, make it customizable in UI
  - Admin panel to adjust weights
  - Toggle between weighted/unweighted views in reports
  - Calculated weighted averages for engineers
- **Time**: 6 hours
- **Impact**: HIGH - Business requirement

---

## 👤 PHASE 4: ENGINEER PORTAL (Week 4)
**Priority: HIGH - End-user experience**

### 4.1 Engineer Personal Dashboard
- **When engineer logs in, they see**:
  - **Current Scores Panel**
    - All their competencies with current scores
    - Visual progress bars
    - Color-coded badges

  - **Progress Tracking**
    - Historical scores per submission (from snapshots)
    - Visual indicators: ↑ Improved, ↓ Regressed, → Unchanged
    - "Last updated" timestamps per competency

  - **Overall Knowledge Graph**
    - Line/area chart showing total competency over time
    - Drill-down by production area
    - Click area to see detailed graph for that area

  - **Competency Breakdown Charts**
    - Bar chart per production area
    - Radar chart of current competencies
    - Comparison to team average (anonymized)

- **Time**: 12 hours
- **Impact**: VERY HIGH - Empowers engineers

### 4.2 Engineer Training Plan with Comments
- **Features**:
  - Engineer can see their own training plan
  - Manager can add comments/notes per competency
  - Engineer can add progress notes
  - Comment thread per training item
  - Mark training items as "In Progress" or "Completed"
  - Due dates for training goals
- **Data Structure**: Add `trainingComments` to localStorage
- **Time**: 10 hours
- **Impact**: HIGH - Collaboration & accountability

---

## 🔧 PHASE 5: ENGINEERING CORE SKILLS (Week 5)
**Priority: MEDIUM - New feature area**

### 5.1 Core Skills Module
- **Separate Section**: Not tied to production areas
- **Skill Categories**:
  - Mechanical Skills (e.g., Troubleshooting, Preventive Maintenance, Repair)
  - Electrical Skills (e.g., Wiring, Circuit Diagnosis, PLC)
  - Software Skills (e.g., Programming, HMI, Data Analysis)
  - Safety Skills
  - Leadership/Soft Skills

- **Assessment Structure**:
  - Similar to production competencies (0-3 scale)
  - Separate assessment grid
  - Can be weighted differently

- **Reports**:
  - Core skills heatmap
  - Engineer competency profiles
  - Shift comparison for core skills
  - Gap analysis

- **Time**: 14 hours
- **Impact**: MEDIUM-HIGH - Separate business need

---

## 📈 PHASE 6: ADVANCED ANALYTICS (Week 6)
**Priority: MEDIUM - Enhanced insights**

### 6.1 Shift Comparison Analytics
- **Requirements**:
  - Add "shift" field to engineer profile (Day/Night/Rotating)
  - Compare shifts across:
    - Production area competencies
    - Core engineering skills
    - Overall team performance
  - Both raw and weighted views

- **Visualizations**:
  - Side-by-side bar charts
  - Shift average score cards
  - Competency gap identification

- **Time**: 8 hours
- **Impact**: MEDIUM - Management insights

### 6.2 Raw vs Weighted Comparison
- **Features**:
  - Tab toggle in reports: "Unweighted | Weighted"
  - Unweighted: Simple average of all scores
  - Weighted: Machine importance factored in
  - Show delta between views
  - Identify where weighting changes rankings

- **Time**: 6 hours
- **Impact**: MEDIUM - Analytical depth

---

## 📋 IMPLEMENTATION SUMMARY

### Total Estimated Time: **~107 hours (13-14 working days)**

### Recommended Priority Order:
1. **PHASE 1** (Week 1): Foundation fixes - enables everything else
2. **PHASE 2** (Week 2): UI/UX - makes app presentable and organized
3. **PHASE 3** (Week 3): Reports - core business value
4. **PHASE 4** (Week 4): Engineer portal - end-user empowerment
5. **PHASE 5** (Week 5): Core skills - parallel competency system
6. **PHASE 6** (Week 6): Advanced analytics - enhanced insights

### Quick Wins (Can be done in parallel):
- Auto-create accounts (1.2)
- Password reset (1.3)
- Admin sub-tabs (2.4)
- Machine weighting UI (3.2)

### High-Impact Features:
1. Dashboard (2.1) - Executive visibility
2. Skill Gap Analysis (3.1) - Identifies training needs
3. Engineer Personal Dashboard (4.1) - User engagement
4. Professional UI (2.2) - Credibility

---

## 🔄 DEPENDENCIES

```
Phase 1 (Foundation)
    ↓
Phase 2 (UI/UX) ← Must complete before other phases for consistency
    ↓
    ├→ Phase 3 (Reports) ← Uses new UI components
    ├→ Phase 4 (Engineer Portal) ← Uses new UI + auth from Phase 1
    └→ Phase 5 (Core Skills) ← Parallel to production areas
         ↓
      Phase 6 (Advanced Analytics) ← Combines all data
```

---

## 💾 DATA STRUCTURE CHANGES NEEDED

### New Fields:
- **Engineer**: `shift` (string: "Day" | "Night" | "Rotating")
- **TrainingComments**: `{ engineerId, competencyKey, comments: [{author, text, date}], status, dueDate }`
- **CoreSkills**: `{ categories: [], engineers: [], assessments: {} }` (parallel structure to production areas)
- **Machine**: `importance` (already exists, make editable)
- **Theme**: `preference` (in localStorage, "light" | "dark")

---

## 🎨 UI COMPONENT ARCHITECTURE

### New Components to Create:
- `Dashboard.jsx` - Landing page with stats
- `SkillGapHeatmap.jsx` - Matrix visualization
- `EngineerProgressChart.jsx` - Timeline graphs
- `CompetencyRadarChart.jsx` - Spider/radar chart
- `ShiftComparison.jsx` - Shift analytics
- `ThemeToggle.jsx` - Light/dark mode switch
- `TrainingPlanComments.jsx` - Collaborative training plan
- `CoreSkillsModule.jsx` - Separate skills section
- `ScoreHistoryGraph.jsx` - Individual progress tracking
- `TabbedPanel.jsx` - Reusable tab component

---

## ✅ TESTING CHECKLIST (Per Phase)

### Phase 1:
- [ ] Training plan displays for engineer with all 2+ scores
- [ ] Training plan shows items for engineer with 0-1 scores
- [ ] New engineer auto-creates user account
- [ ] Password reset works from admin panel

### Phase 2:
- [ ] Dashboard loads without errors
- [ ] Light/dark mode persists after refresh
- [ ] Admin sub-tabs navigate correctly
- [ ] UI looks professional on mobile and desktop

### Phase 3:
- [ ] All report tabs render without errors
- [ ] Heatmaps display correct data
- [ ] Weighted calculations are accurate
- [ ] Machine importance edits save correctly

### Phase 4:
- [ ] Engineer sees only their own data
- [ ] Graphs show historical progress accurately
- [ ] Comments save and display correctly
- [ ] Training plan collaboration works

### Phase 5:
- [ ] Core skills assessments independent from production areas
- [ ] Core skills reports function correctly
- [ ] Data doesn't conflict with production assessments

### Phase 6:
- [ ] Shift comparisons show meaningful data
- [ ] Raw vs weighted toggle works
- [ ] Individual analysis pulls all relevant data

---

## 🚨 RISKS & MITIGATION

| Risk | Impact | Mitigation |
|------|--------|------------|
| Data structure breaking changes | HIGH | Version migrations, backup/restore before updates |
| Performance with large datasets | MEDIUM | Pagination, virtualization for long lists |
| Mobile responsiveness | MEDIUM | Test on mobile throughout, use responsive design |
| Browser compatibility | LOW | Stick to modern ES6+, test on Chrome/Firefox/Safari |
| User adoption of new features | MEDIUM | Training documentation, tooltips, help text |

---

## 📝 NEXT STEPS

1. **Review & Approve Plan**: Confirm priorities align with business needs
2. **Start Phase 1**: Fix critical issues (training plan bug, auto-accounts, password reset)
3. **Design Mockups**: For dashboard and UI redesign (Phase 2)
4. **Gather Requirements**: Shift data, core skills taxonomy
5. **Setup Development Branch**: For systematic implementation
6. **Incremental Deployment**: Deploy after each phase for user feedback

---

**Ready to proceed with Phase 1?**
