// API Client for Training Matrix
// Communicates with the backend SQL Server API

const API_BASE_URL = import.meta.env.VITE_API_URL || 'http://localhost:3001/api';

class ApiClient {
  constructor() {
    this.baseUrl = API_BASE_URL;
  }

  async request(endpoint, options = {}) {
    const url = `${this.baseUrl}${endpoint}`;
    const config = {
      headers: {
        'Content-Type': 'application/json',
        ...options.headers,
      },
      ...options,
    };

    try {
      const response = await fetch(url, config);

      if (!response.ok) {
        const error = await response.json().catch(() => ({ error: 'Request failed' }));
        throw new Error(error.error || `HTTP ${response.status}`);
      }

      return await response.json();
    } catch (error) {
      console.error(`API Error [${endpoint}]:`, error);
      throw error;
    }
  }

  // ==================== HEALTH ====================
  async healthCheck() {
    return this.request('/health');
  }

  // ==================== AUTH ====================
  async login(username, password) {
    return this.request('/auth/login', {
      method: 'POST',
      body: JSON.stringify({ username, password }),
    });
  }

  // ==================== USERS ====================
  async getUsers() {
    return this.request('/users');
  }

  async createUser(userData) {
    return this.request('/users', {
      method: 'POST',
      body: JSON.stringify(userData),
    });
  }

  async updateUserPassword(userId, password) {
    return this.request(`/users/${userId}/password`, {
      method: 'PUT',
      body: JSON.stringify({ password }),
    });
  }

  async deleteUser(userId) {
    return this.request(`/users/${userId}`, {
      method: 'DELETE',
    });
  }

  // ==================== ENGINEERS ====================
  async getEngineers() {
    return this.request('/engineers');
  }

  async createEngineer(engineerData) {
    return this.request('/engineers', {
      method: 'POST',
      body: JSON.stringify(engineerData),
    });
  }

  async updateEngineer(engineerId, engineerData) {
    return this.request(`/engineers/${engineerId}`, {
      method: 'PUT',
      body: JSON.stringify(engineerData),
    });
  }

  async deleteEngineer(engineerId) {
    return this.request(`/engineers/${engineerId}`, {
      method: 'DELETE',
    });
  }

  // ==================== PRODUCTION AREAS ====================
  async getProductionAreas() {
    return this.request('/production-areas');
  }

  async createProductionArea(areaData) {
    return this.request('/production-areas', {
      method: 'POST',
      body: JSON.stringify(areaData),
    });
  }

  async deleteProductionArea(areaId) {
    return this.request(`/production-areas/${areaId}`, {
      method: 'DELETE',
    });
  }

  // ==================== ASSESSMENTS ====================
  async getAssessments(filters = {}) {
    const params = new URLSearchParams(filters);
    return this.request(`/assessments?${params}`);
  }

  async saveAssessment(assessmentData) {
    return this.request('/assessments', {
      method: 'POST',
      body: JSON.stringify(assessmentData),
    });
  }

  // ==================== CORE SKILLS ====================
  async getCoreSkills() {
    return this.request('/core-skills');
  }

  async getCoreSkillAssessments(filters = {}) {
    const params = new URLSearchParams(filters);
    return this.request(`/core-skills/assessments?${params}`);
  }

  async saveCoreSkillAssessment(assessmentData) {
    return this.request('/core-skills/assessments', {
      method: 'POST',
      body: JSON.stringify(assessmentData),
    });
  }

  // ==================== CERTIFICATIONS ====================
  async getCertifications(engineerId = null) {
    const params = engineerId ? `?engineer_id=${engineerId}` : '';
    return this.request(`/certifications${params}`);
  }

  async createCertification(certData) {
    return this.request('/certifications', {
      method: 'POST',
      body: JSON.stringify(certData),
    });
  }

  async deleteCertification(certId) {
    return this.request(`/certifications/${certId}`, {
      method: 'DELETE',
    });
  }

  // ==================== SNAPSHOTS ====================
  async getSnapshots() {
    return this.request('/snapshots');
  }

  async createSnapshot(description) {
    return this.request('/snapshots', {
      method: 'POST',
      body: JSON.stringify({ description }),
    });
  }

  // ==================== AUDIT LOGS ====================
  async getAuditLogs() {
    return this.request('/audit-logs');
  }

  async logAction(userId, action, details) {
    return this.request('/audit-logs', {
      method: 'POST',
      body: JSON.stringify({ userId, action, details }),
    });
  }

  // ==================== DATA EXPORT ====================
  async exportData() {
    return this.request('/data/export');
  }

  async importData(data) {
    return this.request('/data/import', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  }
}

// Export singleton instance
const api = new ApiClient();
export default api;
