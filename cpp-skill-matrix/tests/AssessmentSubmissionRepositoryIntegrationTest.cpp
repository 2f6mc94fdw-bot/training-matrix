#include "database/AssessmentSubmissionRepository.h"
#include "database/DatabaseManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QProcessEnvironment>
#include <QSqlError>
#include <QSqlQuery>

namespace {
bool execSql(QSqlDatabase& db, const QString& sql, const QList<QVariant>& bindings = {})
{
    QSqlQuery query(db);
    query.prepare(sql);
    for (const QVariant& binding : bindings) {
        query.addBindValue(binding);
    }
    if (!query.exec()) {
        qCritical().noquote() << query.lastError().text() << "SQL:" << sql;
        return false;
    }
    return true;
}

int insertedId(QSqlDatabase& db, const QString& sql, const QList<QVariant>& bindings)
{
    QSqlQuery query(db);
    query.prepare(sql);
    for (const QVariant& binding : bindings) {
        query.addBindValue(binding);
    }
    if (!query.exec() || !query.next()) {
        qCritical().noquote() << query.lastError().text() << "SQL:" << sql;
        return 0;
    }
    return query.value(0).toInt();
}

int scalarInt(QSqlDatabase& db, const QString& sql, const QList<QVariant>& bindings = {})
{
    QSqlQuery query(db);
    query.prepare(sql);
    for (const QVariant& binding : bindings) {
        query.addBindValue(binding);
    }
    if (!query.exec() || !query.next()) {
        qCritical().noquote() << query.lastError().text() << "SQL:" << sql;
        return -9999;
    }
    return query.value(0).toInt();
}

bool require(bool condition, const QString& message)
{
    if (!condition) {
        qCritical().noquote() << "FAILED:" << message;
        return false;
    }
    qInfo().noquote() << "PASS:" << message;
    return true;
}

AssessmentSubmission pendingFor(
    AssessmentSubmissionRepository& repository,
    const QString& engineerId,
    const QString& type)
{
    const QList<AssessmentSubmission> pending = repository.findPendingByEngineer(engineerId, type);
    return pending.isEmpty() ? AssessmentSubmission() : pending.first();
}
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString server = env.value("APTITUDE_TEST_DB_SERVER", "localhost");
    const QString database = env.value("APTITUDE_TEST_DB_NAME");
    const QString user = env.value("APTITUDE_TEST_DB_USER");
    const QString password = env.value("APTITUDE_TEST_DB_PASSWORD");
    const int port = env.value("APTITUDE_TEST_DB_PORT", "1433").toInt();

    if (database.isEmpty() || user.isEmpty() || password.isEmpty()) {
        qCritical() << "Set APTITUDE_TEST_DB_NAME, APTITUDE_TEST_DB_USER, and APTITUDE_TEST_DB_PASSWORD.";
        return 2;
    }

    DatabaseManager& manager = DatabaseManager::instance();
    if (!manager.connect(server, database, user, password, port, true, true)) {
        qCritical().noquote() << manager.lastError();
        return 3;
    }
    QSqlDatabase& db = manager.database();

    const QString engineerId = "phase2-test-engineer";
    const QString userId = "phase2-test-user";
    const QString categoryId = "phase2-test-category";
    const QString skillId = "phase2-test-skill";

    if (!execSql(db,
            "INSERT INTO dbo.engineers (id,name,shift) VALUES (?,?,?)",
            {engineerId, "Phase 2 Test Engineer", "Test Shift"}) ||
        !execSql(db,
            "INSERT INTO dbo.users (id,username,password,role,engineer_id) VALUES (?,?,?,?,?)",
            {userId, "phase2-test-user", "not-used", "engineer", engineerId})) {
        return 4;
    }

    const int areaId = insertedId(db,
        "INSERT INTO dbo.production_areas (name) OUTPUT INSERTED.id VALUES (?)",
        {"Phase 2 Test Area"});
    const int machineId = insertedId(db,
        "INSERT INTO dbo.machines (production_area_id,name) OUTPUT INSERTED.id VALUES (?,?)",
        {areaId, "Phase 2 Test Machine"});
    const int competencyId = insertedId(db,
        "INSERT INTO dbo.competencies (machine_id,name) OUTPUT INSERTED.id VALUES (?,?)",
        {machineId, "Phase 2 Test Competency"});
    if (areaId == 0 || machineId == 0 || competencyId == 0 ||
        !execSql(db,
            "INSERT INTO dbo.core_skill_categories (id,name,discipline) VALUES (?,?,?)",
            {categoryId, "Phase 2 Test Category", "Software"}) ||
        !execSql(db,
            "INSERT INTO dbo.core_skills (id,category_id,name) VALUES (?,?,?)",
            {skillId, categoryId, "Phase 2 Test Skill"})) {
        return 5;
    }

    AssessmentSubmissionRepository repository;

