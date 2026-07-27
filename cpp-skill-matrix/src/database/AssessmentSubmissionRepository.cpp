#include "AssessmentSubmissionRepository.h"

#include "DatabaseManager.h"
#include "../utils/Crypto.h"
#include "../utils/Logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <algorithm>

namespace {
QString productionReviewKey(const QString& engineerId, int areaId, int machineId, int competencyId)
{
    return QString("production|%1|%2|%3|%4")
        .arg(engineerId)
        .arg(areaId)
        .arg(machineId)
        .arg(competencyId);
}

QString coreReviewKey(const QString& engineerId, const QString& categoryId, const QString& skillId)
{
    return QString("core|%1|%2|%3").arg(engineerId, categoryId, skillId);
}

QString jsonDetails(const AssessmentSubmission& submission,
                    const QString& event,
                    const QString& decision = QString())
{
    QJsonObject details;
    details["event"] = event;
    details["submissionId"] = submission.id;
    details["assessmentType"] = submission.assessmentType;
    details["engineerId"] = submission.engineerId;
    details["proposedScore"] = submission.proposedScore;
    details["baseScore"] = submission.hasBaseScore
        ? QJsonValue(submission.baseScore)
        : QJsonValue(QJsonValue::Null);
    if (!decision.isEmpty()) {
        details["decision"] = decision;
    }
    if (submission.assessmentType == "production") {
        details["productionAreaId"] = submission.productionAreaId;
        details["machineId"] = submission.machineId;
        details["competencyId"] = submission.competencyId;
    } else {
        details["categoryId"] = submission.categoryId;
        details["skillId"] = submission.skillId;
    }
    return QString::fromUtf8(QJsonDocument(details).toJson(QJsonDocument::Compact));
}

bool insertAudit(QSqlDatabase& db,
                 const QString& userId,
                 const QString& action,
                 const QString& details,
                 QString& error)
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO dbo.audit_logs (id, timestamp, user_id, action, details, created_at) "
        "VALUES (?, SYSUTCDATETIME(), ?, ?, ?, SYSUTCDATETIME())");
    query.addBindValue(Crypto::generateId("audit"));
    query.addBindValue(userId.isEmpty() ? QVariant() : QVariant(userId));
    query.addBindValue(action);
    query.addBindValue(details);
    if (!query.exec()) {
        error = query.lastError().text();
        return false;
    }
    return true;
}

bool notifyAdmins(QSqlDatabase& db,
                  const AssessmentSubmission& submission,
                  const QString& title,
                  const QString& message,
                  QString& error)
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO dbo.notifications "
        "(id, engineer_id, recipient_user_id, title, message, is_read, created_at, created_by_user_id, created_by_name) "
        "SELECT CONVERT(NVARCHAR(50), NEWID()), NULL, u.id, ?, ?, 0, SYSUTCDATETIME(), ?, ? "
        "FROM dbo.users u WHERE u.role = 'admin'");
    query.addBindValue(title);
    query.addBindValue(message);
    query.addBindValue(submission.submittedByUserId.isEmpty()
        ? QVariant() : QVariant(submission.submittedByUserId));
    query.addBindValue(submission.submittedByName.isEmpty()
        ? QVariant() : QVariant(submission.submittedByName));
    if (!query.exec()) {
        error = query.lastError().text();
        return false;
    }
    return true;
}

bool notifyEngineer(QSqlDatabase& db,
                    const AssessmentSubmission& submission,
                    const QString& title,
                    const QString& message,
                    const QString& reviewedByUserId,
                    const QString& reviewedByName,
                    QString& error)
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO dbo.notifications "
        "(id, engineer_id, recipient_user_id, title, message, is_read, created_at, created_by_user_id, created_by_name) "
        "VALUES (?, ?, NULL, ?, ?, 0, SYSUTCDATETIME(), ?, ?)");
    query.addBindValue(Crypto::generateId("note"));
    query.addBindValue(submission.engineerId);
    query.addBindValue(title);
    query.addBindValue(message);
    query.addBindValue(reviewedByUserId.isEmpty() ? QVariant() : QVariant(reviewedByUserId));
    query.addBindValue(reviewedByName.isEmpty() ? QVariant() : QVariant(reviewedByName));
    if (!query.exec()) {
        error = query.lastError().text();
        return false;
    }
    return true;
}
}

