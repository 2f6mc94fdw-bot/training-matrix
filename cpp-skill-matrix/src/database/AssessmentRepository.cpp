#include "AssessmentRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

AssessmentRepository::AssessmentRepository() : lastError_("") {}
AssessmentRepository::~AssessmentRepository() {}

QList<Assessment> AssessmentRepository::findAll()
{
    QList<Assessment> assessments;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AssessmentRepository", lastError_);
        return assessments;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at "
                  "FROM assessments ORDER BY created_at DESC");

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AssessmentRepository", "findAll failed: " + lastError_);
        return assessments;
    }

    while (query.next()) {
        Assessment assessment;
        assessment.setId(query.value(0).toInt());
        assessment.setEngineerId(query.value(1).toString());
        assessment.setProductionAreaId(query.value(2).toInt());
        assessment.setMachineId(query.value(3).toInt());
        assessment.setCompetencyId(query.value(4).toInt());
        assessment.setScore(query.value(5).toInt());
        assessment.setCreatedAt(query.value(6).toDateTime());
        assessment.setUpdatedAt(query.value(7).toDateTime());
        assessments.append(assessment);
    }

    Logger::instance().debug("AssessmentRepository", QString("Found %1 assessments").arg(assessments.size()));
    return assessments;
}

QList<Assessment> AssessmentRepository::findByEngineer(const QString& engineerId)
{
    QList<Assessment> assessments;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AssessmentRepository", lastError_);
        return assessments;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at "
                  "FROM assessments WHERE engineer_id = ? ORDER BY production_area_id, machine_id, competency_id");
    query.addBindValue(engineerId);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AssessmentRepository", "findByEngineer failed: " + lastError_);
        return assessments;
    }

    while (query.next()) {
        Assessment assessment;
        assessment.setId(query.value(0).toInt());
        assessment.setEngineerId(query.value(1).toString());
        assessment.setProductionAreaId(query.value(2).toInt());
        assessment.setMachineId(query.value(3).toInt());
        assessment.setCompetencyId(query.value(4).toInt());
        assessment.setScore(query.value(5).toInt());
        assessment.setCreatedAt(query.value(6).toDateTime());
        assessment.setUpdatedAt(query.value(7).toDateTime());
        assessments.append(assessment);
    }

    Logger::instance().debug("AssessmentRepository",
        QString("Found %1 assessments for engineer %2").arg(assessments.size()).arg(engineerId));
    return assessments;
}

Assessment AssessmentRepository::findById(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AssessmentRepository", lastError_);
        return Assessment();
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at "
                  "FROM assessments WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AssessmentRepository", "findById failed: " + lastError_);
        return Assessment();
    }

    if (query.next()) {
        Assessment assessment;
        assessment.setId(query.value(0).toInt());
        assessment.setEngineerId(query.value(1).toString());
        assessment.setProductionAreaId(query.value(2).toInt());
        assessment.setMachineId(query.value(3).toInt());
        assessment.setCompetencyId(query.value(4).toInt());
        assessment.setScore(query.value(5).toInt());
        assessment.setCreatedAt(query.value(6).toDateTime());
        assessment.setUpdatedAt(query.value(7).toDateTime());

        Logger::instance().debug("AssessmentRepository", QString("Found assessment with id: %1").arg(id));
        return assessment;
    }

    Logger::instance().debug("AssessmentRepository", QString("Assessment not found with id: %1").arg(id));
    return Assessment();
}

bool AssessmentRepository::saveOrUpdate(Assessment& assessment)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AssessmentRepository", lastError_);
        return false;
    }

    // Check if assessment already exists for this combination
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM assessments "
                       "WHERE engineer_id = ? AND production_area_id = ? "
                       "AND machine_id = ? AND competency_id = ?");
    checkQuery.addBindValue(assessment.engineerId());
    checkQuery.addBindValue(assessment.productionAreaId());
    checkQuery.addBindValue(assessment.machineId());
    checkQuery.addBindValue(assessment.competencyId());

    if (!checkQuery.exec()) {
        lastError_ = checkQuery.lastError().text();
        Logger::instance().error("AssessmentRepository", "saveOrUpdate check failed: " + lastError_);
        return false;
    }

    bool exists = checkQuery.next();
    int existingId = exists ? checkQuery.value(0).toInt() : 0;

    if (exists) {
        // UPDATE existing assessment
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE assessments SET score = ?, updated_at = GETDATE() "
                           "WHERE id = ?");
        updateQuery.addBindValue(assessment.score());
        updateQuery.addBindValue(existingId);

        if (!updateQuery.exec()) {
            lastError_ = updateQuery.lastError().text();
            Logger::instance().error("AssessmentRepository", "saveOrUpdate update failed: " + lastError_);
            return false;
        }

        assessment.setId(existingId);
        Logger::instance().info("AssessmentRepository",
            QString("Updated assessment for engineer %1, competency %2").arg(assessment.engineerId()).arg(assessment.competencyId()));
        return true;
    } else {
        // INSERT new assessment
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO assessments (engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at) "
                           "VALUES (?, ?, ?, ?, ?, GETDATE(), GETDATE()); "
                           "SELECT SCOPE_IDENTITY();");
        insertQuery.addBindValue(assessment.engineerId());
        insertQuery.addBindValue(assessment.productionAreaId());
        insertQuery.addBindValue(assessment.machineId());
        insertQuery.addBindValue(assessment.competencyId());
        insertQuery.addBindValue(assessment.score());

        if (!insertQuery.exec()) {
            lastError_ = insertQuery.lastError().text();
            Logger::instance().error("AssessmentRepository", "saveOrUpdate insert failed: " + lastError_);
            return false;
        }

        // Get the generated ID
        if (insertQuery.next()) {
            int newId = insertQuery.value(0).toInt();
            assessment.setId(newId);
            Logger::instance().info("AssessmentRepository",
                QString("Created assessment for engineer %1, competency %2 (ID: %3)").arg(assessment.engineerId()).arg(assessment.competencyId()).arg(newId));
            return true;
        }

        lastError_ = "Failed to retrieve generated ID";
        Logger::instance().error("AssessmentRepository", lastError_);
        return false;
    }
}

bool AssessmentRepository::remove(int id)
{
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AssessmentRepository", lastError_);
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM assessments WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AssessmentRepository", "remove failed: " + lastError_);
        return false;
    }

    Logger::instance().info("AssessmentRepository", QString("Assessment removed: %1").arg(id));
    return true;
}
