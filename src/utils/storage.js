// Storage utility for managing data persistence and audit logging
import bcrypt from 'bcryptjs';

const STORAGE_KEY = 'training_matrix_data';
const AUDIT_KEY = 'training_matrix_audit';

// Password hashing utilities
export const hashPassword = (password) => {
  return bcrypt.hashSync(password, 10);
};

export const verifyPassword = (password, hashedPassword) => {
  return bcrypt.compareSync(password, hashedPassword);
};

// Initialize default data structure
export const getDefaultData = () => ({
  productionAreas: [],
  engineers: [],
  users: [
    {  id: 'admin',
      username: 'admin',
      password: hashPassword('admin123'),
      role: 'admin',
      engineerId: null
    }
  ],
  assessments: {},
  certifications: [],
  snapshots: [],
  coreSkills: {
    categories: [
      {
        id: 'mechanical',
        name: 'Mechanical Skills',
        skills: [
          { id: 'troubleshooting', name: 'Troubleshooting', maxScore: 3 },
          { id: 'preventive-maintenance', name: 'Preventive Maintenance', maxScore: 3 },
          { id: 'repair', name: 'Equipment Repair', maxScore: 3 },
          { id: 'installation', name: 'Installation', maxScore: 3 }
        ]
      },
      {
        id: 'electrical',
        name: 'Electrical Skills',
        skills: [
          { id: 'wiring', name: 'Wiring & Circuits', maxScore: 3 },
          { id: 'circuit-diagnosis', name: 'Circuit Diagnosis', maxScore: 3 },
          { id: 'plc', name: 'PLC Programming', maxScore: 3 },
          { id: 'motor-control', name: 'Motor Control Systems', maxScore: 3 }
        ]
      },
      {
        id: 'software',
        name: 'Software Skills',
        skills: [
          { id: 'programming', name: 'Basic Programming', maxScore: 3 },
          { id: 'hmi', name: 'HMI Operation', maxScore: 3 },
          { id: 'data-analysis', name: 'Data Analysis', maxScore: 3 },
          { id: 'scada', name: 'SCADA Systems', maxScore: 3 }
        ]
      },
      {
        id: 'safety',
        name: 'Safety Skills',
        skills: [
          { id: 'lockout-tagout', name: 'Lockout/Tagout', maxScore: 3 },
          { id: 'confined-spaces', name: 'Confined Spaces', maxScore: 3 },
          { id: 'hazard-identification', name: 'Hazard Identification', maxScore: 3 },
          { id: 'emergency-response', name: 'Emergency Response', maxScore: 3 }
        ]
      },
      {
        id: 'leadership',
        name: 'Leadership & Soft Skills',
        skills: [
          { id: 'communication', name: 'Communication', maxScore: 3 },
          { id: 'teamwork', name: 'Teamwork & Collaboration', maxScore: 3 },
          { id: 'problem-solving', name: 'Problem Solving', maxScore: 3 },
          { id: 'training-others', name: 'Training Others', maxScore: 3 }
        ]
      }
    ],
    assessments: {}
  },
  version: '1.0.0'
});

// Check if a password is already hashed (bcrypt hashes start with $2)
const isPasswordHashed = (password) => {
  return password && typeof password === 'string' && password.startsWith('$2');
};

// Migrate plain text passwords to hashed passwords
const migratePasswords = (data) => {
  let needsMigration = false;

  const migratedUsers = data.users.map(user => {
    if (!isPasswordHashed(user.password)) {
      needsMigration = true;
      console.log(`Migrating password for user: ${user.username}`);
      return {
        ...user,
        password: hashPassword(user.password)
      };
    }
    return user;
  });

  if (needsMigration) {
    const migratedData = { ...data, users: migratedUsers };
    localStorage.setItem(STORAGE_KEY, JSON.stringify(migratedData));
    console.log('✅ Migrated passwords to bcrypt hashes');
    return migratedData;
  }

  return data;
};

// Load data from storage
export const loadData = () => {
  try {
    const stored = localStorage.getItem(STORAGE_KEY);
    if (stored) {
      const data = JSON.parse(stored);
      const mergedData = { ...getDefaultData(), ...data };
      // Migrate plain text passwords to hashed passwords
      return migratePasswords(mergedData);
    }
    return getDefaultData();
  } catch (error) {
    console.error('Error loading data:', error);
    return getDefaultData();
  }
};

// Save data to storage
export const saveData = (data) => {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(data));
    return true;
  } catch (error) {
    console.error('Error saving data:', error);
    return false;
  }
};

// Audit trail functions
export const logAction = (user, action, details) => {
  try {
    const logs = getAuditLogs();
    const newLog = {
      id: Date.now().toString(),
      timestamp: new Date().toISOString(),
      user: user,
      action: action,
      details: details
    };
    logs.unshift(newLog);
    
    // Keep only last 1000 logs
    if (logs.length > 1000) {
      logs.splice(1000);
    }
    
    localStorage.setItem(AUDIT_KEY, JSON.stringify(logs));
    return true;
  } catch (error) {
    console.error('Error logging action:', error);
    return false;
  }
};

export const getAuditLogs = () => {
  try {
    const stored = localStorage.getItem(AUDIT_KEY);
    return stored ? JSON.parse(stored) : [];
  } catch (error) {
    console.error('Error getting audit logs:', error);
    return [];
  }
};

// Snapshot functions for progress history
export const createSnapshot = (data, description) => {
  const snapshot = {
    id: Date.now().toString(),
    timestamp: new Date().toISOString(),
    description: description,
    data: {
      productionAreas: data.productionAreas,
      engineers: data.engineers,
      assessments: data.assessments
    }
  };
  
  const snapshots = data.snapshots || [];
  snapshots.unshift(snapshot);
  
  // Keep only last 50 snapshots
  if (snapshots.length > 50) {
    snapshots.splice(50);
  }
  
  return snapshots;
};

// Export data as JSON for backup
export const exportBackup = () => {
  const data = loadData();
  const backup = {
    ...data,
    exportDate: new Date().toISOString(),
    version: '1.0.0'
  };
  return JSON.stringify(backup, null, 2);
};

// Import data from JSON backup
export const importBackup = (jsonString) => {
  try {
    const data = JSON.parse(jsonString);
    saveData(data);
    return { success: true, message: 'Backup imported successfully' };
  } catch (error) {
    return { success: false, message: 'Invalid backup file: ' + error.message };
  }
};

// Clear all data
export const clearAllData = () => {
  try {
    localStorage.removeItem(STORAGE_KEY);
    localStorage.removeItem(AUDIT_KEY);
    return true;
  } catch (error) {
    console.error('Error clearing data:', error);
    return false;
  }
};
