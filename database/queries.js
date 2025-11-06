// SQL Server Query Functions
// These replace the localStorage operations with direct SQL queries
const db = require('./connection');
const bcrypt = require('bcryptjs');

// ==================== USERS ====================

async function getAllUsers() {
  const result = await db.query(`
    SELECT id, username, role, engineer_id
    FROM users
    ORDER BY username
  `);
  return result.recordset;
}

async function getUserByUsername(username) {
  const result = await db.query(`
    SELECT *
    FROM users
    WHERE username = @param0
  `, [username]);
  return result.recordset[0];
}

async function createUser(user) {
  const hashedPassword = bcrypt.hashSync(user.password, 10);

  await db.query(`
    INSERT INTO users (id, username, password, role, engineer_id)
    VALUES (@param0, @param1, @param2, @param3, @param4)
  `, [user.id, user.username, hashedPassword, user.role, user.engineerId || null]);

  return { success: true };
}

async function updateUserPassword(userId, newPassword) {
  const hashedPassword = bcrypt.hashSync(newPassword, 10);

  await db.query(`
    UPDATE users
    SET password = @param0, updated_at = GETDATE()
    WHERE id = @param1
  `, [hashedPassword, userId]);

  return { success: true };
}

async function deleteUser(userId) {
  await db.query(`
    DELETE FROM users WHERE id = @param0
  `, [userId]);

  return { success: true };
}

async function verifyUserPassword(username, password) {
  const user = await getUserByUsername(username);
  if (!user) return null;

  const valid = bcrypt.compareSync(password, user.password);
  if (!valid) return null;

  // Remove password before returning
  delete user.password;
  return user;
}

// ==================== ENGINEERS ====================

async function getAllEngineers() {
  const result = await db.query(`
    SELECT *
    FROM engineers
    ORDER BY name
  `);
  return result.recordset;
}

async function getEngineerById(engineerId) {
  const result = await db.query(`
    SELECT *
    FROM engineers
    WHERE id = @param0
  `, [engineerId]);
  return result.recordset[0];
}

async function createEngineer(engineer) {
  await db.query(`
    INSERT INTO engineers (id, name, shift)
    VALUES (@param0, @param1, @param2)
  `, [engineer.id, engineer.name, engineer.shift]);

  return engineer;
}

async function updateEngineer(engineerId, updates) {
  await db.query(`
    UPDATE engineers
    SET name = @param0, shift = @param1, updated_at = GETDATE()
    WHERE id = @param2
  `, [updates.name, updates.shift, engineerId]);

  return { success: true };
}

async function deleteEngineer(engineerId) {
  await db.query(`
    DELETE FROM engineers WHERE id = @param0
  `, [engineerId]);

  return { success: true };
}

// ==================== PRODUCTION AREAS ====================

async function getAllProductionAreas() {
  const areasResult = await db.query(`
    SELECT *
    FROM production_areas
    ORDER BY id
  `);

  const areas = areasResult.recordset;

  // Get machines for each area
  for (const area of areas) {
    const machinesResult = await db.query(`
      SELECT *
      FROM machines
      WHERE production_area_id = @param0
      ORDER BY id
    `, [area.id]);

    area.machines = machinesResult.recordset;

    // Get competencies for each machine
    for (const machine of area.machines) {
      const competenciesResult = await db.query(`
        SELECT *
        FROM competencies
        WHERE machine_id = @param0
        ORDER BY id
      `, [machine.id]);

      machine.competencies = competenciesResult.recordset;
    }
  }

  return areas;
}

async function createProductionArea(area) {
  const result = await db.query(`
    INSERT INTO production_areas (name)
    OUTPUT INSERTED.id
    VALUES (@param0)
  `, [area.name]);

  const areaId = result.recordset[0].id;

  // Insert machines if provided
  if (area.machines && area.machines.length > 0) {
    for (const machine of area.machines) {
      const machineResult = await db.query(`
        INSERT INTO machines (production_area_id, name, importance)
        OUTPUT INSERTED.id
        VALUES (@param0, @param1, @param2)
      `, [areaId, machine.name, machine.importance || 1]);

      const machineId = machineResult.recordset[0].id;

      // Insert competencies if provided
      if (machine.competencies && machine.competencies.length > 0) {
        for (const comp of machine.competencies) {
          await db.query(`
            INSERT INTO competencies (machine_id, name, max_score)
            VALUES (@param0, @param1, @param2)
          `, [machineId, comp.name, comp.maxScore || 3]);
        }
      }
    }
  }

  return { id: areaId, success: true };
}

