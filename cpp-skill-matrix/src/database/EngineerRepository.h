#ifndef ENGINEERREPOSITORY_H
#define ENGINEERREPOSITORY_H

#include "../models/Engineer.h"
#include <QList>

class EngineerRepository
{
public:
    EngineerRepository();
    ~EngineerRepository();

    QList<Engineer> findAll();
    QList<Engineer> findByShift(const QString& shift);
    Engineer findById(const QString& id);
    bool save(Engineer& engineer);
    bool update(const Engineer& engineer);
    bool remove(const QString& id);

    QString lastError() const { return lastError_; }

private:
    QString lastError_;
};

#endif // ENGINEERREPOSITORY_H