QList<AssessmentSubmission> AssessmentSubmissionRepository::findAll()
{
    return find(QString(), {});
}

QList<AssessmentSubmission> AssessmentSubmissionRepository::findPendingByEngineer(
    const QString& engineerId, const QString& assessmentType)
{
    QList<QVariant> bindings = {engineerId};
    QString where = "WHERE s.engineer_id = ? AND s.decision = 'pending' ";
    if (!assessmentType.isEmpty()) {
        where += "AND s.assessment_type = ? ";
        bindings.append(assessmentType);
    }
    return find(where, bindings);
}

QList<AssessmentSubmission> AssessmentSubmissionRepository::find(
    const QString& whereClause, const QList<QVariant>& bindings)
{
    lastError_.clear();
    QList<AssessmentSubmission> submissions;
    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        lastError_ = "Database not connected";
        return submissions;
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT TOP (1000) s.id,s.review_key,s.assessment_type,s.engineer_id,s.production_area_id,s.machine_id,"
        "s.competency_id,s.category_id,s.skill_id,s.proposed_score,s.base_score,s.submission_note,"
        "s.submitted_by_user_id,s.submitted_by_name,s.submitted_at,s.decision,s.review_note,"
        "s.reviewed_by_user_id,s.reviewed_by_name,s.reviewed_at,s.updated_at,s.row_version,"
        "e.name,e.shift,"
        "CASE WHEN s.assessment_type='production' THEN pa.name ELSE 'Core Skills' END,"
        "CASE WHEN s.assessment_type='production' THEN m.name ELSE COALESCE(cat.discipline,cat.name) END,"
        "CASE WHEN s.assessment_type='production' THEN c.name ELSE skill.name END "
        "FROM dbo.assessment_submissions s "
        "JOIN dbo.engineers e ON e.id=s.engineer_id "
        "LEFT JOIN dbo.production_areas pa ON pa.id=s.production_area_id "
        "LEFT JOIN dbo.machines m ON m.id=s.machine_id "
        "LEFT JOIN dbo.competencies c ON c.id=s.competency_id "
        "LEFT JOIN dbo.core_skill_categories cat ON cat.id=s.category_id "
        "LEFT JOIN dbo.core_skills skill ON skill.id=s.skill_id "
        + whereClause +
        "ORDER BY CASE s.decision WHEN 'pending' THEN 0 WHEN 'approved' THEN 1 ELSE 2 END, s.submitted_at DESC");
    for (const QVariant& binding : bindings) {
        query.addBindValue(binding);
    }
    if (!query.exec()) {
        lastError_ = query.lastError().text();
        Logger::instance().error("AssessmentSubmissionRepository", "find failed: " + lastError_);
        return submissions;
    }

    while (query.next()) {
        AssessmentSubmission item;
        item.id = query.value(0).toString();
        item.reviewKey = query.value(1).toString();
        item.assessmentType = query.value(2).toString();
        item.engineerId = query.value(3).toString();
        item.productionAreaId = query.value(4).toInt();
        item.machineId = query.value(5).toInt();
        item.competencyId = query.value(6).toInt();
        item.categoryId = query.value(7).toString();
        item.skillId = query.value(8).toString();
        item.proposedScore = query.value(9).toInt();
        item.hasBaseScore = !query.value(10).isNull();
        item.baseScore = query.value(10).toInt();
        item.submissionNote = query.value(11).toString();
        item.submittedByUserId = query.value(12).toString();
        item.submittedByName = query.value(13).toString();
        item.submittedAt = query.value(14).toDateTime();
        item.decision = query.value(15).toString();
        item.reviewNote = query.value(16).toString();
        item.reviewedByUserId = query.value(17).toString();
        item.reviewedByName = query.value(18).toString();
        item.reviewedAt = query.value(19).toDateTime();
        item.updatedAt = query.value(20).toDateTime();
        item.rowVersion = query.value(21).toByteArray();
        item.engineerName = query.value(22).toString();
        item.shift = query.value(23).toString();
        item.areaName = query.value(24).toString();
        item.machineName = query.value(25).toString();
        item.competencyName = query.value(26).toString();
        submissions.append(item);
    }
    return submissions;
}

