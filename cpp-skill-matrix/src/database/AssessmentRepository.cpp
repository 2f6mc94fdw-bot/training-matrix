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
    query.setForwardOnly(true);
    query.prepare("SELECT id, engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at "
                  "FROM assessments");

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

QList<Assessment> AssessmentRepository::findByMaxScore(int maxScore)
{
    QList<Assessment> assessments;

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        Logger::instance().error("AssessmentRepository", lastError_);
        return assessments;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(
        "SELECT id, engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at "
        "FROM assessments "
        "WHERE score <= ? "
        "ORDER BY updated_at DESC");
    query.addBindValue(maxScore);

    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AssessmentRepository", "findByMaxScore failed: " + lastError_);
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
        QString("Found %1 assessments with score <= %2").arg(assessments.size()).arg(maxScore));
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

    // Atomic upsert to avoid race conditions under concurrent writes.
    if (!db.transaction()) {
        lastError_ = db.lastError().text();
        Logger::instance().error("AssessmentRepository", "saveOrUpdate transaction begin failed: " + lastError_);
        return false;
    }

    QSqlQuery mergeQuery(db);
    mergeQuery.prepare(R"(
        MERGE assessments WITH (HOLDLOCK) AS target
        USING (SELECT ? AS engineer_id, ? AS production_area_id, ? AS machine_id, ? AS competency_id, ? AS score) AS source
            ON target.engineer_id = source.engineer_id
           AND target.production_area_id = source.production_area_id
           AND target.machine_id = source.machine_id
           AND target.competency_id = source.competency_id
        WHEN MATCHED THEN
            UPDATE SET score = source.score, updated_at = GETDATE()
        WHEN NOT MATCHED THEN
            INSERT (engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at)
            VALUES (source.engineer_id, source.production_area_id, source.machine_id, source.competency_id, source.score, GETDATE(), GETDATE())
        OUTPUT inserted.id;
    )");
    mergeQuery.addBindValue(assessment.engineerId());
    mergeQuery.addBindValue(assessment.productionAreaId());
    mergeQuery.addBindValue(assessment.machineId());
    mergeQuery.addBindValue(assessment.competencyId());
    mergeQuery.addBindValue(assessment.score());

    if (!mergeQuery.exec()) {
        lastError_ = mergeQuery.lastError().text();
        db.rollback();
        Logger::instance().error("AssessmentRepository", "saveOrUpdate merge failed: " + lastError_);
        return false;
    }

    if (!mergeQuery.next()) {
        lastError_ = "saveOrUpdate merge returned no id";
        db.rollback();
        Logger::instance().error("AssessmentRepository", lastError_);
        return false;
    }

    assessment.setId(mergeQuery.value(0).toInt());

    if (!db.commit()) {
        lastError_ = db.lastError().text();
        db.rollback();
        Logger::instance().error("AssessmentRepository", "saveOrUpdate commit failed: " + lastError_);
        return false;
    }

    Logger::instance().info("AssessmentRepository",
        QString("Upserted assessment for engineer %1, competency %2 (ID: %3)")
            .arg(assessment.engineerId())
            .arg(assessment.competencyId())
            .arg(assessment.id()));
    return true;
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
