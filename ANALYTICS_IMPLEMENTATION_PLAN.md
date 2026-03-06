# Analytics Implementation Plan - Phase 1

## Files to Modify

### 1. AnalyticsWidget.h
**Add new members:**
```cpp
// New tab buttons
QPushButton* criticalSkillsButton_;
QPushButton* machineReadinessButton_;

// Critical Skills tab components
QChartView* riskMatrixChartView_;
QListWidget* trainingPriorityList_;

// Machine Readiness tab components
QListWidget* machineReadinessList_;
QListWidget* vulnerabilityList_;

// New setup methods
void setupCriticalSkillsTab(QWidget* widget);
void setupMachineReadinessTab(QWidget* widget);

// New update methods
void updateCriticalSkillsData();
void updateMachineReadinessData();

// New data structures
struct CompetencyRiskPoint {
    QString name;
    int competencyId;
    double proficiency;      // 0-100%
    double importance;       // 0-5.0 from weight
    QString riskLevel;       // "critical", "maintain", "defer", "excess"
};

struct MachineReadiness {
    QString machineName;
    int machineId;
    int proficientCount;     // engineers with score >= 2
    int expertCount;         // engineers with score == 3
    int totalEngineers;
    double coveragePercent;
    bool isCritical;         // < 50% coverage
    int importance;
};

struct TrainingRecommendation {
    QString competencyName;
    int engineersNeed;
    double impactScore;      // 0-10
    QString reason;
    QString priority;        // "urgent", "high", "medium"
};

// Helper methods
QList<CompetencyRiskPoint> calculateCompetencyRisks();
QList<MachineReadiness> calculateMachineReadiness();
QList<TrainingRecommendation> calculateTrainingROI();
```

### 2. AnalyticsWidget.cpp
**Add implementations for:**

#### A. Critical Skills Dashboard
```cpp
void AnalyticsWidget::setupCriticalSkillsTab(QWidget* widget)
{
    // Scatter plot for risk matrix
    // Top: Training priority list with impact scores
    // Bottom: ROI calculator recommendations
}

void AnalyticsWidget::updateCriticalSkillsData()
{
    // Calculate risk points for all competencies
    // Plot on scatter chart (X=proficiency, Y=importance)
    // Color by quadrant (red/green/gray/blue)
    // Generate training recommendations
}
```

#### B. Machine Readiness Analysis
```cpp
void AnalyticsWidget::setupMachineReadinessTab(QWidget* widget)
{
    // List of machines with coverage scores
    // Vulnerability alerts (single point of failure)
    // Shift gap analysis
}

void AnalyticsWidget::updateMachineReadinessData()
{
    // For each machine:
    //   - Count proficient engineers (score >= 2)
    //   - Calculate coverage %
    //   - Flag if < 50%
    //   - Check for single points of failure
}
```

#### C. Enhanced Automated Insights
```cpp
QList<AnalyticsWidget::Insight> AnalyticsWidget::generateAutomatedInsights()
{
    // Existing insights +

    // 1. Critical Gap Alerts
    for (machineReadiness : machineReadinessList) {
        if (coveragePercent < 30%) {
            add "CRITICAL: Machine X has only Y% coverage"
        }
    }

    // 2. Single Point of Failure
    if (proficientCount == 1) {
        add "WARNING: Only 1 engineer qualified on Machine X"
    }

    // 3. Training Success
    if (improvement > 25%) {
        add "SUCCESS: Competency improved 25% this month"
    }

    // 4. Stagnation Alert
    if (no progress in 60 days) {
        add "ALERT: X engineers show no progress in 60 days"
    }

    // 5. Cross-Training Opportunity
    if (shift A strong in X, shift B strong in Y) {
        add "OPPORTUNITY: Cross-train between shifts"
    }

    // 6. Promotion Ready
    if (engineer proficiency > 90%) {
        add "READY: Engineer X achieved 90% proficiency"
    }

    // 7. Unbalanced Skills
    if (area strong in X but weak in Y) {
        add "UNBALANCED: Focus training on [specific area]"
    }

    // 8. Resource Allocation
    calculate training hours needed per area
    add "RESOURCE: Area X needs 3x more training than Y"

    // 9. Learning Plateau
    identify engineers with flatlined progress
    add specific intervention recommendations

    // 10. High Impact Training
    from ROI calculator, suggest top 3 trainings
}
```

## Implementation Steps

### Step 1: Update Header (AnalyticsWidget.h)
- Add new member variables
- Add new data structures
- Add new method declarations

### Step 2: Add Tab Setup
- Modify setupUI() to add new tab buttons
- Create setupCriticalSkillsTab()
- Create setupMachineReadinessTab()
- Wire up button connections

### Step 3: Implement Calculations
- calculateCompetencyRisks()
- calculateMachineReadiness()
- calculateTrainingROI()

