import * as XLSX from 'xlsx';

// Export complete training matrix to Excel with multiple sheets
export const exportToExcel = (data) => {
  const workbook = XLSX.utils.book_new();
  
  // Sheet 1: Engineer Summary
  const engineerData = data.engineers.map(eng => {
    const scores = calculateEngineerScores(eng.id, data);
    return {
      'Engineer': eng.name,
      'Shift': eng.shift,
      'Total Score': scores.totalScore,
      'Max Possible': scores.maxScore,
      'Completion %': scores.completionPercent.toFixed(1) + '%',
      'Weighted Score': scores.weightedScore.toFixed(1)
    };
  });
  
  const engineerSheet = XLSX.utils.json_to_sheet(engineerData);
  XLSX.utils.book_append_sheet(workbook, engineerSheet, 'Engineer Summary');
  
  // Sheet 2: Production Areas
  const areaData = data.productionAreas.map(area => ({
    'Production Area': area.name,
    'Number of Machines': area.machines.length,
    'Total Competencies': area.machines.reduce((sum, m) => sum + m.competencies.length, 0)
  }));
  
  const areaSheet = XLSX.utils.json_to_sheet(areaData);
  XLSX.utils.book_append_sheet(workbook, areaSheet, 'Production Areas');
  
  // Sheet 3: Detailed Scores - Create a matrix
  const detailedData = [];
  
  // Header row
  const header = ['Engineer', 'Shift'];
  data.productionAreas.forEach(area => {
    area.machines.forEach(machine => {
      header.push(`${area.name} - ${machine.name}`);
    });
  });
  detailedData.push(header);
  
  // Data rows
  data.engineers.forEach(eng => {
    const row = [eng.name, eng.shift];
    data.productionAreas.forEach(area => {
      area.machines.forEach(machine => {
        const machineScore = getMachineScore(eng.id, area.id, machine.id, data);
        row.push(machineScore.score + '/' + machineScore.maxScore);
      });
    });
    detailedData.push(row);
  });
  
  const detailedSheet = XLSX.utils.aoa_to_sheet(detailedData);
  XLSX.utils.book_append_sheet(workbook, detailedSheet, 'Detailed Scores');
  
  // Sheet 4: Skills Gap Analysis
  const gapData = [];
  data.productionAreas.forEach(area => {
    area.machines.forEach(machine => {
      machine.competencies.forEach(comp => {
        const avgScore = getCompetencyAverageScore(area.id, machine.id, comp.id, data);
        const gap = comp.maxScore - avgScore;
        if (gap > 0) {
          gapData.push({
            'Production Area': area.name,
            'Machine': machine.name,
            'Competency': comp.name,
            'Average Score': avgScore.toFixed(2),
            'Max Score': comp.maxScore,
            'Gap': gap.toFixed(2),
            'Priority': gap > comp.maxScore * 0.5 ? 'High' : gap > comp.maxScore * 0.25 ? 'Medium' : 'Low'
          });
        }
      });
    });
  });
  
  const gapSheet = XLSX.utils.json_to_sheet(gapData);
  XLSX.utils.book_append_sheet(workbook, gapSheet, 'Skills Gap');
  
  // Generate and download file
  XLSX.writeFile(workbook, `training_matrix_${new Date().toISOString().split('T')[0]}.xlsx`);
};

