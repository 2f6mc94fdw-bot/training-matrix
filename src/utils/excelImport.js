import * as XLSX from 'xlsx';

// Import data from Excel file
export const importFromExcel = (file) => {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    
    reader.onload = (e) => {
      try {
        const data = new Uint8Array(e.target.result);
        const workbook = XLSX.read(data, { type: 'array' });
        
        const result = parseWorkbook(workbook);
        resolve(result);
      } catch (error) {
        reject(new Error('Failed to parse Excel file: ' + error.message));
      }
    };
    
    reader.onerror = () => {
      reject(new Error('Failed to read file'));
    };
    
    reader.readAsArrayBuffer(file);
  });
};

// Parse workbook and extract data
const parseWorkbook = (workbook) => {
  const imported = {
    productionAreas: [],
    engineers: [],
    assessments: {}
  };
  
  // Parse Engineer Summary sheet
  if (workbook.SheetNames.includes('Engineer Summary')) {
    const sheet = workbook.Sheets['Engineer Summary'];
    const jsonData = XLSX.utils.sheet_to_json(sheet);
    
    jsonData.forEach((row, index) => {
      if (row.Engineer) {
        imported.engineers.push({
          id: `eng_${Date.now()}_${index}`,
          name: row.Engineer,
          shift: row.Shift || 'Day Shift'
        });
      }
    });
  }
  
  // Parse Production Areas sheet
  if (workbook.SheetNames.includes('Production Areas')) {
    const sheet = workbook.Sheets['Production Areas'];
    const jsonData = XLSX.utils.sheet_to_json(sheet);
    
    jsonData.forEach((row, index) => {
      if (row['Production Area']) {
        imported.productionAreas.push({
          id: `area_${Date.now()}_${index}`,
          name: row['Production Area'],
          machines: []
        });
      }
    });
  }
  
  // Parse Detailed Scores sheet to get machines and assessments
  if (workbook.SheetNames.includes('Detailed Scores')) {
    const sheet = workbook.Sheets['Detailed Scores'];
    const range = XLSX.utils.decode_range(sheet['!ref']);
    
    // Get headers (machine names)
    const machineHeaders = [];
    for (let col = 2; col <= range.e.c; col++) {
      const cellAddress = XLSX.utils.encode_cell({ r: 0, c: col });
      const cell = sheet[cellAddress];
      if (cell && cell.v) {
        const [areaName, machineName] = cell.v.split(' - ');
        machineHeaders.push({ areaName, machineName, col });
      }
    }
    
    // Add machines to production areas
    machineHeaders.forEach((header, index) => {
      let area = imported.productionAreas.find(a => a.name === header.areaName);
      if (!area) {
        area = {
          id: `area_${Date.now()}_${index}`,
          name: header.areaName,
          machines: []
        };
        imported.productionAreas.push(area);
      }
      
      if (!area.machines.find(m => m.name === header.machineName)) {
        area.machines.push({
          id: `machine_${Date.now()}_${index}`,
          name: header.machineName,
          importance: 1,
          competencies: [
            {
              id: `comp_${Date.now()}_${index}_1`,
              name: 'Operation',
              maxScore: 3
            },
            {
              id: `comp_${Date.now()}_${index}_2`,
              name: 'Troubleshooting',
              maxScore: 3
            },
            {
              id: `comp_${Date.now()}_${index}_3`,
              name: 'Maintenance',
              maxScore: 3
            }
          ]
        });
      }
    });
    
    // Parse assessment scores
    for (let row = 1; row <= range.e.r; row++) {
      const engineerCell = sheet[XLSX.utils.encode_cell({ r: row, c: 0 })];
      if (!engineerCell) continue;
      
      const engineerName = engineerCell.v;
      const engineer = imported.engineers.find(e => e.name === engineerName);
      if (!engineer) continue;
      
      machineHeaders.forEach(header => {
        const cell = sheet[XLSX.utils.encode_cell({ r: row, c: header.col })];
        if (!cell) return;
        
        const scoreMatch = String(cell.v).match(/^(\d+)\/(\d+)$/);
        if (scoreMatch) {
          const score = parseInt(scoreMatch[1]);
          const area = imported.productionAreas.find(a => a.name === header.areaName);
          const machine = area?.machines.find(m => m.name === header.machineName);
          
          if (machine && machine.competencies.length > 0) {
            // Distribute score across competencies
            const scorePerComp = Math.floor(score / machine.competencies.length);
            machine.competencies.forEach(comp => {
              const key = `${engineer.id}-${area.id}-${machine.id}-${comp.id}`;
              imported.assessments[key] = Math.min(scorePerComp, comp.maxScore);
            });
          }
        }
      });
    }
  }
  
  return imported;
};

// Import engineers from CSV
export const importEngineersFromCSV = (file) => {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    
    reader.onload = (e) => {
      try {
        const text = e.target.result;
        const workbook = XLSX.read(text, { type: 'string' });
        const sheet = workbook.Sheets[workbook.SheetNames[0]];
        const jsonData = XLSX.utils.sheet_to_json(sheet);
        
        const engineers = jsonData.map((row, index) => ({
          id: row.ID || `eng_${Date.now()}_${index}`,
          name: row.Name || `Engineer ${index + 1}`,
          shift: row.Shift || 'Day Shift'
        }));
        
        resolve({ engineers });
      } catch (error) {
        reject(new Error('Failed to parse CSV file: ' + error.message));
      }
    };
    
    reader.onerror = () => {
      reject(new Error('Failed to read file'));
    };
    
    reader.readAsText(file);
  });
};

// Validate imported data
export const validateImportedData = (imported) => {
  const errors = [];
  
  if (!imported.engineers || imported.engineers.length === 0) {
    errors.push('No engineers found in imported data');
  }
  
  if (!imported.productionAreas || imported.productionAreas.length === 0) {
    errors.push('No production areas found in imported data');
  }
  
  imported.productionAreas?.forEach((area, areaIndex) => {
    if (!area.name) {
      errors.push(`Production area at index ${areaIndex} has no name`);
    }
    
    if (!area.machines || area.machines.length === 0) {
      errors.push(`Production area "${area.name}" has no machines`);
    }
    
    area.machines?.forEach((machine, machineIndex) => {
      if (!machine.name) {
        errors.push(`Machine at index ${machineIndex} in area "${area.name}" has no name`);
      }
      
      if (!machine.competencies || machine.competencies.length === 0) {
        errors.push(`Machine "${machine.name}" in area "${area.name}" has no competencies`);
      }
    });
  });
  
  return {
    isValid: errors.length === 0,
    errors
  };
};
