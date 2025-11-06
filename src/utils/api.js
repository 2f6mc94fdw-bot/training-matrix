// API Client for Training Matrix Backend
// Communicates with Express server which connects to SQL Server

const API_BASE_URL = import.meta.env.VITE_API_URL || 'http://localhost:3001/api';

// Helper function to handle API responses
async function handleResponse(response) {
  if (!response.ok) {
    const error = await response.json().catch(() => ({ error: 'Request failed' }));
    throw new Error(error.error || `HTTP ${response.status}: ${response.statusText}`);
  }
  return response.json();
}

// Helper function to make API calls
async function apiCall(endpoint, options = {}) {
  try {
    const response = await fetch(`${API_BASE_URL}${endpoint}`, {
      ...options,
      headers: {
        'Content-Type': 'application/json',
        ...options.headers
      }
    });
    return handleResponse(response);
  } catch (error) {
    console.error(`API Error [${endpoint}]:`, error);
    throw error;
  }
}

// ============================================================================
// AUTHENTICATION
// ============================================================================

export async function login(username, password) {
  return apiCall('/auth/login', {
    method: 'POST',
    body: JSON.stringify({ username, password })
  });
}

// ============================================================================
// DATA - Get All
// ============================================================================

export async function getAllData() {
  return apiCall('/data');
}

// ============================================================================
// USERS
// ============================================================================

export async function getUsers() {
  return apiCall('/users');
}

export async function createUser(userData) {
  return apiCall('/users', {
    method: 'POST',
    body: JSON.stringify(userData)
  });
}

export async function deleteUser(userId) {
  return apiCall(`/users/${userId}`, {
    method: 'DELETE'
  });
}

export async function resetPassword(userId, newPassword) {
  return apiCall(`/users/${userId}/password`, {
    method: 'PUT',
    body: JSON.stringify({ password: newPassword })
  });
}

// ============================================================================
// ENGINEERS
// ============================================================================

export async function getEngineers() {
  return apiCall('/engineers');
}

export async function createEngineer(engineerData) {
  return apiCall('/engineers', {
    method: 'POST',
    body: JSON.stringify(engineerData)
  });
}

export async function updateEngineer(engineerId, updates) {
  return apiCall(`/engineers/${engineerId}`, {
    method: 'PUT',
    body: JSON.stringify(updates)
  });
}

export async function deleteEngineer(engineerId) {
  return apiCall(`/engineers/${engineerId}`, {
    method: 'DELETE'
  });
}

// ============================================================================
// PRODUCTION AREAS
// ============================================================================

export async function getProductionAreas() {
  return apiCall('/production-areas');
}

export async function createProductionArea(areaData) {
  return apiCall('/production-areas', {
    method: 'POST',
    body: JSON.stringify(areaData)
  });
}

export async function updateProductionArea(areaId, updates) {
  return apiCall(`/production-areas/${areaId}`, {
    method: 'PUT',
    body: JSON.stringify(updates)
  });
}

export async function deleteProductionArea(areaId) {
  return apiCall(`/production-areas/${areaId}`, {
    method: 'DELETE'
  });
}

// ============================================================================
// MACHINES
// ============================================================================

export async function createMachine(areaId, machineData) {
  return apiCall(`/production-areas/${areaId}/machines`, {
    method: 'POST',
    body: JSON.stringify(machineData)
  });
}

export async function updateMachine(areaId, machineId, updates) {
  return apiCall(`/production-areas/${areaId}/machines/${machineId}`, {
    method: 'PUT',
    body: JSON.stringify(updates)
  });
}

export async function deleteMachine(areaId, machineId) {
  return apiCall(`/production-areas/${areaId}/machines/${machineId}`, {
    method: 'DELETE'
  });
}

// ============================================================================
// COMPETENCIES
// ============================================================================

export async function createCompetency(areaId, machineId, competencyData) {
  return apiCall(`/production-areas/${areaId}/machines/${machineId}/competencies`, {
    method: 'POST',
    body: JSON.stringify(competencyData)
  });
}

export async function updateCompetency(areaId, machineId, compId, updates) {
  return apiCall(`/production-areas/${areaId}/machines/${machineId}/competencies/${compId}`, {
    method: 'PUT',
    body: JSON.stringify(updates)
  });
}

export async function deleteCompetency(areaId, machineId, compId) {
  return apiCall(`/production-areas/${areaId}/machines/${machineId}/competencies/${compId}`, {
    method: 'DELETE'
  });
}

// ============================================================================
// ASSESSMENTS
// ============================================================================

export async function getAssessments() {
  return apiCall('/assessments');
}

export async function saveAssessment(assessmentData) {
  return apiCall('/assessments', {
    method: 'POST',
    body: JSON.stringify(assessmentData)
  });
}

export async function bulkUpdateAssessments(updates) {
  return apiCall('/assessments/bulk', {
    method: 'POST',
    body: JSON.stringify({ updates })
  });
}

// ============================================================================
// CORE SKILLS
// ============================================================================

export async function getCoreSkills() {
  return apiCall('/core-skills');
}

export async function getCoreSkillAssessments() {
  return apiCall('/core-skills/assessments');
}

export async function saveCoreSkillAssessment(assessmentData) {
  return apiCall('/core-skills/assessments', {
    method: 'POST',
    body: JSON.stringify(assessmentData)
  });
}

export async function createCoreSkillCategory(categoryData) {
  return apiCall('/core-skills/categories', {
    method: 'POST',
    body: JSON.stringify(categoryData)
  });
}

export async function createCoreSkill(categoryId, skillData) {
  return apiCall(`/core-skills/categories/${categoryId}/skills`, {
    method: 'POST',
    body: JSON.stringify(skillData)
  });
}

export async function deleteCoreSkill(skillId) {
  return apiCall(`/core-skills/${skillId}`, {
    method: 'DELETE'
  });
}

// ============================================================================
// CERTIFICATIONS
// ============================================================================

export async function getCertifications() {
  return apiCall('/certifications');
}

export async function createCertification(certData) {
  return apiCall('/certifications', {
    method: 'POST',
    body: JSON.stringify(certData)
  });
}

export async function deleteCertification(certId) {
  return apiCall(`/certifications/${certId}`, {
    method: 'DELETE'
  });
}

// ============================================================================
// SNAPSHOTS
// ============================================================================

export async function getSnapshots() {
  return apiCall('/snapshots');
}

export async function createSnapshot(description) {
  return apiCall('/snapshots', {
    method: 'POST',
    body: JSON.stringify({ description })
  });
}

// ============================================================================
// AUDIT LOGS
// ============================================================================

export async function getAuditLogs() {
  return apiCall('/audit-logs');
}

export async function logAction(user, action, details) {
  return apiCall('/audit-logs', {
    method: 'POST',
    body: JSON.stringify({ user, action, details })
  });
}

// ============================================================================
// HEALTH CHECK
// ============================================================================

export async function checkHealth() {
  return apiCall('/health');
}
