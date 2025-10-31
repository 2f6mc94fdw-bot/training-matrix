import { useState, useEffect } from 'react';
import { loadData, saveData, logAction, createSnapshot } from '../utils/storage';

export const useData = (currentUser) => {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  
  useEffect(() => {
    const loadedData = loadData();
    setData(loadedData);
    setLoading(false);
  }, []);
  
  const updateData = (newData, actionDescription) => {
    setData(newData);
    saveData(newData);
    
    if (currentUser && actionDescription) {
      logAction(currentUser.username, 'data_update', actionDescription);
    }
  };
  
  const addProductionArea = (area) => {
    const newArea = {
      id: `area_${Date.now()}`,
      name: area.name,
      machines: []
    };
    
    const newData = {
      ...data,
      productionAreas: [...data.productionAreas, newArea]
    };
    
    updateData(newData, `Added production area: ${area.name}`);
    return newArea;
  };
  
  const updateProductionArea = (areaId, updates) => {
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId ? { ...area, ...updates } : area
      )
    };
    
    updateData(newData, `Updated production area: ${updates.name || areaId}`);
  };
  
  const deleteProductionArea = (areaId) => {
    const area = data.productionAreas.find(a => a.id === areaId);
    const newData = {
      ...data,
      productionAreas: data.productionAreas.filter(a => a.id !== areaId)
    };
    
    updateData(newData, `Deleted production area: ${area?.name}`);
  };
  
  const addMachine = (areaId, machine) => {
    const newMachine = {
      id: `machine_${Date.now()}`,
      name: machine.name,
      importance: machine.importance || 1,
      competencies: []
    };
    
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId
          ? { ...area, machines: [...area.machines, newMachine] }
          : area
      )
    };
    
    updateData(newData, `Added machine: ${machine.name}`);
    return newMachine;
  };
  
  const updateMachine = (areaId, machineId, updates) => {
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId
          ? {
              ...area,
              machines: area.machines.map(machine =>
                machine.id === machineId ? { ...machine, ...updates } : machine
              )
            }
          : area
      )
    };
    
    updateData(newData, `Updated machine: ${updates.name || machineId}`);
  };
  
  const deleteMachine = (areaId, machineId) => {
    const area = data.productionAreas.find(a => a.id === areaId);
    const machine = area?.machines.find(m => m.id === machineId);
    
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId
          ? { ...area, machines: area.machines.filter(m => m.id !== machineId) }
          : area
      )
    };
    
    updateData(newData, `Deleted machine: ${machine?.name}`);
  };
  
  const addCompetency = (areaId, machineId, competency) => {
    const newComp = {
      id: `comp_${Date.now()}`,
      name: competency.name,
      maxScore: competency.maxScore || 3
    };
    
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId
          ? {
              ...area,
              machines: area.machines.map(machine =>
                machine.id === machineId
                  ? { ...machine, competencies: [...machine.competencies, newComp] }
                  : machine
              )
            }
          : area
      )
    };
    
    updateData(newData, `Added competency: ${competency.name}`);
    return newComp;
  };
  
  const updateCompetency = (areaId, machineId, compId, updates) => {
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId
          ? {
              ...area,
              machines: area.machines.map(machine =>
                machine.id === machineId
                  ? {
                      ...machine,
                      competencies: machine.competencies.map(comp =>
                        comp.id === compId ? { ...comp, ...updates } : comp
                      )
                    }
                  : machine
              )
            }
          : area
      )
    };
    
    updateData(newData, `Updated competency: ${updates.name || compId}`);
  };
  
  const deleteCompetency = (areaId, machineId, compId) => {
    const newData = {
      ...data,
      productionAreas: data.productionAreas.map(area =>
        area.id === areaId
          ? {
              ...area,
              machines: area.machines.map(machine =>
                machine.id === machineId
                  ? {
                      ...machine,
                      competencies: machine.competencies.filter(c => c.id !== compId)
                    }
                  : machine
              )
            }
          : area
      )
    };
    
    updateData(newData, `Deleted competency`);
  };
  
  const addEngineer = (engineer) => {
    const newEngineer = {
      id: `eng_${Date.now()}`,
      name: engineer.name,
      shift: engineer.shift
    };
    
    const newData = {
      ...data,
      engineers: [...data.engineers, newEngineer]
    };
    
    updateData(newData, `Added engineer: ${engineer.name}`);
    return newEngineer;
  };
  
  const updateEngineer = (engineerId, updates) => {
    const newData = {
      ...data,
      engineers: data.engineers.map(eng =>
        eng.id === engineerId ? { ...eng, ...updates } : eng
      )
    };
    
    updateData(newData, `Updated engineer: ${updates.name || engineerId}`);
  };
  
  const deleteEngineer = (engineerId) => {
    const engineer = data.engineers.find(e => e.id === engineerId);
    const newData = {
      ...data,
      engineers: data.engineers.filter(e => e.id !== engineerId)
    };
    
    updateData(newData, `Deleted engineer: ${engineer?.name}`);
  };
  
  const updateAssessment = (engineerId, areaId, machineId, compId, score) => {
    const key = `${engineerId}-${areaId}-${machineId}-${compId}`;
    const newData = {
      ...data,
      assessments: {
        ...data.assessments,
        [key]: {
          score,
          lastUpdated: new Date().toISOString(),
          updatedBy: currentUser?.username || 'System'
        }
      }
    };

    updateData(newData, `Updated assessment for engineer`);
  };
  
  const bulkUpdateAssessments = (updates) => {
    const newAssessments = { ...data.assessments };
    updates.forEach(({ engineerId, areaId, machineId, compId, score }) => {
      const key = `${engineerId}-${areaId}-${machineId}-${compId}`;
      newAssessments[key] = {
        score,
        lastUpdated: new Date().toISOString(),
        updatedBy: currentUser?.username || 'System'
      };
    });

    const newData = {
      ...data,
      assessments: newAssessments
    };

    updateData(newData, `Bulk updated ${updates.length} assessments`);
  };
  
  const addUser = (user) => {
    const newUser = {
      id: `user_${Date.now()}`,
      username: user.username,
      password: user.password,
      role: user.role,
      engineerId: user.engineerId || null
    };
    
    const newData = {
      ...data,
      users: [...data.users, newUser]
    };
    
    updateData(newData, `Added user: ${user.username}`);
    return newUser;
  };
  
  const deleteUser = (userId) => {
    const user = data.users.find(u => u.id === userId);
    const newData = {
      ...data,
      users: data.users.filter(u => u.id !== userId)
    };

    updateData(newData, `Deleted user: ${user?.username}`);
  };

  const resetPassword = (userId, newPassword) => {
    const user = data.users.find(u => u.id === userId);
    if (!user) return;

    const newData = {
      ...data,
      users: data.users.map(u =>
        u.id === userId
          ? { ...u, password: newPassword }
          : u
      )
    };

    updateData(newData, `Reset password for user: ${user.username}`);
  };

  const addCertification = (cert) => {
    const newCert = {
      id: Date.now(),
      name: cert.name,
      validityDays: cert.validityDays || 365
    };

    const newData = {
      ...data,
      certifications: [...(data.certifications || []), newCert]
    };

    updateData(newData, `Added certification: ${cert.name}`);
    return newCert;
  };
  
  const updateCertification = (certId, updates) => {
    const newData = {
      ...data,
      certifications: data.certifications.map(cert =>
        cert.id === certId ? { ...cert, ...updates } : cert
      )
    };
    
    updateData(newData, `Updated certification`);
  };
  
  const deleteCertification = (certId) => {
    const newData = {
      ...data,
      certifications: data.certifications.filter(c => c.id !== certId)
    };
    
    updateData(newData, `Deleted certification`);
  };
  
  const takeSnapshot = (description) => {
    const snapshots = createSnapshot(data, description);
    const newData = {
      ...data,
      snapshots
    };
    
    updateData(newData, `Created snapshot: ${description}`);
  };
  
  const replaceData = (newData) => {
    setData(newData);
    saveData(newData);
  };
  
  return {
    data,
    loading,
    addProductionArea,
    updateProductionArea,
    deleteProductionArea,
    addMachine,
    updateMachine,
    deleteMachine,
    addCompetency,
    updateCompetency,
    deleteCompetency,
    addEngineer,
    updateEngineer,
    deleteEngineer,
    updateAssessment,
    bulkUpdateAssessments,
    addUser,
    deleteUser,
    resetPassword,
    addCertification,
    updateCertification,
    deleteCertification,
    takeSnapshot,
    replaceData
  };
};
