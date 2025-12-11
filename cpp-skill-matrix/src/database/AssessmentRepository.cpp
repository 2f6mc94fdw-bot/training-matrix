#include "AssessmentRepository.h"

AssessmentRepository::AssessmentRepository() : lastError_("") {}
AssessmentRepository::~AssessmentRepository() {}

QList<Assessment> AssessmentRepository::findAll() { return QList<Assessment>(); }
QList<Assessment> AssessmentRepository::findByEngineer(const QString& engineerId) { return QList<Assessment>(); }
Assessment AssessmentRepository::findById(int id) { return Assessment(); }
bool AssessmentRepository::saveOrUpdate(Assessment& assessment) { return false; }
bool AssessmentRepository::remove(int id) { return false; }