async function updateProductionArea(areaId, updates) {
  await db.query(`
    UPDATE production_areas
    SET name = @param0, updated_at = GETDATE()
    WHERE id = @param1
  `, [updates.name, areaId]);

  return { success: true };
}

async function deleteProductionArea(areaId) {
  await db.query(`
    DELETE FROM production_areas WHERE id = @param0
  `, [areaId]);

  return { success: true };
}

// ==================== ASSESSMENTS ====================

async function getAllAssessments() {
  const result = await db.query(`
    SELECT *
    FROM assessments
  `);

  // Convert to object format: { "engineerId-areaId-machineId-compId": score }
  const assessments = {};
  result.recordset.forEach(row => {
    const key = `${row.engineer_id}-${row.production_area_id}-${row.machine_id}-${row.competency_id}`;
    assessments[key] = row.score;
  });

  return assessments;
}

async function getAssessmentScore(engineerId, areaId, machineId, competencyId) {
  const result = await db.query(`
    SELECT score
    FROM assessments
    WHERE engineer_id = @param0
      AND production_area_id = @param1
      AND machine_id = @param2
      AND competency_id = @param3
  `, [engineerId, areaId, machineId, competencyId]);

  return result.recordset[0]?.score || 0;
}

async function saveAssessment(engineerId, areaId, machineId, competencyId, score) {
  await db.query(`
    IF EXISTS (
      SELECT 1 FROM assessments
      WHERE engineer_id = @param0
        AND production_area_id = @param1
        AND machine_id = @param2
        AND competency_id = @param3
    )
    BEGIN
      UPDATE assessments
      SET score = @param4, updated_at = GETDATE()
      WHERE engineer_id = @param0
        AND production_area_id = @param1
        AND machine_id = @param2
        AND competency_id = @param3
    END
    ELSE
    BEGIN
      INSERT INTO assessments (engineer_id, production_area_id, machine_id, competency_id, score)
      VALUES (@param0, @param1, @param2, @param3, @param4)
    END
  `, [engineerId, areaId, machineId, competencyId, score]);

  return { success: true };
}

async function deleteEngineerAssessments(engineerId) {
  await db.query(`
    DELETE FROM assessments WHERE engineer_id = @param0
  `, [engineerId]);

  return { success: true };
}

// ==================== CORE SKILLS ====================

async function getCoreSkills() {
  const categoriesResult = await db.query(`
    SELECT *
    FROM core_skill_categories
    ORDER BY id
  `);

  const categories = categoriesResult.recordset;

  for (const category of categories) {
    const skillsResult = await db.query(`
      SELECT *
      FROM core_skills
      WHERE category_id = @param0
      ORDER BY id
    `, [category.id]);

    category.skills = skillsResult.recordset;
  }

  return { categories };
}

async function getCoreSkillAssessments() {
  const result = await db.query(`
    SELECT *
    FROM core_skill_assessments
  `);

  // Convert to object format: { "engineerId-categoryId-skillId": score }
  const assessments = {};
  result.recordset.forEach(row => {
    const key = `${row.engineer_id}-${row.category_id}-${row.skill_id}`;
    assessments[key] = row.score;
  });

  return assessments;
}

async function saveCoreSkillAssessment(engineerId, categoryId, skillId, score) {
  await db.query(`
    IF EXISTS (
      SELECT 1 FROM core_skill_assessments
      WHERE engineer_id = @param0
        AND category_id = @param1
        AND skill_id = @param2
    )
    BEGIN
      UPDATE core_skill_assessments
      SET score = @param3, updated_at = GETDATE()
      WHERE engineer_id = @param0
        AND category_id = @param1
        AND skill_id = @param2
    END
    ELSE
    BEGIN
      INSERT INTO core_skill_assessments (engineer_id, category_id, skill_id, score)
      VALUES (@param0, @param1, @param2, @param3)
    END
  `, [engineerId, categoryId, skillId, score]);

  return { success: true };
}

