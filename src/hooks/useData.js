import { useState, useEffect } from 'react';
import api from '../utils/api';
import { getDefaultData } from '../utils/storage';

export const useData = (currentUser) => {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);

  // Load all data from API on mount
  useEffect(() => {
    loadDataFromAPI();
  }, []);

  const loadDataFromAPI = async () => {
    try {
      setLoading(true);

      // Load production areas with nested structure
      const productionAreas = await api.getProductionAreas();

      // Load engineers
      const engineers = await api.getEngineers();

      // Load users (we'll keep current users from session for now)
      const users = await api.getUsers().catch(() => []);

      // Load core skills
      const coreSkills = await api.getCoreSkills().catch(() => ({ categories: [] }));

      // Load assessments
      const assessments = await api.getAssessments().catch(() => []);

      // Load core skill assessments
      const coreSkillAssessments = await api.getCoreSkillAssessments().catch(() => []);

      // Load certifications
      const certifications = await api.getCertifications().catch(() => []);

      // Load snapshots
      const snapshots = await api.getSnapshots().catch(() => []);

      // Transform data to match frontend structure
      const transformedData = {
        productionAreas: productionAreas || [],
        engineers: engineers || [],
        users: users.length > 0 ? users : getDefaultData().users,
        assessments: transformAssessments(assessments),
        certifications: certifications || [],
        snapshots: snapshots || [],
        coreSkills: {
          categories: coreSkills.categories || getDefaultData().coreSkills.categories,
          assessments: transformCoreSkillAssessments(coreSkillAssessments)
        }
      };

      setData(transformedData);
      setLoading(false);
    } catch (error) {
      console.error('Error loading data from API:', error);
      // Fallback to default data
      setData(getDefaultData());
      setLoading(false);
    }
  };

  // Transform assessments from array to object keyed by composite key
  const transformAssessments = (assessments) => {
    const result = {};
    assessments.forEach(assessment => {
      const key = `${assessment.engineer_id}-${assessment.production_area_id}-${assessment.machine_id}-${assessment.competency_id}`;
      result[key] = {
        score: assessment.score,
        lastUpdated: assessment.updated_at,
        updatedBy: 'System',
        history: []
      };
    });
    return result;
  };

  // Transform core skill assessments
  const transformCoreSkillAssessments = (assessments) => {
    const result = {};
    assessments.forEach(assessment => {
      const key = `${assessment.engineer_id}-${assessment.category_id}-${assessment.skill_id}`;
      result[key] = {
        score: assessment.score,
        lastUpdated: assessment.updated_at,
        updatedBy: 'System',
        history: []
      };
    });
    return result;
  };

  // Refresh data from API
  const refreshData = async () => {
    await loadDataFromAPI();
  };

  // Log action helper
  const logAction = async (action, details) => {
    if (currentUser) {
      await api.logAction(currentUser.id, action, details).catch(console.error);
    }
  };

  // Production Areas
  const addProductionArea = async (area) => {
    try {
      const newArea = await api.createProductionArea({
        name: area.name
      });

      await logAction('add_production_area', `Added production area: ${area.name}`);
      await refreshData();

      return newArea;
    } catch (error) {
      console.error('Error adding production area:', error);
      throw error;
    }
  };

  const updateProductionArea = async (areaId, updates) => {
    // Note: API doesn't have update endpoint yet, would need to add
    console.warn('Update production area not yet implemented in API');
    await logAction('update_production_area', `Updated production area: ${areaId}`);
  };

  const deleteProductionArea = async (areaId) => {
    try {
      await api.deleteProductionArea(areaId);
      await logAction('delete_production_area', `Deleted production area: ${areaId}`);
      await refreshData();
    } catch (error) {
      console.error('Error deleting production area:', error);
      throw error;
    }
  };

  // Machines (handled by production area endpoint)
  const addMachine = async (areaId, machine) => {
    console.warn('Add machine not yet fully implemented - needs API endpoint');
    await refreshData();
  };

  const updateMachine = async (areaId, machineId, updates) => {
    console.warn('Update machine not yet implemented');
  };

  const deleteMachine = async (areaId, machineId) => {
    console.warn('Delete machine not yet implemented');
  };

  // Competencies
  const addCompetency = async (areaId, machineId, competency) => {
    console.warn('Add competency not yet implemented');
  };

  const updateCompetency = async (areaId, machineId, compId, updates) => {
    console.warn('Update competency not yet implemented');
  };

  const deleteCompetency = async (areaId, machineId, compId) => {
    console.warn('Delete competency not yet implemented');
  };

  // Engineers
  const addEngineer = async (engineer) => {
    try {
      const newEngineer = await api.createEngineer({
        id: `eng_${Date.now()}`,
        name: engineer.name,
        shift: engineer.shift
      });

      await logAction('add_engineer', `Added engineer: ${engineer.name}`);
      await refreshData();

      return {
        newEngineer,
        username: newEngineer.name || engineer.name,
        userCreated: false
      };
    } catch (error) {
      console.error('Error adding engineer:', error);
      throw error;
    }
  };

  const updateEngineer = async (engineerId, updates) => {
    try {
      await api.updateEngineer(engineerId, updates);
      await logAction('update_engineer', `Updated engineer: ${engineerId}`);
      await refreshData();
    } catch (error) {
      console.error('Error updating engineer:', error);
      throw error;
    }
  };

  const deleteEngineer = async (engineerId) => {
    try {
      await api.deleteEngineer(engineerId);
      await logAction('delete_engineer', `Deleted engineer: ${engineerId}`);
      await refreshData();
    } catch (error) {
      console.error('Error deleting engineer:', error);
      throw error;
    }
  };

  // Assessments
  const updateAssessment = async (engineerId, areaId, machineId, compId, score) => {
    try {
      await api.saveAssessment({
        engineer_id: engineerId,
        production_area_id: areaId,
        machine_id: machineId,
        competency_id: compId,
        score: score
      });

      await logAction('update_assessment', `Updated assessment for engineer ${engineerId}`);
      await refreshData();
    } catch (error) {
      console.error('Error updating assessment:', error);
      throw error;
    }
  };

  const bulkUpdateAssessments = async (updates) => {
    try {
      for (const update of updates) {
        await api.saveAssessment({
          engineer_id: update.engineerId,
          production_area_id: update.areaId,
          machine_id: update.machineId,
          competency_id: update.compId,
          score: update.score
        });
      }

      await logAction('bulk_update_assessments', `Bulk updated ${updates.length} assessments`);
      await refreshData();
    } catch (error) {
      console.error('Error bulk updating assessments:', error);
      throw error;
    }
  };

  // Users
  const addUser = async (user) => {
    try {
      const newUser = await api.createUser({
        id: `user_${Date.now()}`,
        username: user.username,
        password: user.password,
        role: user.role,
        engineerId: user.engineerId || null
      });

      await logAction('add_user', `Added user: ${user.username}`);
      await refreshData();

      return newUser;
    } catch (error) {
      console.error('Error adding user:', error);
      throw error;
    }
  };

  const deleteUser = async (userId) => {
    try {
      await api.deleteUser(userId);
      await logAction('delete_user', `Deleted user: ${userId}`);
      await refreshData();
    } catch (error) {
      console.error('Error deleting user:', error);
      throw error;
    }
  };

  const resetPassword = async (userId, newPassword) => {
    try {
      await api.updateUserPassword(userId, newPassword);
      await logAction('reset_password', `Reset password for user: ${userId}`);
    } catch (error) {
      console.error('Error resetting password:', error);
      throw error;
    }
  };

  // Certifications
  const addCertification = async (cert) => {
    try {
      const newCert = await api.createCertification(cert);
      await logAction('add_certification', `Added certification: ${cert.name}`);
      await refreshData();
      return newCert;
    } catch (error) {
      console.error('Error adding certification:', error);
      throw error;
    }
  };

  const updateCertification = async (certId, updates) => {
    console.warn('Update certification not yet implemented');
  };

  const deleteCertification = async (certId) => {
    try {
      await api.deleteCertification(certId);
      await logAction('delete_certification', `Deleted certification: ${certId}`);
      await refreshData();
    } catch (error) {
      console.error('Error deleting certification:', error);
      throw error;
    }
  };

  // Snapshots
  const takeSnapshot = async (description) => {
    try {
      await api.createSnapshot(description);
      await logAction('create_snapshot', `Created snapshot: ${description}`);
      await refreshData();
    } catch (error) {
      console.error('Error creating snapshot:', error);
      throw error;
    }
  };

  // Replace data (for import)
  const replaceData = async (newData) => {
    try {
      await api.importData(newData);
      await logAction('import_data', 'Imported data');
      await refreshData();
    } catch (error) {
      console.error('Error importing data:', error);
      throw error;
    }
  };

  // Core Skills
  const updateCoreSkillAssessment = async (engineerId, categoryId, skillId, score) => {
    try {
      await api.saveCoreSkillAssessment({
        engineer_id: engineerId,
        category_id: categoryId,
        skill_id: skillId,
        score: score
      });

      await logAction('update_core_skill', `Updated core skill assessment`);
      await refreshData();
    } catch (error) {
      console.error('Error updating core skill assessment:', error);
      throw error;
    }
  };

  const addCoreSkillCategory = async (name) => {
    console.warn('Add core skill category not yet implemented');
  };

  const addCoreSkill = async (categoryId, skillName) => {
    console.warn('Add core skill not yet implemented');
  };

  const deleteCoreSkill = async (categoryId, skillId) => {
    console.warn('Delete core skill not yet implemented');
  };

  return {
    data,
    loading,
    refreshData,
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
    deleteCoreSkill
  };
};