bool AssessmentSubmissionRepository::submitProduction(
    const QString& engineerId,
    int productionAreaId,
    int machineId,
    int competencyId,
    int proposedScore,
    const QString& submittedByUserId,
    const QString& submittedByName,
    const QString& note)
{
    AssessmentSubmission submission;
    submission.reviewKey = productionReviewKey(engineerId, productionAreaId, machineId, competencyId);
    submission.assessmentType = "production";
    submission.engineerId = engineerId;
    submission.productionAreaId = productionAreaId;
    submission.machineId = machineId;
    submission.competencyId = competencyId;
    submission.proposedScore = proposedScore;
    submission.submittedByUserId = submittedByUserId;
    submission.submittedByName = submittedByName;
    submission.submissionNote = note;
    return submit(submission);
}

bool AssessmentSubmissionRepository::submitCoreSkill(
    const QString& engineerId,
    const QString& categoryId,
    const QString& skillId,
    int proposedScore,
    const QString& submittedByUserId,
    const QString& submittedByName,
    const QString& note)
{
    AssessmentSubmission submission;
    submission.reviewKey = coreReviewKey(engineerId, categoryId, skillId);
    submission.assessmentType = "core";
    submission.engineerId = engineerId;
    submission.categoryId = categoryId;
    submission.skillId = skillId;
    submission.proposedScore = proposedScore;
    submission.submittedByUserId = submittedByUserId;
    submission.submittedByName = submittedByName;
    submission.submissionNote = note;
    return submit(submission);
}

