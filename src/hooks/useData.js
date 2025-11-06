import { useState, useEffect } from 'react';
import * as api from '../utils/api';
import toast from 'react-hot-toast';

export const useData = (currentUser) => {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);

  // Load all data from API
  const loadAllData = async () => {
    try {
      setLoading(true);
      const allData = await api.getAllData();
      setData(allData);
    } catch (error) {
      console.error('Failed to load data:', error);
      toast.error('Failed to load data from server');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadAllData();
  }, []);

  const logAction = async (action, details) => {
    if (currentUser) {
      try {
        await api.logAction(currentUser.username, action, details);
      } catch (error) {
        console.error('Failed to log action:', error);
      }
    }
  };

  // Production Areas
  const addProductionArea = async (area) => {
    try {
      const newArea = await api.createProductionArea(area);
      await loadAllData(); // Reload data
      await logAction('data_update', `Added production area: ${area.name}`);
      toast.success('Production area added');
      return newArea;
    } catch (error) {
      console.error('Failed to add production area:', error);
      toast.error('Failed to add production area');
      throw error;
    }
  };

  const updateProductionArea = async (areaId, updates) => {
    try {
      await api.updateProductionArea(areaId, updates);
      await loadAllData();
      await logAction('data_update', `Updated production area: ${updates.name || areaId}`);
      toast.success('Production area updated');
    } catch (error) {
      console.error('Failed to update production area:', error);
      toast.error('Failed to update production area');
      throw error;
    }
  };

  const deleteProductionArea = async (areaId) => {
    try {
      const area = data.productionAreas.find(a => a.id === areaId);
      await api.deleteProductionArea(areaId);
      await loadAllData();
      await logAction('data_update', `Deleted production area: ${area?.name}`);
      toast.success('Production area deleted');
    } catch (error) {
      console.error('Failed to delete production area:', error);
      toast.error('Failed to delete production area');
      throw error;
    }
  };

  // Machines
  const addMachine = async (areaId, machine) => {
    try {
      const newMachine = await api.createMachine(areaId, machine);
      await loadAllData();
      await logAction('data_update', `Added machine: ${machine.name}`);
      toast.success('Machine added');
      return newMachine;
    } catch (error) {
      console.error('Failed to add machine:', error);
      toast.error('Failed to add machine');
      throw error;
    }
  };

  const updateMachine = async (areaId, machineId, updates) => {
    try {
      await api.updateMachine(areaId, machineId, updates);
      await loadAllData();
      await logAction('data_update', `Updated machine: ${updates.name || machineId}`);
      toast.success('Machine updated');
    } catch (error) {
      console.error('Failed to update machine:', error);
      toast.error('Failed to update machine');
      throw error;
    }
  };

  const deleteMachine = async (areaId, machineId) => {
    try {
      const area = data.productionAreas.find(a => a.id === areaId);
      const machine = area?.machines.find(m => m.id === machineId);
      await api.deleteMachine(areaId, machineId);
      await loadAllData();
      await logAction('data_update', `Deleted machine: ${machine?.name}`);
      toast.success('Machine deleted');
    } catch (error) {
      console.error('Failed to delete machine:', error);
      toast.error('Failed to delete machine');
      throw error;
    }
  };

  // Competencies
  const addCompetency = async (areaId, machineId, competency) => {
    try {
      const newComp = await api.createCompetency(areaId, machineId, competency);
      await loadAllData();
      await logAction('data_update', `Added competency: ${competency.name}`);
      toast.success('Competency added');
      return newComp;
    } catch (error) {
      console.error('Failed to add competency:', error);
      toast.error('Failed to add competency');
      throw error;
    }
  };

  const updateCompetency = async (areaId, machineId, compId, updates) => {
    try {
      await api.updateCompetency(areaId, machineId, compId, updates);
      await loadAllData();
      await logAction('data_update', `Updated competency: ${updates.name || compId}`);
      toast.success('Competency updated');
    } catch (error) {
      console.error('Failed to update competency:', error);
      toast.error('Failed to update competency');
      throw error;
    }
  };

  const deleteCompetency = async (areaId, machineId, compId) => {
    try {
      await api.deleteCompetency(areaId, machineId, compId);
      await loadAllData();
      await logAction('data_update', 'Deleted competency');
      toast.success('Competency deleted');
    } catch (error) {
      console.error('Failed to delete competency:', error);
      toast.error('Failed to delete competency');
      throw error;
    }
  };

  // Engineers
  const addEngineer = async (engineer) => {
    try {
      const result = await api.createEngineer(engineer);
      await loadAllData();
      await logAction('data_update', `Added engineer: ${engineer.name}`);
      toast.success(`Engineer added${result.userCreated ? ` (User: ${result.username}, Password: password)` : ''}`);
      return result;
    } catch (error) {
      console.error('Failed to add engineer:', error);
      toast.error('Failed to add engineer');
      throw error;
    }
  };

  const updateEngineer = async (engineerId, updates) => {
    try {
      await api.updateEngineer(engineerId, updates);
      await loadAllData();
      await logAction('data_update', `Updated engineer: ${updates.name || engineerId}`);
      toast.success('Engineer updated');
    } catch (error) {
      console.error('Failed to update engineer:', error);
      toast.error('Failed to update engineer');
      throw error;
    }
  };

  const deleteEngineer = async (engineerId) => {
    try {
      const engineer = data.engineers.find(e => e.id === engineerId);
      await api.deleteEngineer(engineerId);
      await loadAllData();
      await logAction('data_update', `Deleted engineer: ${engineer?.name}`);
      toast.success('Engineer deleted');
    } catch (error) {
      console.error('Failed to delete engineer:', error);
      toast.error('Failed to delete engineer');
      throw error;
    }
  };

  // Assessments
  const updateAssessment = async (engineerId, areaId, machineId, compId, score) => {
    try {
      await api.saveAssessment({
        engineerId,
        areaId,
        machineId,
        competencyId: compId,
        score
      });

      // Update local state optimistically
      const key = `${engineerId}-${areaId}-${machineId}-${compId}`;
      const newAssessments = { ...data.assessments };
      newAssessments[key] = {
        score,
        lastUpdated: new Date().toISOString(),
        updatedBy: currentUser?.username || 'System'
      };
      setData({ ...data, assessments: newAssessments });

      await logAction('data_update', 'Updated assessment');
    } catch (error) {
      console.error('Failed to update assessment:', error);
      toast.error('Failed to save assessment');
      throw error;
    }
  };

  const bulkUpdateAssessments = async (updates) => {
    try {
      await api.bulkUpdateAssessments(updates);
      await loadAllData();
      await logAction('data_update', `Bulk updated ${updates.length} assessments`);
      toast.success(`Updated ${updates.length} assessments`);
    } catch (error) {
      console.error('Failed to bulk update assessments:', error);
      toast.error('Failed to bulk update assessments');
      throw error;
    }
  };

  // Users
  const addUser = async (user) => {
    try {
      const newUser = await api.createUser(user);
      await loadAllData();
      await logAction('data_update', `Added user: ${user.username}`);
      toast.success('User added');
      return newUser;
    } catch (error) {
      console.error('Failed to add user:', error);
      toast.error('Failed to add user');
      throw error;
    }
  };

  const deleteUser = async (userId) => {
    try {
      const user = data.users.find(u => u.id === userId);
      await api.deleteUser(userId);
      await loadAllData();
      await logAction('data_update', `Deleted user: ${user?.username}`);
      toast.success('User deleted');
    } catch (error) {
      console.error('Failed to delete user:', error);
      toast.error('Failed to delete user');
      throw error;
    }
  };

  const resetPassword = async (userId, newPassword) => {
    try {
      const user = data.users.find(u => u.id === userId);
      await api.resetPassword(userId, newPassword);
      await logAction('data_update', `Reset password for user: ${user?.username}`);
      toast.success('Password reset successfully');
    } catch (error) {
      console.error('Failed to reset password:', error);
      toast.error('Failed to reset password');
      throw error;
    }
  };

  // Certifications
  const addCertification = async (cert) => {
    try {
      const newCert = await api.createCertification(cert);
      await loadAllData();
      await logAction('data_update', `Added certification: ${cert.name}`);
      toast.success('Certification added');
      return newCert;
    } catch (error) {
      console.error('Failed to add certification:', error);
      toast.error('Failed to add certification');
      throw error;
    }
  };

  const updateCertification = async (certId, updates) => {
    // Note: API doesn't have update cert endpoint yet
    console.warn('updateCertification not implemented in API');
    toast.error('Update certification not yet implemented');
  };

  const deleteCertification = async (certId) => {
    try {
      await api.deleteCertification(certId);
      await loadAllData();
      await logAction('data_update', 'Deleted certification');
      toast.success('Certification deleted');
    } catch (error) {
      console.error('Failed to delete certification:', error);
      toast.error('Failed to delete certification');
      throw error;
    }
  };

  // Snapshots
  const takeSnapshot = async (description) => {
    try {
      await api.createSnapshot(description);
      await loadAllData();
      await logAction('data_update', `Created snapshot: ${description}`);
      toast.success('Snapshot created');
    } catch (error) {
      console.error('Failed to create snapshot:', error);
      toast.error('Failed to create snapshot');
      throw error;
    }
  };

  // Replace data (for imports)
  const replaceData = async (newData) => {
    // Note: This would require a special API endpoint
    console.warn('replaceData not implemented for API mode');
    toast.error('Data import not yet implemented');
  };

  // Core Skills
  const updateCoreSkillAssessment = async (engineerId, categoryId, skillId, score) => {
    try {
      await api.saveCoreSkillAssessment({
        engineerId,
        categoryId,
        skillId,
        score
      });

      // Update local state optimistically
      const key = `${engineerId}-${categoryId}-${skillId}`;
      const newCoreSkills = { ...data.coreSkills };
      if (!newCoreSkills.assessments) {
        newCoreSkills.assessments = {};
      }
      newCoreSkills.assessments[key] = {
        score,
        lastUpdated: new Date().toISOString(),
        updatedBy: currentUser?.username || 'System'
      };
      setData({ ...data, coreSkills: newCoreSkills });

      await logAction('data_update', 'Updated core skill assessment');
    } catch (error) {
      console.error('Failed to update core skill assessment:', error);
      toast.error('Failed to save core skill assessment');
      throw error;
    }
  };

  const addCoreSkillCategory = async (name) => {
    try {
      const newCategory = await api.createCoreSkillCategory({ name });
      await loadAllData();
      await logAction('data_update', `Added core skill category: ${name}`);
      toast.success('Category added');
      return newCategory;
    } catch (error) {
      console.error('Failed to add category:', error);
      toast.error('Failed to add category');
      throw error;
    }
  };

  const addCoreSkill = async (categoryId, skillName) => {
    try {
      const newSkill = await api.createCoreSkill(categoryId, { name: skillName });
      await loadAllData();
      await logAction('data_update', `Added core skill: ${skillName}`);
      toast.success('Skill added');
      return newSkill;
    } catch (error) {
      console.error('Failed to add skill:', error);
      toast.error('Failed to add skill');
      throw error;
    }
  };

  const deleteCoreSkill = async (categoryId, skillId) => {
    try {
      await api.deleteCoreSkill(skillId);
      await loadAllData();
      await logAction('data_update', 'Deleted core skill');
      toast.success('Skill deleted');
    } catch (error) {
      console.error('Failed to delete skill:', error);
      toast.error('Failed to delete skill');
      throw error;
    }
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
    replaceData,
    updateCoreSkillAssessment,
    addCoreSkillCategory,
    addCoreSkill,
    deleteCoreSkill,
    refreshData: loadAllData
  };
};