// Export individual engineer report
export const exportEngineerReport = (engineerId, data) => {
  const engineer = data.engineers.find(e => e.id === engineerId);
  if (!engineer) return;
  
  const workbook = XLSX.utils.book_new();
  
  // Sheet 1: Engineer Info
  const infoData = [
    ['Engineer Report'],
    ['Name:', engineer.name],
    ['Shift:', engineer.shift],
    ['Report Date:', new Date().toLocaleDateString()],
    [''],
    ['Overall Scores'],
  ];
  
  const scores = calculateEngineerScores(engineerId, data);
  infoData.push(['Total Score:', scores.totalScore]);
  infoData.push(['Max Possible:', scores.maxScore]);
  infoData.push(['Completion:', scores.completionPercent.toFixed(1) + '%']);
  infoData.push(['Weighted Score:', scores.weightedScore.toFixed(1)]);
  
  const infoSheet = XLSX.utils.aoa_to_sheet(infoData);
  XLSX.utils.book_append_sheet(workbook, infoSheet, 'Summary');
  
  // Sheet 2: Detailed Competencies
  const detailedData = [];
  
  data.productionAreas.forEach(area => {
    area.machines.forEach(machine => {
      machine.competencies.forEach(comp => {
        const score = getCompetencyScore(engineerId, area.id, machine.id, comp.id, data);
        detailedData.push({
          'Production Area': area.name,
          'Machine': machine.name,
          'Competency': comp.name,
          'Score': score,
          'Max Score': comp.maxScore,
          'Status': score === comp.maxScore ? 'Complete' : score > 0 ? 'In Progress' : 'Not Started'
        });
      });
    });
  });
  
  const detailedSheet = XLSX.utils.json_to_sheet(detailedData);
  XLSX.utils.book_append_sheet(workbook, detailedSheet, 'Competencies');
  
  XLSX.writeFile(workbook, `engineer_report_${engineer.name.replace(/\s/g, '_')}_${new Date().toISOString().split('T')[0]}.xlsx`);
};

// Export engineers list as CSV
export const exportEngineersCSV = (engineers) => {
  const csvData = engineers.map(eng => ({
    'Name': eng.name,
    'Shift': eng.shift,
    'ID': eng.id
  }));
  
  const worksheet = XLSX.utils.json_to_sheet(csvData);
  const csv = XLSX.utils.sheet_to_csv(worksheet);
  
  const blob = new Blob([csv], { type: 'text/csv' });
  const url = window.URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = `engineers_${new Date().toISOString().split('T')[0]}.csv`;
  link.click();
  window.URL.revokeObjectURL(url);
};

// Helper functions
const calculateEngineerScores = (engineerId, data) => {
  let totalScore = 0;
  let maxScore = 0;
  let weightedScore = 0;
  let totalWeight = 0;
  
  data.productionAreas.forEach(area => {
    area.machines.forEach(machine => {
      machine.competencies.forEach(comp => {
        const score = getCompetencyScore(engineerId, area.id, machine.id, comp.id, data);
        totalScore += score;
        maxScore += comp.maxScore;
        
        const weight = machine.importance || 1;
        weightedScore += score * weight;
        totalWeight += comp.maxScore * weight;
      });
    });
  });
  
  return {
    totalScore,
    maxScore,
    completionPercent: maxScore > 0 ? (totalScore / maxScore) * 100 : 0,
    weightedScore: totalWeight > 0 ? (weightedScore / totalWeight) * 100 : 0
  };
};

const getMachineScore = (engineerId, areaId, machineId, data) => {
  const area = data.productionAreas.find(a => a.id === areaId);
  if (!area) return { score: 0, maxScore: 0 };
  
  const machine = area.machines.find(m => m.id === machineId);
  if (!machine) return { score: 0, maxScore: 0 };
  
  let score = 0;
  let maxScore = 0;
  
  machine.competencies.forEach(comp => {
    score += getCompetencyScore(engineerId, areaId, machineId, comp.id, data);
    maxScore += comp.maxScore;
  });
  
  return { score, maxScore };
};

const getCompetencyScore = (engineerId, areaId, machineId, compId, data) => {
  const key = `${engineerId}-${areaId}-${machineId}-${compId}`;
  return data.assessments[key] || 0;
};

const getCompetencyAverageScore = (areaId, machineId, compId, data) => {
  let total = 0;
  let count = 0;
  
  data.engineers.forEach(eng => {
    const score = getCompetencyScore(eng.id, areaId, machineId, compId, data);
    if (score > 0) {
      total += score;
      count++;
    }
  });
  
  return count > 0 ? total / count : 0;
};
