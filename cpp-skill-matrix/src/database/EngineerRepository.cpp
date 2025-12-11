#include "EngineerRepository.h"
#include "../utils/Logger.h"

EngineerRepository::EngineerRepository() : lastError_("") {}
EngineerRepository::~EngineerRepository() {}

QList<Engineer> EngineerRepository::findAll()
{
    Logger::instance().debug("EngineerRepository", "findAll() - stub");
    return QList<Engineer>();
}

QList<Engineer> EngineerRepository::findByShift(const QString& shift)
{
    Logger::instance().debug("EngineerRepository", "findByShift() - stub");
    return QList<Engineer>();
}

Engineer EngineerRepository::findById(const QString& id)
{
    Logger::instance().debug("EngineerRepository", "findById() - stub");
    return Engineer();
}

bool EngineerRepository::save(Engineer& engineer)
{
    Logger::instance().debug("EngineerRepository", "save() - stub");
    return false;
}

bool EngineerRepository::update(const Engineer& engineer)
{
    Logger::instance().debug("EngineerRepository", "update() - stub");
    return false;
}

bool EngineerRepository::remove(const QString& id)
{
    Logger::instance().debug("EngineerRepository", "remove() - stub");
    return false;
}
