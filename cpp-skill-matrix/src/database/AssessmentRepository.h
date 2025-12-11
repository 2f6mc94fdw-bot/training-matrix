#ifndef ASSESSMENTREPOSITORY_H
#define ASSESSMENTREPOSITORY_H

#include "../models/Assessment.h"
#include <QList>

class AssessmentRepository
{
public:
    AssessmentRepository();
    ~AssessmentRepository();

    QList<Assessment> findAll();
    QList<Assessment> findByEngineer(const QString& engineerId);
    Assessment findById(int id);
    bool saveOrUpdate(Assessment& assessment); // Upsert
    bool remove(int id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // ASSESSMENTREPOSITORY_H
