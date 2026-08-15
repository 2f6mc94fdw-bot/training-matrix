#ifndef ASSESSMENTSUBMISSIONREPOSITORY_H
#define ASSESSMENTSUBMISSIONREPOSITORY_H

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QVariant>

struct AssessmentSubmission
{
    QString id;
    QString reviewKey;
    QString assessmentType;
    QString engineerId;
    int productionAreaId = 0;
    int machineId = 0;
    int competencyId = 0;
    QString categoryId;
    QString skillId;
    int proposedScore = 0;
    bool hasBaseScore = false;
    int baseScore = 0;
    QString submissionNote;
    QString submittedByUserId;
    QString submittedByName;
    QDateTime submittedAt;
    QString decision;
    QString reviewNote;
    QString reviewedByUserId;
    QString reviewedByName;
    QDateTime reviewedAt;
    QDateTime updatedAt;
    QByteArray rowVersion;

    QString engineerName;
    QString shift;
    QString areaName;
    QString machineName;
    QString competencyName;
};

struct OfficialCoreScoreChange
{
    QString categoryId;
    QString skillId;
    bool expectedHasScore = false;
    int expectedScore = 0;
    int newScore = 0;
};

class AssessmentSubmissionRepository
{
public:
    QList<AssessmentSubmission> findAll();
    QList<AssessmentSubmission> findPendingByEngineer(const QString& engineerId,
                                                       const QString& assessmentType = QString());

    bool submitProduction(const QString& engineerId,
                          int productionAreaId,
                          int machineId,
                          int competencyId,
                          int proposedScore,
                          const QString& submittedByUserId,
                          const QString& submittedByName,
                          const QString& note = QString());

    bool submitCoreSkill(const QString& engineerId,
                         const QString& categoryId,
                         const QString& skillId,
                         int proposedScore,
                         const QString& submittedByUserId,
                         const QString& submittedByName,
                         const QString& note = QString());

    bool decide(const QString& submissionId,
                const QByteArray& expectedRowVersion,
                const QString& decision,
                const QString& reviewNote,
                const QString& reviewedByUserId,
                const QString& reviewedByName);

    bool saveOfficialProduction(const QString& engineerId,
                                int productionAreaId,
                                int machineId,
                                int competencyId,
                                bool expectedHasScore,
                                int expectedScore,
                                int newScore,
                                const QString& managerUserId,
                                const QString& managerName);

    bool saveOfficialCoreSkills(const QString& engineerId,
                                const QList<OfficialCoreScoreChange>& changes,
                                const QString& managerUserId,
                                const QString& managerName);

    QString lastError() const { return lastError_; }

private:
    bool submit(const AssessmentSubmission& submission);
    QList<AssessmentSubmission> find(const QString& whereClause,
                                     const QList<QVariant>& bindings);

    QString lastError_;
};

#endif // ASSESSMENTSUBMISSIONREPOSITORY_H