bool AssessmentSubmissionRepository::submit(const AssessmentSubmission& requestedSubmission)
{
    lastError_.clear();
    if (requestedSubmission.submittedByUserId.isEmpty()) {
        lastError_ = "An authenticated engineer is required to submit an assessment";
        return false;
    }
    if (requestedSubmission.proposedScore < 0 || requestedSubmission.proposedScore > 3) {
        lastError_ = "Proposed score must be between 0 and 3";
        return false;
    }

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen() || !db.transaction()) {
        lastError_ = db.isOpen() ? db.lastError().text() : "Database not connected";
        return false;
    }

    AssessmentSubmission submission = requestedSubmission;
    QSqlQuery baseQuery(db);
    if (submission.assessmentType == "production") {
        baseQuery.prepare(
            "SELECT score FROM dbo.assessments WITH (UPDLOCK,HOLDLOCK) "
            "WHERE engineer_id=? AND production_area_id=? AND machine_id=? AND competency_id=?");
        baseQuery.addBindValue(submission.engineerId);
        baseQuery.addBindValue(submission.productionAreaId);
        baseQuery.addBindValue(submission.machineId);
        baseQuery.addBindValue(submission.competencyId);
    } else {
        baseQuery.prepare(
            "SELECT score FROM dbo.core_skill_assessments WITH (UPDLOCK,HOLDLOCK) "
            "WHERE engineer_id=? AND category_id=? AND skill_id=?");
        baseQuery.addBindValue(submission.engineerId);
        baseQuery.addBindValue(submission.categoryId);
        baseQuery.addBindValue(submission.skillId);
    }
    if (!baseQuery.exec()) {
        lastError_ = baseQuery.lastError().text();
        db.rollback();
        return false;
    }
    submission.hasBaseScore = baseQuery.next();
    if (submission.hasBaseScore) {
        submission.baseScore = baseQuery.value(0).toInt();
    }

    QSqlQuery pendingQuery(db);
    pendingQuery.prepare(
        "SELECT id FROM dbo.assessment_submissions WITH (UPDLOCK,HOLDLOCK) "
        "WHERE review_key=? AND decision='pending'");
    pendingQuery.addBindValue(submission.reviewKey);
    if (!pendingQuery.exec()) {
        lastError_ = pendingQuery.lastError().text();
        db.rollback();
        return false;
    }

    const bool updatesPending = pendingQuery.next();
    submission.id = updatesPending ? pendingQuery.value(0).toString() : Crypto::generateId("submission");

    QSqlQuery saveQuery(db);
    if (updatesPending) {
        saveQuery.prepare(
            "UPDATE dbo.assessment_submissions SET proposed_score=?,base_score=?,submission_note=?,"
            "submitted_by_user_id=?,submitted_by_name=?,submitted_at=SYSUTCDATETIME(),updated_at=SYSUTCDATETIME() "
            "WHERE id=? AND decision='pending'");
        saveQuery.addBindValue(submission.proposedScore);
        saveQuery.addBindValue(submission.hasBaseScore ? QVariant(submission.baseScore) : QVariant());
        saveQuery.addBindValue(submission.submissionNote.isEmpty() ? QVariant() : QVariant(submission.submissionNote));
        saveQuery.addBindValue(submission.submittedByUserId.isEmpty() ? QVariant() : QVariant(submission.submittedByUserId));
        saveQuery.addBindValue(submission.submittedByName.isEmpty() ? QVariant() : QVariant(submission.submittedByName));
        saveQuery.addBindValue(submission.id);
    } else {
        saveQuery.prepare(
            "INSERT INTO dbo.assessment_submissions "
            "(id,review_key,assessment_type,engineer_id,production_area_id,machine_id,competency_id,"
            "category_id,skill_id,proposed_score,base_score,submission_note,submitted_by_user_id,submitted_by_name,"
            "submitted_at,decision,updated_at) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,SYSUTCDATETIME(),'pending',SYSUTCDATETIME())");
        saveQuery.addBindValue(submission.id);
        saveQuery.addBindValue(submission.reviewKey);
        saveQuery.addBindValue(submission.assessmentType);
        saveQuery.addBindValue(submission.engineerId);
        saveQuery.addBindValue(submission.assessmentType == "production" ? QVariant(submission.productionAreaId) : QVariant());
        saveQuery.addBindValue(submission.assessmentType == "production" ? QVariant(submission.machineId) : QVariant());
        saveQuery.addBindValue(submission.assessmentType == "production" ? QVariant(submission.competencyId) : QVariant());
        saveQuery.addBindValue(submission.assessmentType == "core" ? QVariant(submission.categoryId) : QVariant());
        saveQuery.addBindValue(submission.assessmentType == "core" ? QVariant(submission.skillId) : QVariant());
        saveQuery.addBindValue(submission.proposedScore);
        saveQuery.addBindValue(submission.hasBaseScore ? QVariant(submission.baseScore) : QVariant());
        saveQuery.addBindValue(submission.submissionNote.isEmpty() ? QVariant() : QVariant(submission.submissionNote));
        saveQuery.addBindValue(submission.submittedByUserId.isEmpty() ? QVariant() : QVariant(submission.submittedByUserId));
        saveQuery.addBindValue(submission.submittedByName.isEmpty() ? QVariant() : QVariant(submission.submittedByName));
    }
    if (!saveQuery.exec()) {
        lastError_ = saveQuery.lastError().text();
        db.rollback();
        return false;
    }

    const QString event = updatesPending ? "assessment_submission_updated" : "assessment_submission_created";
    if (!insertAudit(db, submission.submittedByUserId, event, jsonDetails(submission, event), lastError_) ||
        !notifyAdmins(db, submission,
            "Assessment Awaiting Approval",
            QString("%1 submitted a %2 assessment score of %3.")
                .arg(submission.submittedByName.isEmpty() ? "An engineer" : submission.submittedByName)
                .arg(submission.assessmentType == "production" ? "production" : "core skill")
                .arg(submission.proposedScore),
            lastError_)) {
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        lastError_ = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool AssessmentSubmissionRepository::saveOfficialProduction(
    const QString& engineerId,
    int productionAreaId,
    int machineId,
    int competencyId,
    bool expectedHasScore,
    int expectedScore,
    int newScore,
    const QString& managerUserId,
    const QString& managerName)
{
    lastError_.clear();
    if (managerUserId.isEmpty()) {
        lastError_ = "An authenticated manager is required to update an official score";
        return false;
    }
    if (engineerId.isEmpty() || productionAreaId <= 0 || machineId <= 0 || competencyId <= 0 ||
        expectedScore < 0 || expectedScore > 3 || newScore < 0 || newScore > 3) {
        lastError_ = "Invalid production assessment update";
        return false;
    }

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen() || !db.transaction()) {
        lastError_ = db.isOpen() ? db.lastError().text() : "Database not connected";
        return false;
    }

    QSqlQuery currentQuery(db);
    currentQuery.prepare(
        "SELECT score FROM dbo.assessments WITH (UPDLOCK,HOLDLOCK) "
        "WHERE engineer_id=? AND production_area_id=? AND machine_id=? AND competency_id=?");
    currentQuery.addBindValue(engineerId);
    currentQuery.addBindValue(productionAreaId);
    currentQuery.addBindValue(machineId);
    currentQuery.addBindValue(competencyId);
    if (!currentQuery.exec()) {
        lastError_ = currentQuery.lastError().text();
        db.rollback();
        return false;
    }

    const bool hasCurrentScore = currentQuery.next();
    const int currentScore = hasCurrentScore ? currentQuery.value(0).toInt() : 0;
    if (hasCurrentScore != expectedHasScore || (hasCurrentScore && currentScore != expectedScore)) {
        lastError_ = "Conflict: this official score changed after the page loaded. Refresh and try again.";
        db.rollback();
        return false;
    }

    QSqlQuery saveQuery(db);
    if (hasCurrentScore) {
        saveQuery.prepare(
            "UPDATE dbo.assessments SET score=?,updated_at=SYSUTCDATETIME() "
            "WHERE engineer_id=? AND production_area_id=? AND machine_id=? AND competency_id=?");
        saveQuery.addBindValue(newScore);
        saveQuery.addBindValue(engineerId);
        saveQuery.addBindValue(productionAreaId);
        saveQuery.addBindValue(machineId);
        saveQuery.addBindValue(competencyId);
    } else {
        saveQuery.prepare(
            "INSERT INTO dbo.assessments "
            "(engineer_id,production_area_id,machine_id,competency_id,score,created_at,updated_at) "
            "VALUES (?,?,?,?,?,SYSUTCDATETIME(),SYSUTCDATETIME())");
        saveQuery.addBindValue(engineerId);
        saveQuery.addBindValue(productionAreaId);
        saveQuery.addBindValue(machineId);
        saveQuery.addBindValue(competencyId);
        saveQuery.addBindValue(newScore);
    }
    if (!saveQuery.exec()) {
        lastError_ = saveQuery.lastError().text();
        db.rollback();
        return false;
    }

    AssessmentSubmission auditItem;
    auditItem.assessmentType = "production";
    auditItem.engineerId = engineerId;
    auditItem.productionAreaId = productionAreaId;
    auditItem.machineId = machineId;
    auditItem.competencyId = competencyId;
    auditItem.hasBaseScore = hasCurrentScore;
    auditItem.baseScore = currentScore;
    auditItem.proposedScore = newScore;
    const QString message = QString(
        "Your production competency score was adjusted from %1 to %2 by %3.")
        .arg(hasCurrentScore ? QString::number(currentScore) : "not assessed")
        .arg(newScore)
        .arg(managerName.isEmpty() ? "a manager" : managerName);
    if (!insertAudit(db, managerUserId, "assessment_score_manager_updated",
            jsonDetails(auditItem, "assessment_score_manager_updated"), lastError_) ||
        !notifyEngineer(db, auditItem, "Assessment Score Updated", message,
            managerUserId, managerName, lastError_)) {
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        lastError_ = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool AssessmentSubmissionRepository::saveOfficialCoreSkills(
    const QString& engineerId,
    const QList<OfficialCoreScoreChange>& changes,
    const QString& managerUserId,
    const QString& managerName)
{
    lastError_.clear();
    if (managerUserId.isEmpty()) {
        lastError_ = "An authenticated manager is required to update official scores";
        return false;
    }
    if (engineerId.isEmpty()) {
        lastError_ = "Engineer ID is required";
        return false;
    }
    if (changes.isEmpty()) {
        return true;
    }
    QList<OfficialCoreScoreChange> sortedChanges = changes;
    std::sort(sortedChanges.begin(), sortedChanges.end(),
        [](const OfficialCoreScoreChange& left, const OfficialCoreScoreChange& right) {
            if (left.categoryId != right.categoryId) {
                return left.categoryId < right.categoryId;
            }
            return left.skillId < right.skillId;
        });

    for (const OfficialCoreScoreChange& change : sortedChanges) {
        if (change.categoryId.isEmpty() || change.skillId.isEmpty() ||
            change.expectedScore < 0 || change.expectedScore > 3 ||
            change.newScore < 0 || change.newScore > 3) {
            lastError_ = "Invalid core-skill assessment update";
            return false;
        }
    }

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen() || !db.transaction()) {
        lastError_ = db.isOpen() ? db.lastError().text() : "Database not connected";
        return false;
    }

    for (const OfficialCoreScoreChange& change : sortedChanges) {
        QSqlQuery currentQuery(db);
        currentQuery.prepare(
            "SELECT score FROM dbo.core_skill_assessments WITH (UPDLOCK,HOLDLOCK) "
            "WHERE engineer_id=? AND category_id=? AND skill_id=?");
        currentQuery.addBindValue(engineerId);
        currentQuery.addBindValue(change.categoryId);
        currentQuery.addBindValue(change.skillId);
        if (!currentQuery.exec()) {
            lastError_ = currentQuery.lastError().text();
            db.rollback();
            return false;
        }

        const bool hasCurrentScore = currentQuery.next();
        const int currentScore = hasCurrentScore ? currentQuery.value(0).toInt() : 0;
        if (hasCurrentScore != change.expectedHasScore ||
            (hasCurrentScore && currentScore != change.expectedScore)) {
            lastError_ = "Conflict: a core-skill score changed after the page loaded. Refresh and try again.";
            db.rollback();
            return false;
        }

        QSqlQuery saveQuery(db);
        if (hasCurrentScore) {
            saveQuery.prepare(
                "UPDATE dbo.core_skill_assessments SET score=?,updated_at=SYSUTCDATETIME() "
                "WHERE engineer_id=? AND category_id=? AND skill_id=?");
            saveQuery.addBindValue(change.newScore);
            saveQuery.addBindValue(engineerId);
            saveQuery.addBindValue(change.categoryId);
            saveQuery.addBindValue(change.skillId);
        } else {
            saveQuery.prepare(
                "INSERT INTO dbo.core_skill_assessments "
                "(engineer_id,category_id,skill_id,score,created_at,updated_at) "
                "VALUES (?,?,?,?,SYSUTCDATETIME(),SYSUTCDATETIME())");
            saveQuery.addBindValue(engineerId);
            saveQuery.addBindValue(change.categoryId);
            saveQuery.addBindValue(change.skillId);
            saveQuery.addBindValue(change.newScore);
        }
        if (!saveQuery.exec()) {
            lastError_ = saveQuery.lastError().text();
            db.rollback();
            return false;
        }

        AssessmentSubmission auditItem;
        auditItem.assessmentType = "core";
        auditItem.engineerId = engineerId;
        auditItem.categoryId = change.categoryId;
        auditItem.skillId = change.skillId;
        auditItem.hasBaseScore = hasCurrentScore;
        auditItem.baseScore = currentScore;
        auditItem.proposedScore = change.newScore;
        if (!insertAudit(db, managerUserId, "core_skill_score_manager_updated",
                jsonDetails(auditItem, "core_skill_score_manager_updated"), lastError_)) {
            db.rollback();
            return false;
        }
    }

    AssessmentSubmission notificationItem;
    notificationItem.engineerId = engineerId;
    const QString message = QString("%1 core-skill score%2 updated by %3.")
        .arg(changes.size())
        .arg(changes.size() == 1 ? " was" : "s were")
        .arg(managerName.isEmpty() ? "a manager" : managerName);
    if (!notifyEngineer(db, notificationItem, "Core Skills Updated", message,
            managerUserId, managerName, lastError_)) {
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        lastError_ = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}

bool AssessmentSubmissionRepository::decide(
    const QString& submissionId,
    const QByteArray& expectedRowVersion,
    const QString& decision,
    const QString& reviewNote,
    const QString& reviewedByUserId,
    const QString& reviewedByName)
{
    lastError_.clear();
    if (reviewedByUserId.isEmpty()) {
        lastError_ = "An authenticated manager is required to review a submission";
        return false;
    }
    if (decision != "approved" && decision != "rejected") {
        lastError_ = "Decision must be approved or rejected";
        return false;
    }
    if (decision == "rejected" && reviewNote.trimmed().isEmpty()) {
        lastError_ = "A rejection reason is required";
        return false;
    }

    QSqlDatabase& db = DatabaseManager::instance().database();
    if (!db.isOpen() || !db.transaction()) {
        lastError_ = db.isOpen() ? db.lastError().text() : "Database not connected";
        return false;
    }

    QSqlQuery submissionQuery(db);
    submissionQuery.prepare(
        "SELECT assessment_type,engineer_id,production_area_id,machine_id,competency_id,category_id,skill_id,"
        "proposed_score,base_score,submitted_by_user_id,submitted_by_name "
        "FROM dbo.assessment_submissions WITH (UPDLOCK,HOLDLOCK) "
        "WHERE id=? AND row_version=? AND decision='pending'");
    submissionQuery.addBindValue(submissionId);
    submissionQuery.addBindValue(expectedRowVersion);
    if (!submissionQuery.exec()) {
        lastError_ = submissionQuery.lastError().text();
        db.rollback();
        return false;
    }
    if (!submissionQuery.next()) {
        lastError_ = "Conflict: this submission was already changed or reviewed. Refresh and try again.";
        db.rollback();
        return false;
    }

    AssessmentSubmission submission;
    submission.id = submissionId;
    submission.assessmentType = submissionQuery.value(0).toString();
    submission.engineerId = submissionQuery.value(1).toString();
    submission.productionAreaId = submissionQuery.value(2).toInt();
    submission.machineId = submissionQuery.value(3).toInt();
    submission.competencyId = submissionQuery.value(4).toInt();
    submission.categoryId = submissionQuery.value(5).toString();
    submission.skillId = submissionQuery.value(6).toString();
    submission.proposedScore = submissionQuery.value(7).toInt();
    submission.hasBaseScore = !submissionQuery.value(8).isNull();
    submission.baseScore = submissionQuery.value(8).toInt();
    submission.submittedByUserId = submissionQuery.value(9).toString();
    submission.submittedByName = submissionQuery.value(10).toString();

    if (decision == "approved") {
        QSqlQuery officialQuery(db);
        if (submission.assessmentType == "production") {
            officialQuery.prepare(
                "SELECT score FROM dbo.assessments WITH (UPDLOCK,HOLDLOCK) "
                "WHERE engineer_id=? AND production_area_id=? AND machine_id=? AND competency_id=?");
            officialQuery.addBindValue(submission.engineerId);
            officialQuery.addBindValue(submission.productionAreaId);
            officialQuery.addBindValue(submission.machineId);
            officialQuery.addBindValue(submission.competencyId);
        } else {
            officialQuery.prepare(
                "SELECT score FROM dbo.core_skill_assessments WITH (UPDLOCK,HOLDLOCK) "
                "WHERE engineer_id=? AND category_id=? AND skill_id=?");
            officialQuery.addBindValue(submission.engineerId);
            officialQuery.addBindValue(submission.categoryId);
            officialQuery.addBindValue(submission.skillId);
        }
        if (!officialQuery.exec()) {
            lastError_ = officialQuery.lastError().text();
            db.rollback();
            return false;
        }

        const bool hasOfficialScore = officialQuery.next();
        const int currentOfficialScore = hasOfficialScore ? officialQuery.value(0).toInt() : 0;
        if (hasOfficialScore != submission.hasBaseScore ||
            (hasOfficialScore && currentOfficialScore != submission.baseScore)) {
            lastError_ = "Conflict: the official score changed after this submission. Refresh and ask the engineer to resubmit.";
            db.rollback();
            return false;
        }

        QSqlQuery officialSave(db);
        if (submission.assessmentType == "production") {
            if (hasOfficialScore) {
                officialSave.prepare(
                    "UPDATE dbo.assessments SET score=?,updated_at=SYSUTCDATETIME() "
                    "WHERE engineer_id=? AND production_area_id=? AND machine_id=? AND competency_id=?");
                officialSave.addBindValue(submission.proposedScore);
                officialSave.addBindValue(submission.engineerId);
                officialSave.addBindValue(submission.productionAreaId);
                officialSave.addBindValue(submission.machineId);
                officialSave.addBindValue(submission.competencyId);
            } else {
                officialSave.prepare(
                    "INSERT INTO dbo.assessments "
                    "(engineer_id,production_area_id,machine_id,competency_id,score,created_at,updated_at) "
                    "VALUES (?,?,?,?,?,SYSUTCDATETIME(),SYSUTCDATETIME())");
                officialSave.addBindValue(submission.engineerId);
                officialSave.addBindValue(submission.productionAreaId);
                officialSave.addBindValue(submission.machineId);
                officialSave.addBindValue(submission.competencyId);
                officialSave.addBindValue(submission.proposedScore);
            }
        } else {
            if (hasOfficialScore) {
                officialSave.prepare(
                    "UPDATE dbo.core_skill_assessments SET score=?,updated_at=SYSUTCDATETIME() "
                    "WHERE engineer_id=? AND category_id=? AND skill_id=?");
                officialSave.addBindValue(submission.proposedScore);
                officialSave.addBindValue(submission.engineerId);
                officialSave.addBindValue(submission.categoryId);
                officialSave.addBindValue(submission.skillId);
            } else {
                officialSave.prepare(
                    "INSERT INTO dbo.core_skill_assessments "
                    "(engineer_id,category_id,skill_id,score,created_at,updated_at) "
                    "VALUES (?,?,?,?,SYSUTCDATETIME(),SYSUTCDATETIME())");
                officialSave.addBindValue(submission.engineerId);
                officialSave.addBindValue(submission.categoryId);
                officialSave.addBindValue(submission.skillId);
                officialSave.addBindValue(submission.proposedScore);
            }
        }
        if (!officialSave.exec()) {
            lastError_ = officialSave.lastError().text();
            db.rollback();
            return false;
        }
    }

    QSqlQuery decisionQuery(db);
    decisionQuery.prepare(
        "UPDATE dbo.assessment_submissions SET decision=?,review_note=?,reviewed_by_user_id=?,"
        "reviewed_by_name=?,reviewed_at=SYSUTCDATETIME(),updated_at=SYSUTCDATETIME() "
        "WHERE id=? AND row_version=? AND decision='pending'");
    decisionQuery.addBindValue(decision);
    decisionQuery.addBindValue(reviewNote.trimmed().isEmpty() ? QVariant() : QVariant(reviewNote.trimmed()));
    decisionQuery.addBindValue(reviewedByUserId.isEmpty() ? QVariant() : QVariant(reviewedByUserId));
    decisionQuery.addBindValue(reviewedByName.isEmpty() ? QVariant() : QVariant(reviewedByName));
    decisionQuery.addBindValue(submissionId);
    decisionQuery.addBindValue(expectedRowVersion);
    if (!decisionQuery.exec() || decisionQuery.numRowsAffected() != 1) {
        lastError_ = decisionQuery.lastError().isValid()
            ? decisionQuery.lastError().text()
            : "Conflict: this submission changed while the decision was being saved.";
        db.rollback();
        return false;
    }

    const QString action = decision == "approved"
        ? "assessment_submission_approved"
        : "assessment_submission_rejected";
    const QString message = QString("Your %1 assessment submission was %2.%3")
        .arg(submission.assessmentType == "production" ? "production" : "core skill")
        .arg(decision)
        .arg(reviewNote.trimmed().isEmpty() ? QString() : " Manager note: " + reviewNote.trimmed());
    if (!insertAudit(db, reviewedByUserId, action, jsonDetails(submission, action, decision), lastError_) ||
        !notifyEngineer(db, submission, "Assessment Review Update", message,
            reviewedByUserId, reviewedByName, lastError_)) {
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        lastError_ = db.lastError().text();
        db.rollback();
        return false;
    }
    return true;
}