### Step 4: Implement Update Methods
- updateCriticalSkillsData()
- updateMachineReadinessData()
- Enhance generateAutomatedInsights()

### Step 5: Add Visualizations
- Scatter plot for risk matrix
- Styled list items with color coding
- Progress bars for coverage percentages

## Expected Results

### Critical Skills Dashboard
```
┌─────────────────────────────────────────────┐
│ COMPETENCY RISK MATRIX                      │
│                                             │
│ 5.0 │ DEFER        │ ⚠️ CRITICAL          │
│ Imp │             │ ● Line 7 Fill         │
│ o   │             │ ● Tank Room Ops       │
│ r   ├─────────────┼─────────────┤         │
│ t   │ MAINTAIN    │ EXCESS       │         │
│ a   │ ●●●         │ ●●           │         │
│ n   │             │              │         │
│ c   │             │              │         │
│ e   │             │              │         │
│ 0.0 └─────────────┴─────────────┘         │
│      0%           50%          100%         │
│              Proficiency                    │
│                                             │
│ TOP TRAINING PRIORITIES                     │
│ ⚠️ URGENT: Line 7 Filling                  │
│    Coverage: 23% | Impact: 8.5/10          │
│    12 engineers need training               │
│                                             │
│ ⚠️ HIGH: Viaflo Racking Operations         │
│    Coverage: 45% | Impact: 6.8/10          │
│    8 engineers need training                │
└─────────────────────────────────────────────┘
```

### Machine Readiness Analysis
```
┌─────────────────────────────────────────────┐
│ MACHINE COVERAGE STATUS                     │
│                                             │
│ ⚠️ Line 7 Filler                           │
│    Coverage: 23% (3/13 proficient)         │
│    Experts: 1 | Risk: CRITICAL             │
│                                             │
│ ⚠️ Viaflo Racking                          │
│    Coverage: 45% (5/11 proficient)         │
│    Experts: 2 | Risk: MEDIUM               │
│                                             │
│ ✅ Line 1 Packer                           │
│    Coverage: 85% (11/13 proficient)        │
│    Experts: 5 | Risk: LOW                  │
│                                             │
│ VULNERABILITY ALERTS                        │
│                                             │
│ ⚠️ Single Point of Failure                 │
│    Tank Room: Only John Smith expert       │
│    Line 6: Only 2 engineers qualified      │
│                                             │
│ 📊 Shift Analysis                          │
│    Shift A: 8 vulnerable machines          │
│    Shift B: 3 vulnerable machines          │
│    Shift C: 5 vulnerable machines          │
└─────────────────────────────────────────────┘
```

### Enhanced Insights (10+ alerts)
```
┌─────────────────────────────────────────────┐
│ AUTOMATED INSIGHTS                          │
│                                             │
│ ⚠️ CRITICAL GAP                            │
│    Line 7 Filling has 23% coverage         │
│    URGENT: Train 12 engineers immediately  │
│                                             │
│ ⚠️ SINGLE POINT OF FAILURE                 │
│    Only John Smith can operate Tank Room   │
│    Recommend: Cross-train 2 backup staff   │
│                                             │
│ 📈 TRAINING SUCCESS                        │
│    Viaflo Packing improved 35% this month  │
│    Keep momentum: Continue current program │
│                                             │
│ 🎯 HIGH IMPACT OPPORTUNITY                 │
│    Training Shift A in filling would       │
│    improve coverage by 25% (Impact: 8.5)   │
│                                             │
│ 👤 PROMOTION READY                         │
│    Sarah Jones: 92% proficiency achieved   │
│    Consider: Lead operator role            │
│                                             │
│ ⏸️ LEARNING PLATEAU                        │
│    8 engineers: No progress in 60 days     │
│    Action: Review training approach        │
│                                             │
│ 🔄 CROSS-TRAINING OPPORTUNITY              │
│    Shift A: Strong in filling              │
│    Shift B: Strong in packing              │
│    Swap 2 engineers for skill diversity    │
│                                             │
│ ⚖️ UNBALANCED SKILLS                       │
│    Viaflo area: 80% filling, 40% racking   │
│    Focus: Racking training next quarter    │
│                                             │
│ 💰 RESOURCE ALLOCATION                     │
│    Line 7 needs 3x training hours vs Line 1│
│    Budget: Allocate more resources to L7   │
│                                             │
│ 📊 SHIFT VULNERABILITY                     │
│    Shift A: 8 machines under 50% coverage  │
│    Risk: Production disruption if absences │
└─────────────────────────────────────────────┘
```

## Success Metrics

After implementation, managers should be able to:
1. ✅ Identify exact skills to train next (not guessing)
2. ✅ Quantify production risk from skill gaps
3. ✅ Prioritize training by ROI
4. ✅ Detect single points of failure
5. ✅ Track which training programs work
6. ✅ Make data-driven staffing decisions

This transforms analytics from "interesting numbers" to "actionable business intelligence".
