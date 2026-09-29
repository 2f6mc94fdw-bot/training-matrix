#ifndef DEVELOPMENTPLANREPOSITORY_H
#define DEVELOPMENTPLANREPOSITORY_H

#include <QDate>
#include <QDateTime>
#include <QList>
#include <QString>

struct DevelopmentPlanItem {
    int id = 0;
    QString engineerId;
    int productionAreaId = 0;
    int machineId = 0;
    int competencyId = 0;
    QString productionAreaName;
    QString machineName;
    QString competencyName;
    int currentScore = 0;
    int targetScore = 2;
    QDate dueDate;
    QString managerNotes;
    QString guidance;
    QString status = "active";
    QString createdByUserId;
    QString createdByName;
    QDateTime createdAt;
    QDateTime updatedAt;
};

class DevelopmentPlanRepository
{
public:
    DevelopmentPlanRepository();

    bool ensureSchema();
    QList<DevelopmentPlanItem> findByEngineer(const QString& engineerId,
                                               const QString& status = QString());
    bool create(const DevelopmentPlanItem& item);
    bool updateStatus(int id, const QString& status);
    bool remove(int id);
    QString lastError() const { return lastError_; }

private:
    QString lastError_;
    bool schemaEnsured_ = false;
};

#endif // DEVELOPMENTPLANREPOSITORY_H