    if (!require(repository.submitProduction(
            engineerId, areaId, machineId, competencyId, 2, userId, "Phase 2 Test Engineer"),
            "production proposal is submitted") ||
        !require(scalarInt(db,
            "SELECT COUNT(*) FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 0,
            "submission does not change the official production score")) {
        qCritical().noquote() << repository.lastError();
        return 6;
    }

    AssessmentSubmission production = pendingFor(repository, engineerId, "production");
    if (!require(!production.id.isEmpty() && !production.rowVersion.isEmpty(),
            "pending production submission includes a concurrency token") ||
        !require(!repository.decide(
            production.id, production.rowVersion, "approved", QString(), QString(), QString()),
            "anonymous manager decision is rejected") ||
        !require(repository.decide(
            production.id, production.rowVersion, "rejected", "More supervised evidence required",
            "admin", "admin"),
            "manager can reject with a reason") ||
        !require(scalarInt(db,
            "SELECT COUNT(*) FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 0,
            "rejection leaves the official score unchanged")) {
        qCritical().noquote() << repository.lastError();
        return 7;
    }

    if (!require(repository.submitProduction(
            engineerId, areaId, machineId, competencyId, 2, userId, "Phase 2 Test Engineer"),
            "engineer can resubmit after rejection")) {
        qCritical().noquote() << repository.lastError();
        return 8;
    }
    production = pendingFor(repository, engineerId, "production");
    if (!require(repository.decide(
            production.id, production.rowVersion, "approved", "Evidence reviewed",
            "admin", "admin"),
            "manager can approve the production proposal") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 2,
            "approval updates the official production score")) {
        qCritical().noquote() << repository.lastError();
        return 9;
    }

    if (!require(repository.submitProduction(
            engineerId, areaId, machineId, competencyId, 3, userId, "Phase 2 Test Engineer"),
            "second production proposal is submitted")) {
        return 10;
    }
    production = pendingFor(repository, engineerId, "production");
    const QByteArray staleVersion = production.rowVersion;
    if (!execSql(db,
            "UPDATE dbo.assessment_submissions SET submission_note='concurrent change' WHERE id=?",
            {production.id}) ||
        !require(!repository.decide(
            production.id, staleVersion, "approved", QString(), "admin", "admin"),
            "stale manager decision is rejected") ||
        !require(repository.lastError().startsWith("Conflict:"),
            "stale decision reports a conflict") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 2,
            "conflict does not overwrite the official score")) {
        return 11;
    }

    if (!require(repository.submitProduction(
            engineerId, areaId, machineId, competencyId, 3, userId, "Phase 2 Test Engineer"),
            "engineer refreshes the pending proposal")) {
        return 12;
    }
    production = pendingFor(repository, engineerId, "production");
    if (!require(!production.id.isEmpty() && !production.rowVersion.isEmpty(),
            "refreshed proposal has a new concurrency token") ||
        !require(repository.decide(
            production.id, production.rowVersion, "approved", QString(), "admin", "admin"),
            "refreshed proposal can be approved") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 3,
            "approved refreshed proposal updates the official score")) {
        qCritical().noquote() << repository.lastError();
        return 12;
    }

    if (!require(repository.submitCoreSkill(
            engineerId, categoryId, skillId, 2, userId, "Phase 2 Test Engineer"),
            "core-skill proposal is submitted")) {
        return 13;
    }
    AssessmentSubmission core = pendingFor(repository, engineerId, "core");
    if (!require(repository.decide(
            core.id, core.rowVersion, "approved", "Practical check complete", "admin", "admin"),
            "manager can approve a core-skill proposal") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.core_skill_assessments WHERE engineer_id=? AND skill_id=?",
            {engineerId, skillId}) == 2,
            "approval updates the official core-skill score")) {
        qCritical().noquote() << repository.lastError();
        return 14;
    }

    if (!require(repository.saveOfficialProduction(
            engineerId, areaId, machineId, competencyId, true, 3, 1, "admin", "admin"),
            "manager direct production edit is saved transactionally") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 1,
            "manager direct production edit updates the official score") ||
        !require(!repository.saveOfficialProduction(
            engineerId, areaId, machineId, competencyId, true, 3, 2, "admin", "admin"),
            "stale manager direct production edit is rejected") ||
        !require(repository.lastError().startsWith("Conflict:"),
            "stale manager direct edit reports a conflict") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.assessments WHERE engineer_id=? AND competency_id=?",
            {engineerId, competencyId}) == 1,
            "stale manager edit does not overwrite the official score")) {
        qCritical().noquote() << repository.lastError();
        return 15;
    }

    OfficialCoreScoreChange coreChange;
    coreChange.categoryId = categoryId;
    coreChange.skillId = skillId;
    coreChange.expectedHasScore = true;
    coreChange.expectedScore = 2;
    coreChange.newScore = 3;
    if (!require(repository.saveOfficialCoreSkills(
            engineerId, {coreChange}, "admin", "admin"),
            "manager core-skill changes are saved as one transaction") ||
        !require(scalarInt(db,
            "SELECT score FROM dbo.core_skill_assessments WHERE engineer_id=? AND skill_id=?",
            {engineerId, skillId}) == 3,
            "manager core-skill batch updates the official score") ||
        !require(scalarInt(db,
            "SELECT COUNT(*) FROM dbo.audit_logs WHERE action LIKE 'assessment_submission_%'") >= 8,
            "submission workflow writes audit records") ||
        !require(scalarInt(db,
            "SELECT COUNT(*) FROM dbo.audit_logs WHERE action IN "
            "('assessment_score_manager_updated','core_skill_score_manager_updated')") == 2,
            "manager direct edits write audit records") ||
        !require(scalarInt(db,
            "SELECT COUNT(*) FROM dbo.notifications WHERE engineer_id=? OR recipient_user_id='admin'",
            {engineerId}) >= 8,
            "submission workflow writes manager and engineer notifications")) {
        qCritical().noquote() << repository.lastError();
        return 16;
    }

    qInfo() << "Assessment submission integration workflow passed.";
    manager.disconnect();
    return 0;
}