// ==================== CERTIFICATIONS ====================

async function getAllCertifications() {
  const result = await db.query(`
    SELECT *
    FROM certifications
    ORDER BY date_earned DESC
  `);
  return result.recordset;
}

async function getEngineerCertifications(engineerId) {
  const result = await db.query(`
    SELECT *
    FROM certifications
    WHERE engineer_id = @param0
    ORDER BY date_earned DESC
  `, [engineerId]);
  return result.recordset;
}

async function createCertification(cert) {
  const result = await db.query(`
    INSERT INTO certifications (engineer_id, name, date_earned, expiry_date)
    OUTPUT INSERTED.id
    VALUES (@param0, @param1, @param2, @param3)
  `, [cert.engineerId, cert.name, cert.dateEarned, cert.expiryDate || null]);

  return { id: result.recordset[0].id, success: true };
}

async function deleteCertification(certId) {
  await db.query(`
    DELETE FROM certifications WHERE id = @param0
  `, [certId]);

  return { success: true };
}

// ==================== SNAPSHOTS ====================

async function getAllSnapshots() {
  const result = await db.query(`
    SELECT TOP 50 *
    FROM snapshots
    ORDER BY timestamp DESC
  `);

  // Parse JSON data
  return result.recordset.map(row => ({
    ...row,
    data: JSON.parse(row.data)
  }));
}

async function createSnapshot(snapshot) {
  await db.query(`
    INSERT INTO snapshots (id, description, timestamp, data)
    VALUES (@param0, @param1, @param2, @param3)
  `, [snapshot.id, snapshot.description, snapshot.timestamp, JSON.stringify(snapshot.data)]);

  return { success: true };
}

// ==================== AUDIT LOGS ====================

async function getAllAuditLogs() {
  const result = await db.query(`
    SELECT TOP 1000 *
    FROM audit_logs
    ORDER BY timestamp DESC
  `);
  return result.recordset;
}

async function createAuditLog(log) {
  await db.query(`
    INSERT INTO audit_logs (id, timestamp, user_id, action, details)
    VALUES (@param0, @param1, @param2, @param3, @param4)
  `, [log.id, log.timestamp, log.user, log.action, log.details]);

  return { success: true };
}

// ==================== BULK OPERATIONS ====================

async function getAllData() {
  const [
    engineers,
    users,
    productionAreas,
    assessments,
    coreSkills,
    coreSkillAssessments,
    certifications,
    snapshots
  ] = await Promise.all([
    getAllEngineers(),
    getAllUsers(),
    getAllProductionAreas(),
    getAllAssessments(),
    getCoreSkills(),
    getCoreSkillAssessments(),
    getAllCertifications(),
    getAllSnapshots()
  ]);

  return {
    engineers,
    users,
    productionAreas,
    assessments,
    coreSkills,
    coreSkillAssessments: coreSkillAssessments,
    certifications,
    snapshots,
    version: '2.0.0'
  };
}

// Export all functions
module.exports = {
  // Users
  getAllUsers,
  getUserByUsername,
  createUser,
  updateUserPassword,
  deleteUser,
  verifyUserPassword,

  // Engineers
  getAllEngineers,
  getEngineerById,
  createEngineer,
  updateEngineer,
  deleteEngineer,

  // Production Areas
  getAllProductionAreas,
  createProductionArea,
  updateProductionArea,
  deleteProductionArea,

  // Assessments
  getAllAssessments,
  getAssessmentScore,
  saveAssessment,
  deleteEngineerAssessments,

  // Core Skills
  getCoreSkills,
  getCoreSkillAssessments,
  saveCoreSkillAssessment,

  // Certifications
  getAllCertifications,
  getEngineerCertifications,
  createCertification,
  deleteCertification,

  // Snapshots
  getAllSnapshots,
  createSnapshot,

  // Audit Logs
  getAllAuditLogs,
  createAuditLog,

  // Bulk
  getAllData
};
